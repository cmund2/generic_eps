#ifndef _GENERIC_EPS_CHECKOUT_DEVICE_CFG_H_
#define _GENERIC_EPS_CHECKOUT_DEVICE_CFG_H_

/*
** GENERIC_EPS Checkout Configuration
*/
#define GENERIC_EPS_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define GENERIC_EPS_CFG_STRING           "/dev/usart_29"
#define GENERIC_EPS_CFG_HANDLE           29 
#define GENERIC_EPS_CFG_BAUDRATE_HZ      115200
#define GENERIC_EPS_CFG_MS_TIMEOUT       250
#define GENERIC_EPS_CFG_DEBUG

#endif /* _GENERIC_EPS_CHECKOUT_DEVICE_CFG_H_ */
