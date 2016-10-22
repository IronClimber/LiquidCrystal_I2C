#ifndef FDB_LIQUID_CRYSTAL_I2C_H
#define FDB_LIQUID_CRYSTAL_I2C_H

#include "stm32f4xx.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_i2c.h"

#include <inttypes.h>

#define BUFFER_SIZE 500

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit


typedef struct {
	I2C_HandleTypeDef* hi2c;
	uint8_t addr;
	uint8_t cols;
	uint8_t rows;
	uint8_t charsize;
	uint8_t backlightval;
	uint8_t displayfunction;
	uint8_t displaycontrol;
	uint8_t displaymode;
} LiquidCrystal_I2C_TypeDef;

/**
 * This is the driver for the Liquid Crystal LCD displays that use the I2C bus.
 */

	/**
	 * @param lcd_hi2c  I2C port
	 * @param lcd_addr	I2C slave address of the LCD display. Most likely printed on the
	 *					LCD circuit board, or look in the supplied LCD documentation.
	 * @param lcd_cols	Number of columns your LCD display has.
	 * @param lcd_rows	Number of rows your LCD display has.
	 * @param charsize	The size in dots that the display has, use LCD_5x10DOTS or LCD_5x8DOTS.
	 */
	void LiquidCrystal_I2C_Init(I2C_HandleTypeDef* lcd_hi2c, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t lcd_charsize);
	
	 /**
	  * Remove all the characters currently shown. Next print/write operation will start
	  * from the first position on LCD display.
	  */
	void LiquidCrystal_I2C_Clear();
	  
	/**
	 * Next print/write operation will start from the first position on the LCD display.
	 */
	void LiquidCrystal_I2C_Home();

	 /**
	  * Do not show any characters on the LCD display. Backlight state will remain unchanged.
	  * Also all characters written on the display will return, when the display in enabled again.
	  */
	void LiquidCrystal_I2C_NoDisplay();
	  
	/**
	 * Show the characters on the LCD display, this is the normal behavior. This method should
	 * only be used after NoDisplay() has been used.
	 */ 
	void LiquidCrystal_I2C_Display();

	/**
	 * Do not blink the cursor indicator.
	 */
	void LiquidCrystal_I2C_NoBlink();
	 
	/**
	 * Start blinking the cursor indicator.
	 */ 
	void LiquidCrystal_I2C_Blink();

	/**
	 * Do not show a cursor indicator.
	 */
	void LiquidCrystal_I2C_NoCursor();

	/**
 	 * Show a cursor indicator, cursor can blink on not blink. Use the
	 * functions Blink() and NoBlink() for changing cursor blink.
	 */ 
	void LiquidCrystal_I2C_Cursor();

	void LiquidCrystal_I2C_SetCursor(uint8_t, uint8_t);

	void LiquidCrystal_I2C_ScrollDisplayLeft();
	void LiquidCrystal_I2C_ScrollDisplayRight();

	//Type direction
	void LiquidCrystal_I2C_LeftToRight();
	void LiquidCrystal_I2C_RightToLeft();

	//Backlight functions
	void LiquidCrystal_I2C_NoBacklight();
	void LiquidCrystal_I2C_Backlight();
	void LiquidCrystal_I2C_SetBacklight(uint8_t new_val);

	void LiquidCrystal_I2C_Autoscroll();
	void LiquidCrystal_I2C_NoAutoscroll();

	void LiquidCrystal_I2C_CreateChar(uint8_t, uint8_t[]);

	void LiquidCrystal_I2C_AddValue(uint8_t value);
	void LiquidCrystal_I2C_AddNibble(uint8_t nibble);
	void LiquidCrystal_I2C_AddByte(uint8_t byte, uint8_t mode);
	void LiquidCrystal_I2C_AddBytes(uint8_t* data, uint16_t size);

	void LiquidCrystal_I2C_WriteNibble(uint8_t nibble);

	void LiquidCrystal_I2C_WriteCommand(uint8_t cmd);
	void LiquidCrystal_I2C_WriteSymbol(uint8_t symb);

	void LiquidCrystal_I2C_PrintString(char *str);

	void LiquidCrystal_I2C_WriteSymbolCustomDDRAM(uint8_t ddram_addr, uint8_t ch);

	void LiquidCrystal_I2C_WriteExpander(uint8_t data);

	void LiquidCrystal_I2C_Write(void);

	void DelayMicroseconds(uint32_t mc);


#endif // FDB_LIQUID_CRYSTAL_I2C_H
