/*
 * lcd.h
 *
 *  Created on: Mar 6, 2018
 *      Author: David
 */

#ifndef LCD_H_
#define LCD_H_

#define LCD_HEIGHT        32
#define LCD_WIDTH       144 // Pixels
#define LCD_WIDTH_WORDS     9   // Display is written 16 bits at a time

#define DISPLAY_CLEAR           0x01 // Fill DDRAM with "20H" and set DDRAM address counter (AC) to "00H"
#define RETURN_HOME             0x02 // Set DDRAM address counter (AC) to "00H", and put cursor
// to origin &#65533;Gthe content of DDRAM are not changed
#define ENTRY_MODE_SET          0x04 // Set cursor position and display shift when doing write or read
// operation
#define DISPLAY_CONTROL         0x08 // D=1: Display ON, C=1: Cursor ON, B=1: Character Blink ON
#define CURSOR_DISPLAY_CONTROL  0x10 // Cursor position and display shift control; the content of
// DDRAM are not changed
#define FUNCTION_SET            0x20 // DL=1 8-bit interface, DL=0 4-bit interface
// RE=1: extended instruction, RE=0: basic instruction
#define SET_CGRAM_ADDRESS       0x40 // Set CGRAM address to address counter (AC)
// Make sure that in extended instruction SR=0
#define SET_DDRAM_ADDRESS       0x80 // Set DDRAM address to address counter (AC)
// AC6 is fixed to 0

// Instruction set 2: (RE=1: extended instruction)
#define STANDBY                 0x01 // Enter standby mode, any other instruction can terminate.
// COM1&#65533;c32 are halted
#define SCROLL_OR_RAM_ADDR_SEL  0x02 // SR=1: enable vertical scroll position
// SR=0: enable CGRAM address (basic instruction)
#define REVERSE_BY_LINE         0x04 // Select 1 out of 4 line (in DDRAM) and decide whether to
// reverse the display by toggling this instruction
// R1,R0 initial value is 0,0
#define EXTENDED_FUNCTION_SET   0x20 // DL=1 :8-bit interface, DL=0 :4-bit interface
// RE=1: extended instruction, RE=0: basic instruction
#define SET_SCROLL_ADDRESS      0x40 // G=1 :graphic display ON, G=0 :graphic display OFF
#define SET_GRAPHIC_RAM_ADDRESS 0x80 // Set GDRAM address to address counter (AC)
// Set the vertical address first and followed the horizontal
// address by consecutive writings
// Vertical address range: AC5&#65533;cAC0, Horizontal address range: AC3&#65533;cAC0

// Parameters regarding Instruction Sets 1 & 2
#define DISPLAY_SHIFT_S         0x01 // Set 1, ENTRY_MODE_SET
#define INCREASE_DECREASE_ID    0x02 // Set 1, ENTRY_MODE_SET
#define CURSOR_BLINK_ON_B       0x01 // Set 1, DISPLAY_CONTROL
#define CURSOR_ON_C             0x02 // Set 1, DISPLAY_CONTROL
#define DISPLAY_ON_D            0x04 // Set 1, DISPLAY_CONTROL
#define SHIFT_RL                0x04 // Set 1, CURSOR_DISPLAY_CONTROL
#define CURSOR_SC               0x08 // Set 1, CURSOR_DISPLAY_CONTROL
#define EXTENDED_INSTRUCTION_RE 0x04 // Set 1, FUNCTION_SET; Set 2, EXTENDED_FUNTION_SET
#define DATA_LENGTH_DL          0x10 // Set 1, FUNCTION_SET; Set 2, EXTENDED_FUNTION_SET
#define REVERSE_BY_LINE_R0      0x01 // Set 2, REVERSE_BY_LINE
#define REVERSE_BY_LINE_R1      0x02 // Set 2, REVERSE_BY_LINE
#define EN_VERTICAL_SCROLL_SR   0x01 // Set 2, SCROLL_OR_RAM_ADDR_SEL
#define GRAPHIC_ON_G            0x02 // Set 2, EXTENDED_FUNTION_SET

#define BUSY_FLAG_BF            0x80

#define DATAPINS  (0xFF00) // Pins 8->15
#define RS_PIN  (0x0010) // Pin 4
#define E_PIN      (0x0020) // Pin 5

void lcd_init(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t cmd);
void lcd_writebyte(uint16_t x, uint16_t y, uint16_t dat);
void lcd_cleardisp(void);
void lcd_show(void);
void lcd_pix(uint16_t x, uint16_t y, uint8_t onoff);
void lcd_drawCircle(uint16_t x, uint16_t y, uint16_t radius);
void lcd_drawChar(uint16_t x, uint16_t y, unsigned char c, GFXfont* gfxFont);
void lcd_write(uint16_t x, uint16_t y, char* string, GFXfont* gfxFont);

#endif /* LCD_H_ */
