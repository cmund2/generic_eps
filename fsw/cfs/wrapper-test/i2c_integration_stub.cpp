#include "generic_eps_device.h"
#include <generic_eps_hardware_model.hpp>
#include <vector>

using namespace Nos3;

// Stub file that intercepts I2C transactions from the EPS device/app and send it to the simulation model

// Global pointer to sim model instance
Generic_epsHardwareModel* g_sim_model = nullptr;

// C-style interface for the device code
extern "C" int32_t i2c_master_transaction(i2c_bus_info_t *device, uint8_t addr, void *txbuf, uint8_t txlen, void *rxbuf, uint8_t rxlen, uint16_t timeout)
{
    std::vector<uint8_t> request((uint8_t*)txbuf, (uint8_t*)txbuf + txlen);
    std::vector<uint8_t> response;
    // Call the simulation model
    g_sim_model->determine_i2c_response_for_request(request, response);

    // Copy result into rxbuf (limit to rxlen)
    for (uint8_t i = 0; i < rxlen && i < response.size(); ++i)
        ((uint8_t*)rxbuf)[i] = response[i];
    return 0; // Simulate OS_SUCCESS
}