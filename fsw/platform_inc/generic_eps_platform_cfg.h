/************************************************************************
** File:
**   $Id: generic_eps_platform_cfg.h  $
**
** Purpose:
**  Define generic_eps Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_EPS_PLATFORM_CFG_H_
#define _GENERIC_EPS_PLATFORM_CFG_H_

/*
** Default GENERIC_EPS Configuration
*/
#ifndef GENERIC_EPS_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define GENERIC_EPS_CFG_STRING           "usart_29"
    #define GENERIC_EPS_CFG_HANDLE           29 
    #define GENERIC_EPS_CFG_BAUDRATE_HZ      115200
    #define GENERIC_EPS_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_EPS_CFG_DEBUG
#endif

#endif /* _GENERIC_EPS_PLATFORM_CFG_H_ */
