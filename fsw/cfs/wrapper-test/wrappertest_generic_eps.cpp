#include "generic_eps_app.h"
#include "generic_eps_device.h"
#include "generic_eps_hardware_model.hpp"
#include <boost/property_tree/ptree.hpp>
#include <iostream>

// Global pointer for stub
Nos3::Generic_epsHardwareModel* g_sim_model = nullptr;

// ---------- Simple test cases ----------
void test_switch7_on(i2c_bus_info_t* device)
{
    GENERIC_EPS_Device_HK_tlm_t hk = {0};
    int status = GENERIC_EPS_CommandSwitch(device, 7, 0xAA, &hk);
    std::cout << "[test_switch7_on] Switch 7: " << (status == OS_SUCCESS ? "ON" : "Failed") << std::endl;
    for (int i = 0; i < 5; ++i) {
        g_sim_model->update_battery_values();  // step the sim model
        GENERIC_EPS_RequestHK(device, &hk);   // get current HK
        double batt_voltage = hk.BatteryVoltage * 0.001;
        std::cout << "T=" << i << " | Batt V=" << batt_voltage << " V\n";
    }
}

void test_switch7_off(i2c_bus_info_t* device)
{
    GENERIC_EPS_Device_HK_tlm_t hk = {0};
    int status = GENERIC_EPS_CommandSwitch(device, 7, 0x00, &hk);
    std::cout << "[test_switch7_off] Switch 7: " << (status == OS_SUCCESS ? "OFF" : "Failed") << std::endl;
    for (int i = 0; i < 5; ++i) {
        g_sim_model->update_battery_values();
        GENERIC_EPS_RequestHK(device, &hk);
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
    config.put("simulator.hardware-model.static-data-file", "solar_vectors.csv"); // name of your file!

    // --- Initialize sim model & set global pointer for stub ---
    Nos3::Generic_epsHardwareModel sim_model(config);
    g_sim_model = &sim_model;

    // --- Device structure for calls ---
    i2c_bus_info_t device;
    device.handle = 1;
    device.addr = 0x2A;
    device.isOpen = I2C_OPEN;
    device.speed = 400000;

    // --- Run your test cases ---
    std::cout << "\n=== Running Switch 7 ON Test ===\n";
    test_switch7_on(&device);

    std::cout << "\n=== Running Switch 7 OFF Test ===\n";
    test_switch7_off(&device);

    std::cout << "\nDone.\n";
    return 0;
}
