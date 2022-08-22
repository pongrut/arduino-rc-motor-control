/*
    2013 Copyright (c) Seeed Technology Inc.  All right reserved.

    Author:Loovee
    2013-9-18

    Grove - Serial LCD RGB Backlight demo.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    DISPLAY_RGB_ADDR = 0x62
    DISPLAY_TEXT_ADDR = 0x3e
*/

#include <Wire.h>
#include "rgb_lcd.h"

// uncomment #define ENABLE_ENCODER to compile this for using Encoders
#define ENABLE_LCD

#define LCD_MAX_ROW     2   // It's 2 row lcd
#define LCD_MAX_COLUMN  16  // with 16 maximum characters

#ifdef ENABLE_LCD

  // Define LCD display connections which is used for motors & rc controller
  rgb_lcd lcd;

  const int colorR = 0;
  const int colorG = 255;
  const int colorB = 0;
   
  void display_setup(int driveMode){
    lcd.begin (LCD_MAX_COLUMN, LCD_MAX_ROW);                  // start the library & set up the LCD's number of columns and rows
    lcd.setRGB(colorR, colorG, colorB);
    lcd.clear();  
    lcd.setCursor (0, 0);
    lcd.print ("CTRL's started"); // print a simple message
    lcd.setCursor (0,1);
    lcd.print ("Drive Mode: "); // print a simple message
    if(driveMode){
      lcd.print ("Auto"); // print a simple message
    }else{
      lcd.print ("R/C"); // print a simple message
    }
    delay(1000);
    lcd.clear();
  }
  void display_clear(){
    lcd.clear(); 
  }
  
  void display_show(int col, int row, char* txt){
    if(row <= LCD_MAX_ROW && col <= LCD_MAX_COLUMN){
      lcd.setCursor (col, row);
    }else{
      lcd.setCursor (0, 0);
    }
    lcd.print(txt);
  }

  void display_show_int(int col, int row,  int number){
    //char txt[max_length];
   // dtostrf(number, max_length, 0, txt);
    if(row <= LCD_MAX_ROW && col <= LCD_MAX_COLUMN){
      lcd.setCursor (col, row);
    }else{
      lcd.setCursor (0, 0);
    }
    lcd.print(number);
  }
#endif
/*
char str_temp[6];


dtostrf(temp, 4, 2, str_temp);
sprintf(temperature,"%s F", str_temp);
  */
