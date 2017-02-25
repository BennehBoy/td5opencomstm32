/*
Td5Menu.cpp -
 
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

#include <Arduino.h>
#include "td5menu.h"
#include "td5strings.h"

List voidMenu(0, 0, 0, 0, 0, (char**)&_void_menu);
List topMenu(0, 1, 18, 4, 4, (char**)&_main_menu);
List subMenu0(0, 1, 18, 5, 5, (char**)&_sub_menu_0);
List subMenu0_1(0, 1, 18, 3, 3, (char**)&_sub_menu_0_1);
List subMenu0_3(0, 1, 18, 2, 2, (char**)&_sub_menu_0_3);

extern Adafruit_SSD1306 lcd;

Td5Menu::Td5Menu(Button ** b)
{
  btns = b;
}

void Td5Menu::init()
{
  current_choice = 0;
  setCurrentMenu(VOID_MENU);
}

void Td5Menu::setTopMenu()
{
  setCurrentMenu(MAIN_MENU);
  drawMenu();
}

void Td5Menu::setCurrentMenuPtr(List* sub_menu)
{
  current_menu_ptr = sub_menu;
}

void Td5Menu::setCurrentMenu(byte sub_menu)
{
  current_menu = sub_menu;
  switch(current_menu)
  {
    case MAIN_MENU:
      setCurrentMenuPtr(&topMenu); 
      current_header = 0;
      show();  
      break;

    case MAIN_MENU_ECU:
      setCurrentMenuPtr(&subMenu0);    
      current_header = 1;
      show();
      break;    

    case SUB_MENU_ECU_SETTINGS:
      setCurrentMenuPtr(&subMenu0_1);    
      current_header = 3;
      show();
      break; 

    case SUB_MENU_ECU_FAULT_CODES:
      setCurrentMenuPtr(&subMenu0_3);    
      current_header = 2;
      show();
      break; 

    case VOID_MENU:
    case MAIN_MENU_INSTRUMENT:
    case SUB_MENU_ECU_SETTINGS_READ:
    case SUB_MENU_ECU_SETTINGS_SAVE:
    case SUB_MENU_ECU_SETTINGS_RESTORE:    
    case SUB_MENU_ECU_FAULT_CODES_READ:
    case SUB_MENU_ECU_FAULT_CODES_RESET:
    case SUB_MENU_ECU_FUELLING:
    case MAIN_MENU_OPTIONS:
      setCurrentMenuPtr(&voidMenu); 
      current_header = -1;
      hide();
      break;         
  } 
  current_choice = 0;
  current_menu_ptr->update(0);
}

byte Td5Menu::selectMenu()
{
  byte retVal = 0;
  int menuAction = select();

  switch(current_menu)
  {
    /////////////////  Main Menu /////////////////    
    case MAIN_MENU:
      if(menuAction == MENU_SEL_ENTER)
      {
        switch(current_choice)
        {
          case 0:  // Instrument
            retVal = MAIN_MENU_INSTRUMENT; 
            break;

          case 1:  // Ecu
            setCurrentMenu(MAIN_MENU_ECU);
            drawMenu();
            break;
    
          case 2:  // Options
            retVal = MAIN_MENU_OPTIONS;
            break;
    
          case 3:  // Tools
            break;
        }
      }        
      if(menuAction == MENU_SEL_QUIT)
      {
      }        
      break;

    /////////////////  Ecu Menu //////////////////    
    case MAIN_MENU_ECU:
      if(menuAction == MENU_SEL_ENTER)
      {
        switch(current_choice)
        {
          case 0:  // Fuelling
            setCurrentMenu(SUB_MENU_ECU_FUELLING);
            retVal = SUB_MENU_ECU_FUELLING; 
            break;
          
          case 1:  // Settings
            setCurrentMenu(SUB_MENU_ECU_SETTINGS);
            drawMenu();
            break;
    
          case 2:  // Input-output
            break;
    
          case 3:  // Fault codes
            setCurrentMenu(SUB_MENU_ECU_FAULT_CODES);
            drawMenu();
            break;

          case 4:  // Map
            break;
        }
      }        
      if(menuAction == MENU_SEL_QUIT)
      {
        setCurrentMenu(MAIN_MENU);
        drawMenu();
      }        
      break;

    /////////////  Fault Codes Menu //////////////    
    case SUB_MENU_ECU_FAULT_CODES:
      if(menuAction == MENU_SEL_ENTER)
      {
        switch(current_choice)
        {
          case 0:  // Read
            setCurrentMenu(SUB_MENU_ECU_FAULT_CODES_READ);
            retVal = SUB_MENU_ECU_FAULT_CODES_READ; 
            break;
          
          case 1:  // Reset
            setCurrentMenu(SUB_MENU_ECU_FAULT_CODES_RESET);
            retVal = SUB_MENU_ECU_FAULT_CODES_RESET;             
            break;
        }
      }        
      if(menuAction == MENU_SEL_QUIT)
      {
        setCurrentMenu(MAIN_MENU_ECU);
        drawMenu();
      }        
      break;
      
    /////////////  Settings Menu /////////////////    
    case SUB_MENU_ECU_SETTINGS:
      if(menuAction == MENU_SEL_ENTER)
      {
        switch(current_choice)
        {
          case 0:  // Read
            setCurrentMenu(SUB_MENU_ECU_SETTINGS_READ);
            retVal = SUB_MENU_ECU_SETTINGS_READ; 
            break;
          
          case 1:  // Save to file
            setCurrentMenu(SUB_MENU_ECU_SETTINGS_SAVE);
            retVal = SUB_MENU_ECU_SETTINGS_SAVE;             
            break;

          case 2:  // Restore from file
            setCurrentMenu(SUB_MENU_ECU_SETTINGS_RESTORE);
            retVal = SUB_MENU_ECU_SETTINGS_RESTORE;             
            break;            
        }
      }        
      if(menuAction == MENU_SEL_QUIT)
      {
        setCurrentMenu(MAIN_MENU_ECU);
        drawMenu();
      }        
      break;      
  }

  if(menuAction == MENU_SEL_UP)
  {
    retVal =  MENU_UP_BTN;
  } 

  if(menuAction == MENU_SEL_DOWN)
  {
    retVal =  MENU_DOWN_BTN;
  } 

  if(menuAction == MENU_SEL_LEFT)
  {
    retVal =  MENU_LEFT_BTN;
  } 

  if(menuAction == MENU_SEL_RIGHT)
  {
    retVal =  MENU_RIGHT_BTN;
  } 

  if(menuAction == MENU_SEL_POWER)
  {
    retVal = MENU_POWER_OFF;
  } 

  return retVal;
}

void Td5Menu::quitCurrentMenu()
{
  switch(current_menu)
  {
    case VOID_MENU:
    case MAIN_MENU:
      break;

    case MAIN_MENU_ECU:
      setCurrentMenu(MAIN_MENU);
      drawMenu();
      break;

    case SUB_MENU_ECU_FAULT_CODES:
    case SUB_MENU_ECU_FUELLING:
    case SUB_MENU_ECU_SETTINGS:
      setCurrentMenu(MAIN_MENU_ECU);
      drawMenu();
      break;  

    case SUB_MENU_ECU_FAULT_CODES_READ:
    case SUB_MENU_ECU_FAULT_CODES_RESET:
      setCurrentMenu(SUB_MENU_ECU_FAULT_CODES);
      drawMenu();
      break;

    case SUB_MENU_ECU_SETTINGS_READ:
    case SUB_MENU_ECU_SETTINGS_SAVE:
    case SUB_MENU_ECU_SETTINGS_RESTORE:    
      setCurrentMenu(SUB_MENU_ECU_SETTINGS);
      drawMenu();
      break;      
  }
}

int Td5Menu::select()
{
  byte button_pressed = 0;

  current_menu_ptr->show();

  button_pressed=wait_on_escape(50);
  switch (button_pressed)
  {
  case 0:
    break;

  case 1:
    if (current_choice-1>=0) current_choice--;
    current_menu_ptr->update(current_choice);
    return (MENU_SEL_UP);
    break;

  case 2:
    if ((current_choice+1)<current_menu_ptr->getItemCount()) current_choice++;
    current_menu_ptr->update(current_choice);
	return (MENU_SEL_DOWN);
    break;

  case 3: // Left is pressed
    return (MENU_SEL_LEFT);
    break;

  case 4: // Right is pressed
    return (MENU_SEL_RIGHT);
    break;

  case 5: // Enter is pressed
    return(MENU_SEL_ENTER);
    break;

  case 6: // Escape is pressed
    return (MENU_SEL_QUIT);
    break;

  case 7: // Powerup is pressed
	return (MENU_SEL_POWER);
    break;

  default:
    break;
  }
}

void Td5Menu::drawMenu()
{
  #ifdef _DEBUG_
  Serial.println("draw menu");
  #endif
  if(!hidden)
  {
    lcd.clearDisplay();
    lcd.setCursor(0,0);
	lcd.print(menu_header[current_header]);
    #ifdef _DEBUG_
	Serial.println(menu_header[current_header]);
    #endif
    current_menu_ptr->show();
	lcd.display();
  }
}

void Td5Menu::hide()
{
  hidden = true;
  lcd.clearDisplay();
  lcd.display();
}

void Td5Menu::show()
{
  hidden = false;
}

//Interactions
byte Td5Menu::wait_on_escape(int ref_time) { 
  // Returns key pressed or 0 if time expires before any key was pressed.
  //Wait on button push.
  long temp0;
  byte temp1;
  temp0=millis();
  do
  {
    for (byte i=0;i<7;i++)
    {
      temp1=btns[i]->sense();

      if((temp1==buttons_released)||(temp1==buttons_held))
      {
        return(i+1);
      }
    }
  }   
  while ((millis()-temp0<ref_time));

  return (0);
}


