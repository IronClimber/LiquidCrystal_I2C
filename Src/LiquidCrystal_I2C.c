#include "stm32f4xx.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_i2c.h"
#include "LiquidCrystal_I2C.h"
#include <inttypes.h>
#include "i2c.h"

//TX buffer
static uint8_t buffer[500];
static uint8_t buffer_counter;

LiquidCrystal_I2C_TypeDef lc_t;

/*
 * 4-bit mode
 * no backlight
 * display on, no cursor, blinking cursor off
 *
 */
void LiquidCrystal_I2C_Init(I2C_HandleTypeDef* lcd_hi2c, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t lcd_charsize)
{
	lc_t.hi2c = lcd_hi2c;
	lc_t.addr = lcd_addr;
	lc_t.cols = lcd_cols;
	lc_t.rows = lcd_rows;
	lc_t.charsize = lcd_charsize;
	lc_t.backlightval = LCD_NOBACKLIGHT;

	lc_t.displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (lc_t.rows > 1) {
		lc_t.displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((lc_t.charsize != 0) && (lc_t.rows == 1)) {
		lc_t.displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands.
	HAL_Delay(50);

	// Now we pull both RS and R/W low to begin commands
	LiquidCrystal_I2C_WriteExpander(0x00);	// reset I2C_expander

	// put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	LiquidCrystal_I2C_WriteNibble(0x03 << 4);
	DelayMicroseconds(4500); // wait min 4.1ms

	// second try
	LiquidCrystal_I2C_WriteNibble(0x03 << 4);
	DelayMicroseconds(150);

	// third go!
	LiquidCrystal_I2C_WriteNibble(0x03 << 4);

	// finally, set to 4-bit interface
	LiquidCrystal_I2C_WriteNibble(0x02 << 4);

	// set # lines, font size, etc.
	LiquidCrystal_I2C_WriteCommand(LCD_FUNCTIONSET | lc_t.displayfunction);
	
	// turn the display on with no cursor or blinking default
	lc_t.displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LiquidCrystal_I2C_Display();
	
	// clear it off
	LiquidCrystal_I2C_Clear();
	
	// Initialize to default text direction (for roman languages)
	lc_t.displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	LiquidCrystal_I2C_WriteCommand(LCD_ENTRYMODESET | lc_t.displaymode);
	
	LiquidCrystal_I2C_Home();
}

/********** high level commands, for the user! */
void LiquidCrystal_I2C_Clear(){
	LiquidCrystal_I2C_WriteCommand(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	HAL_Delay(3);  // this command takes a long time!
}

void LiquidCrystal_I2C_Home(){
	LiquidCrystal_I2C_WriteCommand(LCD_RETURNHOME);  // set cursor position to zero
	HAL_Delay(3);  // this command takes a long time!
}

void LiquidCrystal_I2C_SetCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > lc_t.rows) {
		row = lc_t.rows-1;    // we count rows starting w/0
	}
	LiquidCrystal_I2C_WriteCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C_NoDisplay() {
	lc_t.displaycontrol &= ~LCD_DISPLAYON;
	LiquidCrystal_I2C_WriteCommand(LCD_DISPLAYCONTROL | lc_t.displaycontrol);
}
void LiquidCrystal_I2C_Display() {
	lc_t.displaycontrol |= LCD_DISPLAYON;
	LiquidCrystal_I2C_WriteCommand(LCD_DISPLAYCONTROL | lc_t.displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C_NoCursor() {
	lc_t.displaycontrol &= ~LCD_CURSORON;
	LiquidCrystal_I2C_WriteCommand(LCD_DISPLAYCONTROL | lc_t.displaycontrol);
}
void LiquidCrystal_I2C_Cursor() {
	lc_t.displaycontrol |= LCD_CURSORON;
	LiquidCrystal_I2C_WriteCommand(LCD_DISPLAYCONTROL | lc_t.displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C_NoBlink() {
	lc_t.displaycontrol &= ~LCD_BLINKON;
	LiquidCrystal_I2C_WriteCommand(LCD_DISPLAYCONTROL | lc_t.displaycontrol);
}
void LiquidCrystal_I2C_Blink() {
	lc_t.displaycontrol |= LCD_BLINKON;
	LiquidCrystal_I2C_WriteCommand(LCD_DISPLAYCONTROL | lc_t.displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C_ScrollDisplayLeft(void) {
	LiquidCrystal_I2C_WriteCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2C_ScrollDisplayRight(void) {
	LiquidCrystal_I2C_WriteCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C_LeftToRight(void) {
	lc_t.displaymode |= LCD_ENTRYLEFT;
	LiquidCrystal_I2C_WriteCommand(LCD_ENTRYMODESET | lc_t.displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C_RightToLeft(void) {
	lc_t.displaymode &= ~LCD_ENTRYLEFT;
	LiquidCrystal_I2C_WriteCommand(LCD_ENTRYMODESET | lc_t.displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C_Autoscroll(void) {
	lc_t.displaymode |= LCD_ENTRYSHIFTINCREMENT;
	LiquidCrystal_I2C_WriteCommand(LCD_ENTRYMODESET | lc_t.displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C_NoAutoscroll(void) {
	lc_t.displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	LiquidCrystal_I2C_WriteCommand(LCD_ENTRYMODESET | lc_t.displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
// !!!!Note: Set cursor after use
void LiquidCrystal_I2C_CreateChar(uint8_t location, uint8_t* charmap) {
	location &= 0x7; // we only have 8 locations 0-7
	LiquidCrystal_I2C_WriteCommand(LCD_SETCGRAMADDR | (location << 3));
	uint8_t symbol_rows = lc_t.charsize ? 10 : 8;
	LiquidCrystal_I2C_AddBytes(charmap, symbol_rows);
	LiquidCrystal_I2C_Write();
}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C_NoBacklight(void) {
	lc_t.backlightval=LCD_NOBACKLIGHT;
	LiquidCrystal_I2C_WriteExpander(lc_t.backlightval);
}

void LiquidCrystal_I2C_Backlight(void) {
	lc_t.backlightval=LCD_BACKLIGHT;
	LiquidCrystal_I2C_WriteExpander(lc_t.backlightval);
}

void LiquidCrystal_I2C_SetBacklight(uint8_t new_val){
	if (new_val) {
		LiquidCrystal_I2C_Backlight();		// turn backlight on
	} else {
		LiquidCrystal_I2C_NoBacklight();	// turn backlight off
	}
}

/*********** mid level commands, for sending data/cmds */

void LiquidCrystal_I2C_WriteCommand(uint8_t cmd) {
	LiquidCrystal_I2C_AddByte(cmd, 0);
	LiquidCrystal_I2C_Write();
	DelayMicroseconds(40);
}
void LiquidCrystal_I2C_WriteSymbol(uint8_t symb) {
	LiquidCrystal_I2C_AddByte(symb, Rs);
	LiquidCrystal_I2C_Write();
}

void LiquidCrystal_I2C_PrintString(char *str){
	uint8_t i=0;
	while(str[i]) {
		LiquidCrystal_I2C_AddByte(str[i], Rs);
	    i++;
	}
	LiquidCrystal_I2C_Write();
}

/************ low level data pushing commands **********/
/*
// write either command or data */

/* Buffer manipulating */
void LiquidCrystal_I2C_AddValue(uint8_t value) {
	buffer[buffer_counter] = value;
	//LCD_Printf("(%3d: %x)         ", buffer_counter, buffer[buffer_counter]);
	buffer_counter++;
}

void LiquidCrystal_I2C_AddNibble(uint8_t nibble) {
	LiquidCrystal_I2C_AddValue(nibble);
	LiquidCrystal_I2C_AddValue(nibble |En);
	LiquidCrystal_I2C_AddValue(nibble);
}
void LiquidCrystal_I2C_AddByte(uint8_t byte, uint8_t mode) {
	uint8_t high_nibble = byte & 0xF0;
	uint8_t low_nibble = (byte << 4) & 0xF0;
	LiquidCrystal_I2C_AddNibble(high_nibble | lc_t.backlightval | mode);
	LiquidCrystal_I2C_AddNibble(low_nibble | lc_t.backlightval | mode);
}
void LiquidCrystal_I2C_AddBytes(uint8_t* data, uint16_t size) {
	uint8_t i;
	for (i=0; i<size; i++) {
		LiquidCrystal_I2C_AddByte(data[i], Rs);
	}
}

//Data writing
void LiquidCrystal_I2C_WriteExpander(uint8_t data){
	LiquidCrystal_I2C_AddValue(data);
	LiquidCrystal_I2C_Write();
}

void LiquidCrystal_I2C_Write(void) {
	HAL_I2C_Master_Transmit(lc_t.hi2c, lc_t.addr << 1, buffer, buffer_counter, 100);
	buffer_counter = 0;
	DelayMicroseconds(40);
}

//---------------------------------------------------------

//DDRAM.  First row addresses 0x80 - 0xA8
//       Second row addresses 0xC0 - 0xE8
void LiquidCrystal_I2C_WriteSymbolCustomDDRAM(uint8_t ddram_addr, uint8_t ch) {
	LiquidCrystal_I2C_WriteCommand(LCD_SETDDRAMADDR | ddram_addr);
	LiquidCrystal_I2C_WriteSymbol(ch);
}


void LiquidCrystal_I2C_WriteNibble(uint8_t nibble) {
	LiquidCrystal_I2C_AddNibble(nibble | lc_t.backlightval);
	LiquidCrystal_I2C_Write();
}

void DelayMicroseconds(uint32_t mc)
{
        volatile uint32_t nCount;
        nCount=(HAL_RCC_GetHCLKFreq()/10000000/1.1)*mc;
        for (; nCount!=0; nCount--);
}
