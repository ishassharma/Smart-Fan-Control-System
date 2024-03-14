/* * ******************************************************************************
	* PROJECT: SMART FAN CONTROL SYSTEM
	* AUTHOR:  ISHA SHARMA
	* COURSE:  EMBEDDED SYSTEMS DESIGN SP'23
	* TOOLS:   STMCUBEIDE
	* DATE:    06-05-2023
	* FILE:    led.h
	* BRIEF:   This header file is used for the functions related to indicator led
	*
	* ******************************************************************************
 */

#ifndef INC_LED_H_
#define INC_LED_H_

void init_led(void);

#define LED_BSRR_ON (0b01) //bsrr register value for led to be lit
#define LED_BSRR_OFF (0b01<<16) //bsrr register value for the led to be turned off
#define RCC_GPIOA_EN (0b01) //enable clock
#define GPIOA_P0_OUTPUT (0b01) //set as output

#endif /* INC_LED_H_ */
