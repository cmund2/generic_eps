/**
 * @file
 *
 * Extra scaffolding functions for the generic_eps_app unit test
 */

#ifndef UT_GENERIC_EPS_APP_H
#define UT_GENERIC_EPS_APP_H

/*
 * Pull in the real app’s typedefs so we know what GENERIC_EPS_AppData_t is
 */
#include "generic_eps_app.h"

/*
 * Allow UT access to the global GENERIC_EPS_AppData instance
 */
extern GENERIC_EPS_AppData_t GENERIC_EPS_AppData;

#endif /* UT_GENERIC_EPS_APP_H */
