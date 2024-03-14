/* * ******************************************************************************
	* PROJECT: SMART FAN CONTROL SYSTEM
	* AUTHOR:  ISHA SHARMA
	* COURSE:  EMBEDDED SYSTEMS DESIGN SP'23
	* TOOLS:   STMCUBEIDE
	* DATE:    06-05-2023
	* FILE:    dcmotor.c
	* BRIEF:   This file is used for the functions related to dcm motor and pwm
	*
	* ******************************************************************************
 */

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include "dcmotor.h"

/*
name: init_gpio_pwmpin
description: function to initialise PWM output pin
parameters: none
returns: none
 */
void init_gpio_pwmpin(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable clock for GPIO Port B

    // Configure PB9 as an alternate function output
    GPIOB->MODER |= GPIO_MODER_MODE9_1; // Set to alternate function mode
    GPIOB->AFR[1] |= (2 << GPIO_AFRH_AFSEL9_Pos); // Set alternate function to AF2 (TIM4_CH4)

    // Set output type to push-pull
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT9);

    // Set output speed to high
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED9;

    // Set initial output to low
    GPIOB->BSRR |= GPIO_BSRR_BR9;
}

/*
name: init_motor_pins
description: function to initialise gpio pins for motor direction control
parameters: none
returns: none
 */
void init_motor_pins(void)
{
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // Set PB12 and PB13 as outputs
    GPIOB->MODER |= GPIO_MODER_MODE12_0 | GPIO_MODER_MODE13_0;
    GPIOB->MODER &= ~(GPIO_MODER_MODE12_1 | GPIO_MODER_MODE13_1);

    // Set output type to push-pull
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT12 | GPIO_OTYPER_OT13);

    // Set output speed to high
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED12 | GPIO_OSPEEDR_OSPEED13;

    // Set PB12 and PB13 initial output to low
    GPIOB->BSRR |= GPIO_BSRR_BR12 | GPIO_BSRR_BR13;

    // Set in1 to high and in2 to low
    GPIOB->BSRR |= GPIO_BSRR_BS12;
    GPIOB->BSRR &= ~(GPIO_BSRR_BS13);
}


/*
name: init_pwm_timer
description: function to intiliase pwm
parameters: none
returns: none
 */void init_pwm_timer(void)
{
	// Enable clock for TIM4
	RCC->APB1ENR  |= RCC_APB1ENR_TIM4EN;

	// Set the prescaler value to achieve10 KHz freq with system clock of 84 MHz
	//PSC = (48 MHz / 10 kHz) - 1 = 4799
	TIM4->PSC = 4799;

	// Set the auto-reload value to achieve a frequency of 10 kHz
	//ARR = (48 MHz / (10 kHz * (4799 + 1))) - 1 = 999
	//PWM frequency = (system clock frequency) / (prescaler x (arr + 1))
	//frequency is determined by the arr register
	TIM4->ARR = 999;

	// Set the duty cycle using CCR4 register
	TIM4->CCR4 = duty_cycle;

	// Set output mode to PWM mode 1
	TIM4->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
	TIM4->CCMR2 &= ~(TIM_CCMR2_OC4M_0);

	// Enable output for channel 4
	TIM4->CCER |= TIM_CCER_CC4E;

	// Enable counter for TIM4
	TIM4->CR1 |= TIM_CR1_CEN;
}

