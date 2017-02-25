/*
Td5menu.h -
 
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

#ifndef Td5Menu_h
#define Td5Menu_h

#include <Buttons.h>
#include <Adafruit_SSD1306.h>
#include "td5defs.h"
#include "td5hmi.h"

#define lcd_columns   20
#define lcd_rows       4

extern List topMenu;
extern List subMenu0;

class Td5Menu
{
public:
  Td5Menu(Button ** b);
  void init();
  byte selectMenu();
  void setCurrentMenu(byte sub_menu);
  void setTopMenu();
  void quitCurrentMenu();
  void show();
  void hide();  
private:
  void drawMenu();
  byte wait_on_escape(int);  
  int select();
  void setCurrentMenuPtr(List* sub_menu);
  
private:
  Button ** btns;
  byte current_menu;
  List* current_menu_ptr;
  byte current_choice;
  byte current_header;
  boolean hidden;
};

#define VOID_MENU                         0
#define MAIN_MENU                         1
#define MAIN_MENU_INSTRUMENT             10
#define MAIN_MENU_ECU                    20
#define SUB_MENU_ECU_FUELLING            21
#define SUB_MENU_ECU_SETTINGS            22
#define SUB_MENU_ECU_IN_OUT              23
#define SUB_MENU_ECU_FAULT_CODES         24
#define SUB_MENU_ECU_FAULT_CODES_READ    25
#define SUB_MENU_ECU_FAULT_CODES_RESET   26
#define SUB_MENU_ECU_MAP                 27
#define SUB_MENU_ECU_FUELLING            28
#define SUB_MENU_ECU_SETTINGS            29
#define SUB_MENU_ECU_SETTINGS_READ       30
#define SUB_MENU_ECU_SETTINGS_SAVE       31
#define SUB_MENU_ECU_SETTINGS_RESTORE    32
#define MAIN_MENU_OPTIONS                70
#define MAIN_MENU_TOOLS                  80

#define MENU_UP_BTN                      90
#define MENU_DOWN_BTN                    91
#define MENU_LEFT_BTN                    92
#define MENU_RIGHT_BTN                   93
#define MENU_ENTER_BTN                   94
#define MENU_QUIT_BTN                    95
#define MENU_POWER_OFF                   99

#define MENU_SEL_UP                      -5
#define MENU_SEL_DOWN                    -6
#define MENU_SEL_LEFT                    -3
#define MENU_SEL_RIGHT                   -4
#define MENU_SEL_ENTER                    1
#define MENU_SEL_QUIT                    -1
#define MENU_SEL_POWER                   -7

#endif

