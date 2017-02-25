/*
Td5Hmi.h -

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

*/

#ifndef Td5Hmi_h
#define Td5Hmi_h

#include <Adafruit_SSD1306.h>
#include <Buttons.h>
#include "td5defs.h"

// Class Hmi
class Hmi
{
public:
  Hmi(byte c, byte r, byte l);
  virtual void show();
  virtual void update(byte progr);

protected:
  byte col;
  byte row;
  byte length;
};

// Class LcdBacklight
class LcdBacklight
{
public:
  LcdBacklight(byte pinBkLight);
  void incBrightness(byte inc);
  void decBrightness(byte dec);
  void setBrightness(byte br);
  void update();
  void on();
  void off();
  
protected:
  byte pin;
  byte brightness;
};


// Class ProgBar
class ProgBar: public Hmi
{
public:
  ProgBar(byte c, byte r, byte l, byte bmin , byte bmax);
  void show();
  void hide();
  void update(byte progr);

private:
  float ratio;
  byte advance;
};

// Class ScrollBarV
class ScrollBarV: public Hmi
{
public:
  ScrollBarV(byte c, byte r, byte h);
  void update(byte progr);

private:
  byte v_height;
};

// Class List
class List: public Hmi
{
public:
  List(byte c, byte r, byte l, byte items, byte item_scr, char** list);
  List();
  void init(byte c, byte r, byte l, byte items, byte item_scr, char** list);
  void show();
  void update(byte progr);
  byte getItemCount(){return item_rows;};
  
private:
  void render(byte progr);

private:
  byte item_rows;             // Number of rows to display of the list
  byte item_per_screen;       // Number of list items per screen
  byte item_selected;         // Selected item
  char** item_list;           // Pointer to the PROGMEM list
};


// Class ParamList
class ParamList: public Hmi
{
public:
  ParamList(byte c, byte r, byte l, byte items, byte item_scr, char** list, byte l_list, char** param, byte l_param, Button ** b);
  ParamList(Button ** b);
  void init(byte c, byte r, byte l, byte items, byte item_scr, char** list, byte l_list, char** param, byte l_param);
  void show();
  void update(byte progr);
  byte getItemCount(){return item_rows;};
  
private:
  void render(byte progr);

private:
  byte item_rows;             // Number of rows to display of the list
  byte item_per_screen;       // Number of list items per screen
  byte item_selected;         // Selected item
  byte length_list;           // list field lenght 
  byte length_param;          // param field lenght 
  char** item_list;           // Pointer to the PROGMEM list
  char** item_param;          // Pointer to the param list
};


// Functions
void print_welcome_screen();
void print_connect_screen(char* str);
void print_instrument_screen(int rpm = 0, float coolant_t = 0.0, float turbo_p = 0.0, float battery = 0.0, float ambient_p = 1.0, float maf = 0.0, float inlet_t = 0.0, float fuel_t = 0.0);
void print_read_fault_codes_screen(int index, int count, char* description);
void print_reset_fault_codes_screen(boolean yes_no);
void print_fuelling_screen(byte currentParam, boolean logActive);
void print_options_screen(byte currentParam);
void update_fuelling_screen(byte currentParam);

void print_pid(byte col, byte row, char* tag, float fvalue, byte precision);
void setBrightness(byte bright);

extern Button btn_up;
extern Button btn_down;
extern Button btn_left;
extern Button btn_right;
extern Button btn_enter;
extern Button btn_quit;
extern Button btn_powerup;
extern Button *btns[];

#endif
