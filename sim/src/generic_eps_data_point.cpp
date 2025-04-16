#include <ItcLogger/Logger.hpp>
#include <generic_eps_data_point.hpp>

#include <sim_coordinate_transformations.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_epsDataPoint::Generic_epsDataPoint(double count) : _not_parsed(false)
    {
        sim_logger->trace("Generic_epsDataPoint::Generic_epsDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        _in_sun = true;
        _sun_vector[0] = count * 0.001;
        _sun_vector[1] = count * 0.002;
        _sun_vector[2] = count * 0.003;
    }

    Generic_epsDataPoint::Generic_epsDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft), _not_parsed(true)
    {
        sim_logger->trace("Generic_epsDataPoint::Generic_epsDataPoint:  42 Constructor executed");

        /* Initialize data */
        _in_sun = true;
        _sun_vector[0] = 0.0;
        _sun_vector[1] = 0.0;
        _sun_vector[2] = 0.0;
    }

    void Generic_epsDataPoint::do_parsing(void) const
    {
        try 
        {   
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
           std::string svb_key, eci_pos_key, qbn_key;
           svb_key.append("SC[").append(std::to_string(_sc)).append("].svb");
           eci_pos_key.append("SC[").append(std::to_string(_sc)).append("].PosR");
           qbn_key.append("SC[").append(std::to_string(_sc)).append("].B[0].qn");

           /* Parse 42 telemetry */
           std::string svb_values = _dp.get_value_for_key(svb_key);
           std::string eci_pos_values = _dp.get_value_for_key(eci_pos_key);
           std::string qbn_values = _dp.get_value_for_key(qbn_key);

           std::vector<double> svb_data;
           std::vector<double> eci_pos_data;
           std::vector<double> qbn_data;

           svb_data.resize(3);
           eci_pos_data.resize(3);
           qbn_data.resize(3);

           parse_double_vector(svb_values, svb_data);
           parse_double_vector(eci_pos_values, eci_pos_data);
           parse_double_vector(qbn_values, qbn_data);
            
           if( svb_data.size() == 0 )
           {
              sim_logger->trace("Generic_epsDataPoint::Generic_epsDataPoint: No Data, continue.");
              return;
           }

           _sun_vector[0] = svb_data[0];
           _sun_vector[1] = svb_data[1];
           _sun_vector[2] = svb_data[2];

           if( eci_pos_data.size() == 0 || qbn_data.size() == 0 )
           {
              sim_logger->trace("Generic_epsDataPoint::Generic_epsDataPoint: No Pos or QBN Data for Eclipse Calc, continue.");
              return;
           }

           std::vector<std::vector<double>> cbn;
           std::vector<double> svn;
           double rmag;
           std::vector<double> r;
           
           SimCoordinateTransformations::Q2C( qbn_data, cbn );
           SimCoordinateTransformations::MTxV( cbn, svb_data, svn );
           rmag = SimCoordinateTransformations::norm( eci_pos_data );
           SimCoordinateTransformations::SxV( (-1.0/rmag), eci_pos_data, r);

           double theta = acos( SimCoordinateTransformations::dot( r, svn ) );

           _in_sun = true; // is in the sun

           if ( (theta < SimCoordinateTransformations::SIM_CONSTANTS.PI/2) && (rmag * sin(theta) < SimCoordinateTransformations::SIM_CONSTANTS.R_plus) ) // simple calculation using sun vector and nominal earth radius... does not account for penumbra, etc.
           {
               _in_sun = false; // false value means in eclipse
           }     

           _not_parsed = false;

           /* Debug print */
           sim_logger->trace("Generic_epsDataPoint::Generic_epsDataPoint:  Parsed svb = %f %f %f; In_Sun: %d", _sun_vector[0], _sun_vector[1], _sun_vector[2], _in_sun);
        } 
        catch(const std::exception& e) 
        {
            /* Report error */
            sim_logger->error("Generic_epsDataPoint::Generic_epsDataPoint:  Parsing exception %s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string Generic_epsDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_epsDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic_eps Data Point:   Valid: ";
        ss << (_in_sun ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Generic_eps Data: "
           << _sun_vector[0]
           << " "
           << _sun_vector[1]
           << " "
           << _sun_vector[2];

        return ss.str();
    }
} /* namespace Nos3 */
