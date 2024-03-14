/* * ******************************************************************************
	* PROJECT: SMART FAN CONTROL SYSTEM
	* AUTHOR:  ISHA SHARMA
	* COURSE:  EMBEDDED SYSTEMS DESIGN SP'23
	* TOOLS:   STMCUBEIDE
	* DATE:    06-05-2023
	* FILE:    dcmotor.h
	* BRIEF:   This is the header file used for the functions related to dcm motor and pwm
	*
	* ******************************************************************************
 */

#ifndef INC_DCMOTOR_H_
#define INC_DCMOTOR_H_

extern uint32_t duty_cycle;
void init_gpio_pwmpin(void);
void init_motor_pins(void);
void init_pwm_timer(void);
#endif /* INC_DCMOTOR_H_ */
