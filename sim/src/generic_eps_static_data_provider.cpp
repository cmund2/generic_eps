#include "generic_static_data_provider.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Nos3 {

    GenericStaticDataProvider::GenericStaticDataProvider(const boost::property_tree::ptree& config)
            : SimIDataProvider(config), _index(0)
    {
        // Correct config key
        std::string filename = config.get("simulator.hardware-model.static-data-file", "solar_vectors.txt");
        std::ifstream infile(filename);
        if (!infile.is_open())
            std::cerr << "Warning: Could not open solar input file: " << filename << std::endl;

        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            double x, y, z;
            if (!(iss >> x >> y >> z)) continue;
            _solar_vectors.push_back({x, y, z});
        }
    }

    boost::shared_ptr<SimIDataPoint> GenericStaticDataProvider::get_data_point(void) const
    {
        std::vector<double> solar{0,0,0};
        if (!_solar_vectors.empty()) {
            solar = _solar_vectors[_index];
            _index = (_index + 1) % _solar_vectors.size();
        }

        class SolarVectorPoint : public SimIDataPoint {
        public:
            SolarVectorPoint(const std::vector<double>& sv) : _sv(sv) {}
            double get_sun_vector_x() const override { return _sv[0]; }
            double get_sun_vector_y() const override { return _sv[1]; }
            double get_sun_vector_z() const override { return _sv[2]; }
        private:
            std::vector<double> _sv;
        };

        return boost::shared_ptr<SimIDataPoint>(new SolarVectorPoint(solar));
    }

}
