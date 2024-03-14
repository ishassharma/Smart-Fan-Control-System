/**
 * Reference and token of thanks to code written by Alexander Lutsai, Tilen Majerle
 * https://controllerstech.com/oled-display-using-i2c-stm32/
 * ******************************************************************************
	* PROJECT: SMART FAN CONTROL SYSTEM
	* AUTHOR:  ISHA SHARMA
	* COURSE:  EMBEDDED SYSTEMS DESIGN SP'23
	* TOOLS:   STMCUBEIDE
	* DATE:    06-05-2023
	* FILE:    ssd1306.h
	* BRIEF:   This is header file which is used for functions related to the SSD1306 OLED
	*
	* ******************************************************************************
 */
#ifndef SSD1306_H
#define SSD1306_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

#include "stm32f4xx_hal.h"
#include "fonts.h"
#include "stdlib.h"
#include "string.h"

/* I2C address */
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR         0x78 //slave address
#endif

/* SSD1306 settings */
/* SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            128
#endif

/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           64
#endif

/**
 * @brief  SSD1306 color enum
 */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /* no pixel, blank */
	SSD1306_COLOR_WHITE = 0x01  /* pixel is set to color of oled */
} SSD1306_COLOR_t;

uint8_t SSD1306_Init(void);
void SSD1306_UpdateScreen(void);
void SSD1306_Fill(SSD1306_COLOR_t Color);
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);
void SSD1306_GotoXY(uint16_t x, uint16_t y);
char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);
char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color);

#ifndef ssd1306_I2C_TIMEOUT
#define ssd1306_I2C_TIMEOUT					20000 //i2c timeout of 2000
#endif

void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);
void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);
void SSD1306_ScrollRight(uint8_t start_row, uint8_t end_row);
void SSD1306_ScrollLeft(uint8_t start_row, uint8_t end_row);
void SSD1306_Stopscroll(void);
void SSD1306_Clear (void);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
