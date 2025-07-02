/*******************************************************************************
** File: coveragetest_generic_eps_device.c
**
** Purpose:
**   Coverage Unit Test skeleton for the GENERIC_EPS device layer.
*******************************************************************************/

#include "generic_eps_app_coveragetest_common.h"  /* Brings in UT_ASSERT, UT_KEY, stubs, etc */

/*--------------------------------------------------------------------
 * Unit Tests for EPS for TDAC Intern Project
 *-------------------------------------------------------------------*/


/*--------------------------------------------------------------------
 * Test GENERIC_EPS Device I²C Error Handling
 *-------------------------------------------------------------------*/

// Test that GENERIC_EPS_RequestHK returns an error message when the I2C bus is broken
void Test_GENERIC_EPS_RequestHK_I2CError(void)
{
    // Initialize a dummy i2c bus and a struct to store telemetry results
    i2c_bus_info_t dev  = {0};   // our fake I²C bus descriptor (fields ignored in stub)
    GENERIC_EPS_Device_HK_tlm_t data = {0};   // where RequestHK would normally store results

    // Stub the I2C call to have a bus failure
    UT_SetDeferredRetcode( UT_KEY(i2c_master_transaction), 1, OS_ERROR);

    int32_t status = GENERIC_EPS_RequestHK(&dev, &data);

    // On I²C failure, RequestHK must not attempt to parse data and should return OS_ERROR
    UtAssert_True(status == OS_ERROR, "GENERIC_EPS_RequestHK() returned OS_ERROR on i2c_master_transaction failure (got %ld)", (long)status);
}







/*
 * Setup / Teardown called before / after every test
 */
void Generic_eps_DeviceUT_Setup(void)
{
    UT_ResetState(0);
}

void Generic_eps_DeviceUT_TearDown(void)
{
    /* nothing to clean up */
}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(GENERIC_EPS_RequestHK_I2CError);
}