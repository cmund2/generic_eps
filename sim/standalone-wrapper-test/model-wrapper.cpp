#include "generic_eps_hardware_model.hpp"
#include <boost/property_tree/ptree.hpp>
#include <iostream>

int main()
{
    boost::property_tree::ptree config;
    config.put("simulator.hardware-model.data-provider.type", "GENERIC_STATIC_DATA_PROVIDER");
    config.put("simulator.hardware-model.static-data-file", "solar_vectors.txt");

    Nos3::Generic_epsHardwareModel model(config);

    for (int i = 0; i < 100; ++i) {
        model.update_battery_values();  // or any other sim logic
        std::cout << "Step " << i << " Voltage: " << model.get_battery_voltage() << " V\n";
    }

    return 0;
}