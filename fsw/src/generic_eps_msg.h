/*******************************************************************************
** File:
**   generic_eps_msg.h
**
** Purpose:
**  Define GENERIC_EPS application commands and telemetry messages
**
*******************************************************************************/
#ifndef _GENERIC_EPS_MSG_H_
#define _GENERIC_EPS_MSG_H_

#include "cfe.h"
#include "generic_eps_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define GENERIC_EPS_NOOP_CC                 0
#define GENERIC_EPS_RESET_COUNTERS_CC       1
#define GENERIC_EPS_ENABLE_CC               2
#define GENERIC_EPS_DISABLE_CC              3
#define GENERIC_EPS_CONFIG_CC               4


/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define GENERIC_EPS_REQ_HK_TLM              0
#define GENERIC_EPS_REQ_DATA_TLM            1


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} GENERIC_EPS_NoArgs_cmd_t;


/*
** GENERIC_EPS write configuration command
*/
typedef struct
{
    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32   DeviceCfg;

} GENERIC_EPS_Config_cmd_t;


/*
** GENERIC_EPS device telemetry definition
*/
typedef struct 
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    GENERIC_EPS_Device_Data_tlm_t Generic_eps;

} OS_PACK GENERIC_EPS_Device_tlm_t;
#define GENERIC_EPS_DEVICE_TLM_LNGTH sizeof ( GENERIC_EPS_Device_tlm_t )


/*
** GENERIC_EPS housekeeping type definition
*/
typedef struct 
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
  
    /*
    ** TODO: Edit and add specific telemetry values to this struct
    */
    uint8   DeviceEnabled;
    GENERIC_EPS_Device_HK_tlm_t DeviceHK;

} OS_PACK GENERIC_EPS_Hk_tlm_t;
#define GENERIC_EPS_HK_TLM_LNGTH sizeof ( GENERIC_EPS_Hk_tlm_t )

#endif /* _GENERIC_EPS_MSG_H_ */
