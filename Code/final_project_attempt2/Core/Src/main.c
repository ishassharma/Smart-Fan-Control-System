/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) .
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
 * ******************************************************************************
	* PROJECT: SMART FAN CONTROL SYSTEM
	* AUTHOR:  ISHA SHARMA
	* COURSE:  EMBEDDED SYSTEMS DESIGN SP'23
	* TOOLS:   STMCUBEIDE
	* DATE:    06-05-2023
	* FILE:    main.c
	* BRIEF:   This is the main.c
	*
	* ******************************************************************************
	*
	* References used for the project:
	* [1] https://www.youtube.com/watch?v=M5ddTjrcvEs&t=54s
	* [2] https://microcontrollerslab.com/dc-motor-l293d-motor-driver-ic-arduino-tutorial/
	* [3] https://microcontrollerslab.com/l293d-motor-driver-ic-introduction-pinouts-example/
	* [4] https://www.youtube.com/watch?v=4jcxeJxvi3Y
	* [5] https://www.ti.com/lit/ds/symlink/ads1115.pdf
	* [6] https://www.digikey.com/htmldatasheets/production/2047793/0/0/1/SSD1306.pdf
	* [7] https://thecodeprogram.com/ads1115-with-stm32-cubemx
	* [8] https://cdn-shop.adafruit.com/datasheets/l293d.pdf
	* [9] STM32F411VE Datasheet, Reference manual, and User Manuals
	* [10] https://www.thomasnet.com/articles/instruments-controls/types-of-motor-controllers-and-drives/
	* [11] https://microcontrollerslab.com/analog-to-digital-adc-converter-working/
	* [12] https://deepbluembedded.com/stm32-pwm-example-timer-pwm-mode-tutorial/
	* [13] https://deepbluembedded.com/?s=dc+motor
	* [14] https://controllerstech.com/oled-display-using-i2c-stm32/
	* [15] Professor, TAs and CLassmates
	*
	*
	*
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fonts.h"
#include "ssd1306.h"
#include "dcmotor.h"
#include "led.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define SLAVE_ADDRESS 0x48 //SLAVE ADDRESS FOR I2C SLAVE 1001000

/* Variables*/
unsigned char ADCwrite[6]; //buffer to write to pointer register
int16_t reading_from_pot; //variable for raw reading of pot
float voltage_from_pot; //variable for converted voltage value from the raw reading
const float voltage_conversion = 6.114 / 32768.0; //PGA FOR MORE THAN 4 WAS 6.114, FOR BITS X2
uint32_t duty_cycle; //variable to determine duty cycle for pwm

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  /* initialse pwm, motor direction pins, led and ssd1306 */
  init_gpio_pwmpin();
  init_pwm_timer();
  init_motor_pins();
  init_led();
  SSD1306_Init(); //initialising display

  //display startup message and led startup sequence
  SSD1306_GotoXY(0,0);
  SSD1306_Puts("SMART FAN",&Font_11x18,1);
  SSD1306_GotoXY(0,30);
  SSD1306_Puts("CONTROL SYSTEM",&Font_11x18,1);
  SSD1306_UpdateScreen(); //update display on OLED

  GPIOA->BSRR |= LED_BSRR_ON;
  HAL_Delay(500);
  GPIOA->BSRR |= LED_BSRR_OFF;
  HAL_Delay(500);
  GPIOA->BSRR |= LED_BSRR_ON;
  HAL_Delay(500);
  GPIOA->BSRR |= LED_BSRR_OFF;

  SSD1306_ScrollRight(0x00,0x0f); //scroll right entire screen using pages
  HAL_Delay(2000); //2 sec

  SSD1306_ScrollLeft(0x00,0x0f); //scroll left entire screen using pages
  HAL_Delay(2000); //2 sec

  SSD1306_Stopscroll();
  SSD1306_Clear();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //SET CONFIG REG AND CONVERSION REG OF ADC
	  ADCwrite[0] = 0x01; // CONVERION REG=00, CONFIG REG =01
	  ADCwrite[1]=0xC1; //REGISTER FIELDS: os-1,mux-100,pga,000,mode-1 (11000001) msbs
	  ADCwrite[2]= 0x83; // REGISTER FIELDS: dr-default-100,comp-alldefault 00011 (10000011)
	  HAL_I2C_Master_Transmit(&hi2c2,SLAVE_ADDRESS<<1,ADCwrite,3,100); //hal I2C transmit with timeout=100ms

	  //retrieve data from conversion reg
	  ADCwrite[0] = 0x00; // CONVERION REG=00, CONFIG REG =01
	  HAL_I2C_Master_Transmit(&hi2c2,SLAVE_ADDRESS<<1,ADCwrite,1,100); //hal I2C transmit with timeout=100ms
	  HAL_Delay(20);
	  HAL_I2C_Master_Receive(&hi2c2,SLAVE_ADDRESS<<1,ADCwrite,2,100); //hal I2C receive with timeout=100ms

	  //conversion to voltage, TAKING 8 BITS SHIFTING, OTHER sequentially
	  reading_from_pot = (ADCwrite[0]<<8 | ADCwrite[1]);
	  if (reading_from_pot<0)
	  {
		  reading_from_pot=0;// truncating the negative values just incase
	  }

	  //converted voltage value
	  voltage_from_pot = reading_from_pot * voltage_conversion; //MAX VALUE 4.75, MIN VALUE 0.0

	  //pwm duty cycle updation
	  duty_cycle = (uint32_t)(voltage_from_pot * 1000 / 4.75);
	  TIM4->CCR4 = duty_cycle;

	  if(voltage_from_pot>0.2)
	  {
		    GPIOA->BSRR |= LED_BSRR_ON; //fan on
	  }
	  else
	  {
		    GPIOA->BSRR |= LED_BSRR_OFF; //fan off
	  }

	  if((voltage_from_pot>2.5)&&(voltage_from_pot<4.5))
	  {
		    SSD1306_GotoXY(0,0);
		    SSD1306_Puts("SPEED: 2",&Font_11x18,1);
		    SSD1306_UpdateScreen(); //update display on OLED
		    HAL_Delay(500);
		    SSD1306_Clear();

	  }
	  else if(voltage_from_pot>4.5)
	  {
		    SSD1306_GotoXY(0,0);
		    SSD1306_Puts("MAX SPEED",&Font_11x18,1);
		    SSD1306_UpdateScreen(); //update display on OLED
		    HAL_Delay(500);
		    SSD1306_Clear();

	  }
	  else if((voltage_from_pot>0.2)&&(voltage_from_pot<2.5))
	  {
		    SSD1306_GotoXY(0,0);
		    SSD1306_Puts("SPEED: 1",&Font_11x18,1);
		    SSD1306_UpdateScreen(); //update display on OLED
		    HAL_Delay(500);
		    SSD1306_Clear();
	  }
	  else
	  {
		    SSD1306_GotoXY(0,0);
		    SSD1306_Puts("FAN OFF",&Font_11x18,1);
		    SSD1306_UpdateScreen(); //update display on OLED
		    HAL_Delay(500);
		    SSD1306_Clear();

	  }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
