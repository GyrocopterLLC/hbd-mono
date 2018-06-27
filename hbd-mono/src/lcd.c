/*
 * lcd.c
 *
 *  Created on: Mar 6, 2018
 *      Author: David
 */
#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx.h"
#include "gpio.h"
#include "gfxfont.h"
#include "lcd.h"
#include <string.h>

#define LCD_SLEWRATE_DELAY_US 5 // Delay for the slowish open collector pins to change

uint16_t lcd_framebuffer[LCD_HEIGHT*LCD_WIDTH_WORDS];

static void delay_us(int us)
{
  // clock speed = 48MHz
  volatile int count = 48 * us; // this probably works enough
  while(count--);
}

void lcd_init(void)
{

  GPIO_Clk(GPIOB);
  // Port B, Pins 8->15 are data
  // Port B, Pin 4 is rs
  // Port B, Pin 5 is e

  GPIO_Output(GPIOB,4);
  GPIO_Output(GPIOB,5);
  GPIO_Output(GPIOB,8);
  GPIO_Output(GPIOB,9);
  GPIO_Output(GPIOB,10);
  GPIO_Output(GPIOB,11);
  GPIO_Output(GPIOB,12);
  GPIO_Output(GPIOB,13);
  GPIO_Output(GPIOB,14);
  GPIO_Output(GPIOB,15);

  GPIOB->BRR |= (RS_PIN | E_PIN); // pins 4 & 5
  GPIOB->BRR |= DATAPINS; // pins 8->15


  lcd_cmd(FUNCTION_SET | DATA_LENGTH_DL); // 8 bits interface, RE=0
  delay_us(10000);
    lcd_cmd(FUNCTION_SET | DATA_LENGTH_DL); // again
    delay_us(10000);
    lcd_cmd(DISPLAY_CONTROL | DISPLAY_ON_D ); // display on
    delay_us(10000);
    lcd_cmd(DISPLAY_CLEAR); // clear display
    delay_us(10000);
    lcd_cmd(ENTRY_MODE_SET | INCREASE_DECREASE_ID); // move cursor right
    delay_us(10000);
    lcd_cmd(RETURN_HOME);
//    And now change to extended mode (ie pixel control, not character)

    lcd_cmd(EXTENDED_FUNCTION_SET | DATA_LENGTH_DL);
    delay_us(10000);
    lcd_cmd(EXTENDED_FUNCTION_SET | DATA_LENGTH_DL | EXTENDED_INSTRUCTION_RE); //RE=1 (Extended funtion set)
    delay_us(10000);
    lcd_cmd(EXTENDED_FUNCTION_SET | DATA_LENGTH_DL | EXTENDED_INSTRUCTION_RE | GRAPHIC_ON_G);
    delay_us(10000);
    lcd_cmd(SCROLL_OR_RAM_ADDR_SEL);
}

void lcd_cmd(uint8_t cmd)
{
  GPIOB->BRR |= DATAPINS;
  GPIOB->BSRR |= (((uint32_t)cmd) << 8) & DATAPINS;
  GPIOB->BRR |= RS_PIN;
  GPIOB->BSRR |= E_PIN;
  delay_us(LCD_SLEWRATE_DELAY_US);
//  HAL_Delay(3);
  GPIOB->BRR |= E_PIN;
  delay_us(LCD_SLEWRATE_DELAY_US);
}


void lcd_data(uint8_t cmd)
{
  GPIOB->BRR |= DATAPINS;
  GPIOB->BSRR |= (((uint32_t)cmd) << 8) & DATAPINS;
  GPIOB->BSRR |= (RS_PIN | E_PIN);
  delay_us(LCD_SLEWRATE_DELAY_US);
//  HAL_Delay(3);
  GPIOB->BRR |= E_PIN;
  delay_us(LCD_SLEWRATE_DELAY_US);
}

void lcd_writebyte(uint16_t x, uint16_t y, uint16_t dat)
{
  lcd_cmd(SET_GRAPHIC_RAM_ADDRESS | y);
  lcd_cmd(SET_GRAPHIC_RAM_ADDRESS | x);

  lcd_data((dat&0xFF00) >> 8);
  lcd_data(dat&0x00FF);
}

void lcd_cleardisp(void)
{
  memset(lcd_framebuffer,0,2*LCD_WIDTH_WORDS*LCD_HEIGHT);
}

void lcd_filldisp(void)
{
  memset(lcd_framebuffer,0xff,2*LCD_WIDTH_WORDS*LCD_HEIGHT);
}

