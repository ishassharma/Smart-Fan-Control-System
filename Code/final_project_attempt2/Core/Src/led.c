/* * ******************************************************************************
	* PROJECT: SMART FAN CONTROL SYSTEM
	* AUTHOR:  ISHA SHARMA
	* COURSE:  EMBEDDED SYSTEMS DESIGN SP'23
	* TOOLS:   STMCUBEIDE
	* DATE:    06-05-2023
	* FILE:    led.c
	* BRIEF:   This file is used for the functions related to indicator led
	*
	* ******************************************************************************
 */

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include "led.h"

/*
name: init_led
description: function to initialise pin for led
parameters: none
returns: none
 */
void init_led(void)
{
    // Enable the GPIOA clock
	RCC->AHB1ENR |= RCC_GPIOA_EN;

    // Set the PA5 mode of the GPIO pin as output
    GPIOA->MODER |= GPIOA_P0_OUTPUT;

    // Turn off the LED initially
    GPIOA->BSRR |= LED_BSRR_OFF;
}
