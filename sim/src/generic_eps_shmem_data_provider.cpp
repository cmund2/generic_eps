#include <generic_eps_shmem_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(GenericEpsShmemDataProvider,"GENERIC_EPS_SHMEM_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    GenericEpsShmemDataProvider::GenericEpsShmemDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("GenericEpsShmemDataProvider::GenericEpsShmemDataProvider:  Constructor executed");
        const std::string shm_name = config.get("simulator.hardware-model.data-provider.shared-memory-name", "Blackboard");
        const size_t shm_size = sizeof(BlackboardData);
        bip::shared_memory_object shm(bip::open_or_create, shm_name.c_str(), bip::read_write);
        shm.truncate(shm_size);
        bip::mapped_region shm_region(shm, bip::read_write);
        _shm_region = std::move(shm_region); // don't let this go out of scope/get destroyed
        _blackboard_data = static_cast<BlackboardData*>(_shm_region.get_address());    
    }

    boost::shared_ptr<SimIDataPoint> GenericEpsShmemDataProvider::get_data_point(void) const
    {
        boost::shared_ptr<Generic_epsDataPoint> dp;
        {
            bip::scoped_lock<bip::interprocess_mutex> lock(_blackboard_data->mutex);
            dp = boost::shared_ptr<Generic_epsDataPoint>(
                new Generic_epsDataPoint(_blackboard_data->svb[0], _blackboard_data->svb[1], _blackboard_data->svb[2]));
            // lock is released when scope ends
        }
        sim_logger->debug("GenericEpsShmemDataProvider::get_data_point: values=%f, %f, %f",
            dp->get_sun_vector_x(), dp->get_sun_vector_y(), dp->get_sun_vector_z());
        return dp;
    }
}
