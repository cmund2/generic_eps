#include "utgenstub.h"
#include "generic_eps_device.h"

/*
 * ----------------------------------------------------
 * Generated stub for GENERIC_EPS_CRC8()
 * ----------------------------------------------------
 */
uint8_t GENERIC_EPS_CRC8(uint8_t *payload, uint32_t length)
{
    UT_GenStub_SetupReturnBuffer(GENERIC_EPS_CRC8, uint8_t);

    UT_GenStub_AddParam(GENERIC_EPS_CRC8, uint8_t *, payload);
    UT_GenStub_AddParam(GENERIC_EPS_CRC8, uint32_t, length);

    UT_GenStub_Execute(GENERIC_EPS_CRC8, Basic, NULL);

    return UT_GenStub_GetReturnValue(GENERIC_EPS_CRC8, uint8_t);
}

/*
 * ----------------------------------------------------
 * Generated stub for GENERIC_EPS_CommandDevice()
 * ----------------------------------------------------
 */
int32_t GENERIC_EPS_CommandDevice(i2c_bus_info_t *device, uint8_t reg, uint8_t value)
{
    UT_GenStub_SetupReturnBuffer(GENERIC_EPS_CommandDevice, int32_t);

    UT_GenStub_AddParam(GENERIC_EPS_CommandDevice, i2c_bus_info_t *, device);
    UT_GenStub_AddParam(GENERIC_EPS_CommandDevice, uint8_t, reg);
    UT_GenStub_AddParam(GENERIC_EPS_CommandDevice, uint8_t, value);

    UT_GenStub_Execute(GENERIC_EPS_CommandDevice, Basic, NULL);

    return UT_GenStub_GetReturnValue(GENERIC_EPS_CommandDevice, int32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub for GENERIC_EPS_RequestHK()
 * ----------------------------------------------------
 */
int32_t GENERIC_EPS_RequestHK(i2c_bus_info_t *device, GENERIC_EPS_Device_HK_tlm_t *data)
{
    UT_GenStub_SetupReturnBuffer(GENERIC_EPS_RequestHK, int32_t);

    UT_GenStub_AddParam(GENERIC_EPS_RequestHK, i2c_bus_info_t *, device);
    UT_GenStub_AddParam(GENERIC_EPS_RequestHK, GENERIC_EPS_Device_HK_tlm_t *, data);

    UT_GenStub_Execute(GENERIC_EPS_RequestHK, Basic, NULL);

    return UT_GenStub_GetReturnValue(GENERIC_EPS_RequestHK, int32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub for GENERIC_EPS_CommandSwitch()
 * ----------------------------------------------------
 */
int32_t GENERIC_EPS_CommandSwitch(i2c_bus_info_t *device,
                                  uint8_t switch_num,
                                  uint8_t value,
                                  GENERIC_EPS_Device_HK_tlm_t *data)
{
    UT_GenStub_SetupReturnBuffer(GENERIC_EPS_CommandSwitch, int32_t);

    UT_GenStub_AddParam(GENERIC_EPS_CommandSwitch, i2c_bus_info_t *, device);
    UT_GenStub_AddParam(GENERIC_EPS_CommandSwitch, uint8_t, switch_num);
    UT_GenStub_AddParam(GENERIC_EPS_CommandSwitch, uint8_t, value);
    UT_GenStub_AddParam(GENERIC_EPS_CommandSwitch, GENERIC_EPS_Device_HK_tlm_t *, data);

    UT_GenStub_Execute(GENERIC_EPS_CommandSwitch, Basic, NULL);

    return UT_GenStub_GetReturnValue(GENERIC_EPS_CommandSwitch, int32_t);
}