void lcd_show(void)
{

  for(int y = 0; y< LCD_HEIGHT;y++)
  {
    lcd_writebyte(0,y,lcd_framebuffer[y*LCD_WIDTH_WORDS]);
    for(int x = 1; x < LCD_WIDTH_WORDS; x++)
    {
      lcd_data(((lcd_framebuffer[y*LCD_WIDTH_WORDS + x])&0xFF00) >> 8);
      lcd_data((lcd_framebuffer[y*LCD_WIDTH_WORDS + x])&0x00FF);
    }
  }
}

void lcd_pix(uint16_t x, uint16_t y, uint8_t onoff)
{
  if((x < LCD_WIDTH) && (y < LCD_HEIGHT))
  {
    // Which half-word?
    uint8_t x_word = x / 16;
    // Which bit?
    uint8_t bitpos = x % 16;
    if(onoff)
      lcd_framebuffer[y*LCD_WIDTH_WORDS + x_word] |= (0x0001 << (15-bitpos));
    else
      lcd_framebuffer[y*LCD_WIDTH_WORDS + x_word] &= ~(0x0001 << (15-bitpos));
  }
}

void lcd_drawCircle(uint16_t x, uint16_t y, uint16_t radius)
{
  // Bresenham variant, thanks TI/LuminaryMicro!
  int32_t a, b, d;
  a = 0;
  b = radius;
  d = 3-(2*radius);

  while(a<=b)
  {
    lcd_pix(x-b,y-a,1);
    lcd_pix(x+b,y-a,1);
    if(a != 0)
    {
      lcd_pix(x-b,y+a,1);
      lcd_pix(x+b,y+a,1);
    }
    if(a!=b)
    {
      lcd_pix(x-a,y-b,1);
      if(a!=0)
      {
        lcd_pix(x+a,y-b,1);
      }
      lcd_pix(x-a,y+b,1);
      if(a!=0)
      {
        lcd_pix(x+a,y+b,1);
      }
    }
    if(d<0)
    {
      d+= 4*a+6;
    }
    else
    {
      d+= 4*(a-b)+10;
      b-=1;
    }
    a++;
  }
}

void lcd_drawChar(uint16_t x, uint16_t y, unsigned char c, GFXfont* gfxFont)
{
  c -= gfxFont->first;
  GFXglyph *glyph  = &(gfxFont->glyph[c]);
  uint8_t  *bitmap = gfxFont->bitmap;

  uint16_t bo = glyph->bitmapOffset;
  uint8_t  w  = glyph->width,
       h  = glyph->height;
  int8_t   xo = glyph->xOffset,
       yo = glyph->yOffset;
  uint8_t  xx, yy, bits = 0, bit = 0;
  int16_t  xo16 = 0, yo16 = 0;

  for(yy=0; yy<h; yy++)
  {
    for(xx=0; xx<w; xx++)
    {
      if(!(bit++ & 7))
      {
        bits = bitmap[bo++];
      }
      if(bits & 0x80)
      {
        lcd_pix(x+xo+xx, y+yo+yy, 1);
      }
      else
      {
        lcd_pix(x+xo+xx, y+yo+yy, 0);
      }
      bits <<= 1;
    }
  }
}

void lcd_write(uint16_t x, uint16_t y, char* string, GFXfont* gfxFont)
{
  GFXglyph* charGlyph;
  char c;
  while(*string)
  {
    lcd_drawChar(x,y,*string,gfxFont);
    c = *string - gfxFont->first;
    charGlyph = &(gfxFont->glyph[c]);
    x+= charGlyph->width+1;
    string++;
  }
}

/*
void lcd_drawFilledCircle(uint16_t x, uint16_t y, uint16_t radius)
{
  // Again, thanks to TI and LuminaryMicro for some example code
  int32_t a, b, d;

  a=0;
  b=radius;
  d = 3-(2*radius);
  while(a<=b)
  {
    gfx_drawHLine(x-b,y-a,2*b+1,1);
    if(a!=0)
    {
      gfx_drawHLine(x-b,y+a,2*b+1,1);
    }
    if((d>=0) && (a!=b))
    {
      gfx_drawHLine(x-a,y-b,2*a+1,1);
      gfx_drawHLine(x-a,y+b,2*a+1,1);
    }
    if(d<0)
    {
      d+=4*a+6;
    }
    else
    {
      d+=4*(a-b)+10;
      b-=1;
    }
    a++;
  }
}
*/
