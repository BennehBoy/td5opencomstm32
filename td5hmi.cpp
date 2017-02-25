/*
Td5Hmi.cpp -
 
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

#include "td5hmi.h"
#include "td5comm.h"
#include "td5strings.h"

static byte brightness = 0;

// Initialize buttons
  
Button btn_up(KPbtn_up, LOW);
Button btn_down(KPbtn_down, LOW);
Button btn_left(KPbtn_left, LOW);
Button btn_right(KPbtn_right, LOW);
Button btn_enter(KPbtn_enter, LOW);
Button btn_quit(KPbtn_quit, LOW);
Button btn_powerup(keyPadPowerPin, LOW);
Button *btns[]={&btn_up, &btn_down, &btn_left, &btn_right, &btn_enter, &btn_quit, &btn_powerup};

ParamList fuellingParams(0, 1, 19, 22, 3, (char**)&fuelling_params, 14, (char**)&fuelling_values, 5, btns);
ParamList optionsParams(0, 1, 19, 3, 3, (char**)&options_params, 14, (char**)&options_values, 5, btns);

extern Adafruit_SSD1306 lcd;

// Class Hmi
Hmi::Hmi(byte c, byte r, byte l)
{
  col = c;
  row = r;
  length = l;
}

// Class LcdBacklight

LcdBacklight::LcdBacklight(byte pinBkLight)
{

}

void LcdBacklight::incBrightness(byte inc)
{

}

void LcdBacklight::decBrightness(byte dec)
{

}

void LcdBacklight::update()
{

}

void LcdBacklight::setBrightness(byte br)
{

}

void LcdBacklight::on()
{

}

void LcdBacklight::off()
{

}


// Class ProgBar
ProgBar::ProgBar(byte c, byte r, byte l, byte bmin , byte bmax):
Hmi(c, r, l)
{
  advance = 0;
  ratio = (float)((float)l / (float)(bmax - bmin));
}

void ProgBar::show()
{
  // here we should save the current screen

  lcd.setCursor(col, row);
  for(int i = col; i < length; i++)
  {
    lcd.setCursor(col*8, row*8);
    lcd.write(254);
  }
}

void ProgBar::hide()
{
  // here we should restore the current screen
  show();
}


void ProgBar::update(byte progr)
{
  byte curr_adv = ((float)progr * ratio);
  if(advance != curr_adv)
  {
    lcd.setCursor(col*8, row*8);
    for(int i = col; i < length; i++)
    {
      lcd.write((i > curr_adv ? 254 : 255));
    }
    advance = curr_adv;
  }
}


  
// Class ScrollBarV
ScrollBarV::ScrollBarV(byte c, byte r, byte h):
Hmi(c, r, 1)
{
  v_height = h;
}

void ScrollBarV::update(byte progr)
{
  int mapped;
  if (progr>99) 
  {
    progr=99;
  }	
  mapped=(int)(v_height*2-2)*progr/100; // This is mapped position, 2 per row of bar.
  for (byte i=0;i<v_height;i++)
  {
    lcd.setCursor(col*8,(row+i)*8);
    if (i==(mapped+1)/2)
    {
      if (i==0)
      {
        lcd.write(0);
      }
      else if (i==v_height-1)
      {
        lcd.write(5);
      }
      else
      {
        if (mapped+1==(mapped+1)/2*2) lcd.write(2);
        else lcd.write(3);
      }
    }
    else
    {
      if (i==0)
      {
        lcd.write(1);
      }
      else if (i==v_height-1)
      {
        lcd.write(4);
      }
      else
      {
        lcd.write(' ');
      }
    }
  }
}


// Class List
List::List(byte c, byte r, byte l, byte items, byte item_scr, char** list):
Hmi(c, r, l)
{
  item_rows = items;
  item_per_screen = item_scr;
  item_selected= 0;
  item_list = list;
}

List::List():
Hmi(0, 0, 0)
{
  item_rows = 0;
  item_per_screen = 0;
  item_selected= 0;
  item_list = 0;
}

void List::init(byte c, byte r, byte l, byte items, byte item_scr, char** list)
{
  col = c;
  row = r;
  length = l;
  item_rows = items;
  item_per_screen = item_scr;
  item_selected= 0;
  item_list = list;
}


void List::show()
{
  render(item_selected);  
}

void List::update(byte progr)
{
  render(progr);  
}


void List::render(byte item_sel)
{
  char indicator = '~';
  char list_buffer[22];
  byte _first_item;
  byte _last_item;

  item_selected = item_sel;

  _first_item=item_selected-item_per_screen/2;
  if (_first_item>127) _first_item=0;
  else if (item_selected-item_per_screen/2+item_per_screen>(item_rows - 1)) _first_item=(item_rows - 1)+1-item_per_screen;
  
  _last_item=_first_item+item_per_screen-1; // Determine last item based on first item, total item per screen, and total item.
  if (_last_item>(item_rows - 1)) _last_item=item_rows - 1;

  for (byte i=_first_item;i<_first_item+item_per_screen;i++)
  {

    //Display item on LCD
    lcd.setCursor((col+((i-_first_item)/item_rows)*(length+1))*8, (row+(i-_first_item)%item_rows)*8); 

    if (i<=_last_item)
    {
      lcd.write((i==item_selected)?indicator:' ');// Show " " or a dot
	  if(i!=item_selected) {
		lcd.fillRect((col+((i-_first_item)/item_rows)*(length+1))*8, (row+(i-_first_item)%item_rows)*8, 8, 8, BLACK);
	  }
    }
    else
    {
      lcd.write(' ');
    } 

    lcd.print(item_list[i]);
  }
  lcd.display();
}

// Class List
ParamList::ParamList(byte c, byte r, byte l, byte items, byte item_scr, char** list, byte l_list, char** param, byte l_param, Button ** b):
Hmi(c, r, l)
{
  item_rows = items;
  item_per_screen = item_scr;
  item_selected= 0;
  item_list = list;
  length_list = l_list;
  length_param = l_param;
  item_param = param;
}

ParamList::ParamList(Button ** b):
Hmi(0, 0, 0)
{
  item_rows = 0;
  item_per_screen = 0;
  item_selected= 0;
  item_list = 0;
  length_list = 0;
  length_param = 0;
}

void ParamList::init(byte c, byte r, byte l, byte items, byte item_scr, char** list, byte l_list, char** param, byte l_param)
{
  col = c;
  row = r;
  length = l;
  item_rows = items;
  item_per_screen = item_scr;
  item_selected= 0;
  item_list = list;
  length_list = l_list;
  length_param = l_param;
  item_param = param;  
}


void ParamList::show()
{
  render(item_selected);  
}

void ParamList::update(byte progr)
{
  render(progr);  
}


void ParamList::render(byte item_sel)
{
  char indicator = '~';
  char list_buffer[22];
  byte _first_item;
  byte _last_item;
  
  item_selected = item_sel;

  _first_item=item_selected-item_per_screen/2;
  if (_first_item>127) _first_item=0;
  else if (item_selected-item_per_screen/2+item_per_screen>(item_rows - 1)) _first_item=(item_rows - 1)+1-item_per_screen;
  
  _last_item=_first_item+item_per_screen-1; // Determine last item based on first item, total item per screen, and total item.
  if (_last_item>(item_rows - 1)) _last_item=item_rows - 1;

  for (byte i=_first_item;i<_first_item+item_per_screen;i++)
  {

    //Display item on LCD
    lcd.setCursor((col+((i-_first_item)/item_rows)*(length+1))*8, (row+(i-_first_item)%item_rows)*8); 

    if (i<=_last_item)
    {
      lcd.write((i==item_selected)?indicator:' ');// Show " " or a dot
    }
    else
    {
      lcd.write(' ');
    }

    lcd.print(item_list[i]);
  }
}


void print_welcome_screen()
{
  lcd.clearDisplay();
  lcd.setCursor(2*8, 0);
  lcd.print(Td5OpenComWelcome);
  lcd.setCursor(4*8, 1*8);
  lcd.print("for STM32"); 
  lcd.setCursor(4*8, 2*8);
  lcd.print("by Luca72");
  lcd.setCursor(3*8, 3*8);
  lcd.print("& BennehBoy");
  lcd.display();
  delay(1000);
}

void print_connect_screen(char *str)
{
  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.setCursor(0, 2*8);
  lcd.print("Connecting to ECU   ");
  lcd.display();
}

void print_instrument_screen(int rpm, float coolant_t, float turbo_p, float battery, float ambient_p, float maf, float inlet_t, float fuel_t)
{
  lcd.clearDisplay();
  print_pid(0, 0, "RPM", rpm, 0); 
  print_pid(10*8, 0, "CoT", coolant_t, 1);
  print_pid(0, 1*8, "TbP", turbo_p, 2); 
  print_pid(10*8, 1*8, "BtV", battery, 1);
  print_pid(0, 2*8, "AmP", ambient_p, 2); 
  print_pid(10*8, 2*8, "MAF", maf, 1);
  print_pid(0, 3*8, "InT", inlet_t, 1); 
  print_pid(10*8, 3*8, "FuT", fuel_t, 1);
  lcd.display();
}

void print_read_fault_codes_screen(int index, int count, char* description)
{
  char buf[7];
  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.print("Read Fault C.");
  lcd.setCursor(15*8, 0);
  sprintf(buf, "%02d-%02d", index, count);
  lcd.print(buf);
  lcd.setCursor(0, 1*8);
  lcd.print(description);
  lcd.display();
}

void print_reset_fault_codes_screen(boolean yes_no)
{
  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.print("Reset Fault Codes");
  lcd.setCursor(3*8, 2*8);
  if(yes_no)
  {
    lcd.print("[YES]   NO ");
  }
  else
  {
    lcd.print(" YES   [NO]");
  }
  lcd.display();
}

void print_fuelling_screen(byte currentParam, boolean logActive)
{
  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  #ifdef _USE_SD_  
  if(logActive)
  {
    lcd.print("Fuelling (log=ON)");    
  }
  else
  {
    lcd.print("Fuelling (log=OFF)");        
  }
  #else
  lcd.print("Fuelling");
  #endif
  fuellingParams.update(currentParam);
  fuellingParams.show();
  lcd.display();
}

void print_options_screen(byte currentParam)
{
  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.print("Options");
  optionsParams.update(currentParam);
  optionsParams.show();
  lcd.display();
}

void update_fuelling_screen(byte currentParam)
{
  fuellingParams.update(currentParam);
}


void print_pid(byte col, byte row, char* tag, float fvalue, byte precision)
{
  lcd.setCursor(col, row);
  lcd.print(tag);
  byte colOffset;
  if(precision == 0)
  {
    colOffset = 5;
  }
  else
  {
    colOffset = 5 - precision;
  }
  lcd.setCursor((col + colOffset)*8, row*8);
  if (fvalue < 10.0)
    lcd.print(' ');
  if (fvalue < 100.0)
    lcd.print(' ');
  if ((fvalue < 1000.0) && (precision == 0))
    lcd.print(' ');
  lcd.print(fvalue, precision);
}

void setBrightness(byte bright)
{
  brightness = bright;   
}

void incBrightness(byte inc)
{
  brightness += inc;   
}

void decBrightness(byte inc)
{
  brightness += inc;   
}

