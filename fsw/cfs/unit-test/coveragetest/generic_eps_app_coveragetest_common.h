/**
 * @file
 *
 * Common definitions for all GENERIC_EPS coverage tests
 */

#ifndef GENERIC_EPS_APP_COVERAGETEST_COMMON_H
#define GENERIC_EPS_APP_COVERAGETEST_COMMON_H

/* UT-Assert public APIs */
#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

/* cFE APIs and your EPS app’s headers */
#include "cfe.h"
#include "generic_eps_events.h"
#include "generic_eps_app.h"

/*
 * Macro to call a function and check its int32 return code
 */
#define UT_TEST_FUNCTION_RC(func, exp)                                                               \
    {                                                                                                \
        int32 rcexp = (exp);                                                                         \
        int32 rcact = (func);                                                                        \
        UtAssert_True(rcact == rcexp, "%s (%ld) == %s (%ld)", #func, (long)rcact, #exp, (long)rcexp); \
    }

/*
 * Macro to add a test to the UTF runner
 */
#define ADD_TEST(test) UtTest_Add((Test_##test), GenericEPS_UT_Setup, GenericEPS_UT_TearDown, #test)

/* Setup called before each test */
void GenericEPS_UT_Setup(void);

/* Teardown called after each test */
void GenericEPS_UT_TearDown(void);

#endif /* GENERIC_EPS_APP_COVERAGETEST_COMMON_H */