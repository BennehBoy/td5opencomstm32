/*
Td5Defs.h -

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

#ifndef Td5Defs_h
#define Td5Defs_h

#define Td5OpenComWelcome  "Td5OpenCom v0.1"

// Debug mode: enables debug comments on serial monitor.
#define _DEBUG_

// SD card option: uses too much RAM, enable only with ATMega2560.
#define _USE_SD_

// Bluetooth option.
//#define _USE_BLUETOOTH_

// If uncommented shows fault codes full string, otherwise shows fault index.
#define _FAULT_CODES_STRINGS_






	#define MOSI                 PA7  //4
	#define MISO                 PA6  //5
	#define SCK                  PA5  //6
	#define keyPadPowerPin       PA13 //22
	#define KPbtn_quit 			 PA0  //11
	#define KPbtn_enter 		 PC15 //12	
	#define SD_CS                PC14 //13
	#define btEnablePin          PC13 //14
	#define KPbtn_up 			 PA14 //21
	#define OLED_CS     		 PB5  //17
	#define KPbtn_down 			 PB4  //18
	#define KPbtn_left 			 PB3  //19
	#define KPbtn_right 	 	 PA15 //20
	#define ledPin               PB12 //31
	#define OLED_RESET  		 PA8  //27
	#define OLED_DC     		 PB15 //28
	#define K_OUT                PA9  //26   //TX1
	#define K_IN                 PA10 //25	  //RX1
  //#define SDA                  16  Already defined in Wire.h
  //#define SCL                  15  Already defined in Wire.h


#define LCD_COLUMNS          20
#define LCD_ROWS              4

#define MAX_OPTIONS_PARAMS    3

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))


  #define remoteSerial        Serial3


#endif
