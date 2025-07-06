#include "generic_eps_app.h"
#include "generic_eps_device.h"
#include "generic_eps_hardware_model.hpp"
#include <boost/property_tree/ptree.hpp>
#include <iostream>

// Global pointer for stub
Nos3::Generic_epsHardwareModel* g_sim_model = nullptr;

void test_switch7_on_and_drain(i2c_bus_info_t* device, int steps)
{
    GENERIC_EPS_Device_HK_tlm_t hk = {0};
    int status = GENERIC_EPS_CommandSwitch(device, 7, 0xAA, &hk);
    std::cout << "[test_switch7_on_and_drain] Switch 7: " << (status == OS_SUCCESS ? "ON" : "Failed") << std::endl;
    for (int i = 0; i < steps; ++i) {
        g_sim_model->update_battery_values();  // Step the sim model (simulate "time passing")
        GENERIC_EPS_RequestHK(device, &hk);   // Get current HK from device
        double batt_voltage = hk.BatteryVoltage * 0.001;
        std::cout << "T=" << i << " | Batt V=" << batt_voltage << " V\n";
    }
}

int main()
{
    // --- Set up sim config to use static data provider and data file ---
    boost::property_tree::ptree config;
    config.put("hardware-model.name", "GENERIC_EPS");
    config.put("simulator.hardware-model.data-provider.type", "GENERIC_STATIC_DATA_PROVIDER");
    config.put("simulator.hardware-model.static-data-file", "solar_vectors.csv"); /

    // --- Initialize sim model & set global pointer for stub ---
    Nos3::Generic_epsHardwareModel sim_model(config);
    g_sim_model = &sim_model;

    // --- Device structure for calls ---
    i2c_bus_info_t device;
    device.handle = 1;
    device.addr = 0x2A;
    device.isOpen = I2C_OPEN;
    device.speed = 400000;

    // --- Run single test case: Switch 7 ON, drain battery ---
    std::cout << "\n=== Running Switch 7 ON + Drain Test ===\n";
    test_switch7_on_and_drain(&device, 100);

    std::cout << "\nDone.\n";
    return 0;
}

