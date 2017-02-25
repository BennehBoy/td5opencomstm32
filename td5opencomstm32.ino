/*

 Td5OpenCom
 
 modified 12 Jan 2013
 by Luca Veronesi
 
 This code is in the public domain.
 
 */

// SD card: it uses too much RAM on a 328p. Uncomment on Mega 2560

//#include <avr/pgmspace.h>
//#include <avr/sleep.h>
//#include <avr/power.h>
//#include <SoftwareSerialEx.h>
//#include <LiquidCrystalEx.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
//#include <E24C1024.h>
#include <Buttons.h>
//#include <MemoryFree.h>
#include "td5defs.h"
#ifdef _USE_SD_
#include <SdFat.h>
#endif
#include "td5strings.h"
#include "td5comm.h"
#include "td5hmi.h"
#include "td5menu.h"
#include "td5remote.h"


#define MODE_STARTUP                0
#define MODE_TO_STANDBY            10
#define MODE_STANDBY 	           15
#define MODE_TO_INSTRUMENT         20
#define MODE_INSTRUMENT            25
#define MODE_TO_MENU               30
#define MODE_MENU                  31
#define MODE_TO_OPTIONS            32
#define MODE_OPTIONS               33
#define MODE_TO_FAULT_CODES_READ   40
#define MODE_FAULT_CODES_READ      41
#define MODE_TO_FAULT_CODES_RESET  42
#define MODE_FAULT_CODES_RESET     43
#define MODE_TO_FUELLING           44
#define MODE_FUELLING              45
#define MODE_TO_REMOTE             90
#define MODE_REMOTE                95

#define STARTUP_MODE_MENU          31
#define STARTUP_MODE_INSTRUMENT    25

// Initialize global variables
char p_buffer[32];
static byte deviceMode = MODE_STARTUP;
static byte startupMode = MODE_INSTRUMENT;
static boolean sd_present = false;
#ifdef _USE_BLUETOOTH_
static boolean bt_module_enable = true;
#endif
static byte modeState = 0;
static boolean logFuelling = false;

byte menuChoice = 0;

// Initialize global objects
//SoftwareSerialEx obdSerial(K_IN, K_OUT); // RX, TX
//LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Adafruit_SSD1306 lcd(OLED_DC, OLED_RESET, OLED_CS);
//LcdBacklight lcdbacklight(lcdBKlightPin);
Td5Comm td5;
Td5Remote remote;
Td5Menu menu(btns);
#ifdef _USE_SD_
SdFat sd;
SdFile sdLogFile;
#endif

void leaveSleep()
{
/*   sleep_disable();
  detachInterrupt(1); */
}

void enterSleep()
{
/*   sleep_enable();
  attachInterrupt(1, leaveSleep, LOW);
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  #ifdef _DEBUG_
  Serial.println(P("Enter sleep mode"));
  #endif
  delay(100);
  sleep_mode(); */
  /* The program will continue from here. */
  //#ifdef _DEBUG_
  //Serial.println(P("Leave sleep mode"));
  //#endif
  //sleep_disable();
}

void setup()
{
  // Open serial communications
  Serial.begin(57600);
  #ifdef _DEBUG_
  Serial.println("Startup");
  Serial.print("Free Memory: ");
  //Serial.println(freeMemory());
  #endif

  #ifdef _USE_BLUETOOTH_
  // Initialize remote control
  remote.init();
  #endif
  
  //startupMode=STARTUP_MODE_INSTRUMENT;
  startupMode=STARTUP_MODE_MENU;

  pinMode (OLED_CS, OUTPUT);
  digitalWrite(OLED_CS, HIGH);
  
  lcd.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, true); //construct our displays


  lcd.clearDisplay();   // clears the screen and buffer


  lcd.display();
  
  lcd.setTextColor(WHITE);
  lcd.setTextSize(1);

  // Initialize LCD
  //lcd.begin(20, 4);

  // Initialize OBD comunication
  td5.init();
  
  // Initialize Menu
  menu.init();
  
  // Initialize Lcd Backlight
  //lcdbacklight.setBrightness(70);
  
  #ifdef _USE_SD_
  // Initialize SD reader
  if (!sd.begin(SD_CS, SPI_FULL_SPEED))
  {
    #ifdef _DEBUG_
    Serial.println("SD init failed...");
    #endif
    lcd.setCursor(0, 24); 
    lcd.println("SD init failed...   ");
	lcd.display();
    sd_present = false;
  }
  else
  {
    #ifdef _DEBUG_
    Serial.println("SD init OK");
    #endif
    sd_present = true;    
  }
  #endif
}

void loop()
{
  // main state machine
  switch(deviceMode)
  {
  case MODE_STARTUP:
    modeStartup();
    break;

  case MODE_TO_STANDBY:
    modeToStanby();
    break;
    
  case MODE_STANDBY:
    modeStandby();
    break;

  case MODE_TO_MENU:
    modeToMenu();
    break;

  case MODE_MENU:
    modeMenu();
    break;

  case MODE_TO_INSTRUMENT:
    modeToInstrument();
    break;

  case MODE_INSTRUMENT:
    modeInstrument();
    break;

  case MODE_TO_FAULT_CODES_READ:
    modeToFaultCodesRead();
    break;

  case MODE_FAULT_CODES_READ:
    modeFaultCodesRead();
    break;

  case MODE_TO_FAULT_CODES_RESET:
    modeToFaultCodesReset();
    break;

  case MODE_FAULT_CODES_RESET:
    modeFaultCodesReset();
    break;
    
  case MODE_TO_FUELLING:
    modeToFuelling();
    break;

  case MODE_FUELLING:
    modeFuelling();
    break;
    
  case MODE_TO_REMOTE:
    modeToRemote();
    break;
    
  case MODE_REMOTE:
    modeRemote();
      break;
      
  case MODE_TO_OPTIONS:
    modeToOptions();
    break;
    
  case MODE_OPTIONS:
    modeOptions();
      break;      
  }

     
  if(td5.ecuIsConnected())
  {
    // keep ecu alive
    if(td5.getLastReceivedPidElapsedTime() > KEEP_ALIVE_TIME)
    {
      td5.getPid(&pidKeepAlive);  
    }

    // shutdown in case of too many frames lost
    if (td5.getConsecutiveLostFrames() > 3)
    {
      #ifdef _DEBUG_
      Serial.println("Connection error:");
      Serial.println("Too many lost frames");      
      Serial.println("To standby MODE");
      #endif
      lcd.setCursor(0, 24);
      lcd.print("Too many lost frames");
	  lcd.display();
      delay(1000);
      lcd.setCursor(0, 24);
      lcd.print("Shutdown...         ");
	  lcd.display();
      delay(500);
      lcd.clearDisplay();

      td5.disconnectFromEcu();
      deviceMode = MODE_TO_STANDBY;
    }
  }
  
  if(!remote.isConnected())
  {
    if(remote.getCommand() == REMOTE_CONNECT_BT)
    {
      #ifdef _DEBUG_
      Serial.println("Remote connection request");
      #endif
      remote.connect();
      deviceMode = MODE_TO_REMOTE;
    }  
  }
}





///////////////  Mode Startup ///////////////
void modeStartup()
{
  #ifdef _DEBUG_
  Serial.println("Standby MODE");
  #endif
  deviceMode = MODE_STANDBY;
}

//////////////  Mode To Standby //////////////
void modeToStanby()
{
  #ifdef _DEBUG_
  Serial.println("Standby MODE");
  #endif
  #ifdef _USE_BLUETOOTH_
  remote.btModuleOff();
  #endif  
  deviceMode = MODE_STANDBY;
}

///////////////  Mode Standby ///////////////
void modeStandby()
{
  
  enterSleep();

//  if(btn_powerup.sense()==buttons_released)
//  {
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    #ifdef _USE_BLUETOOTH_
    if(bt_module_enable)
    {
      remote.btModuleOn();
    }
    #endif      
    print_welcome_screen();
	lcd.display();
	lcd.clearDisplay();
    delay(1000);
    if(startupMode == STARTUP_MODE_INSTRUMENT)
    {
      deviceMode = MODE_TO_INSTRUMENT;
    }
    if(startupMode == STARTUP_MODE_MENU)
    {
      deviceMode = MODE_TO_MENU;
    }
//  }
}

/////////////  Mode To Instrument /////////////
void modeToInstrument()
{
  if(td5.ecuIsConnected())
  {
    #ifdef _DEBUG_
    Serial.println("Instrument MODE");
    #endif
    print_instrument_screen();
    deviceMode = MODE_INSTRUMENT;
  }
  else
  {
    #ifdef _DEBUG_
    Serial.println("Connecting...");
    #endif
    print_connect_screen("Instrument Mode");
    if(td5.connectToEcu())
    {
      #ifdef _DEBUG_
      Serial.println("Connected");
      #endif
      lcd.setCursor(0, 3*8);
      lcd.print("Connected           ");
	  lcd.display();
      td5.getPid(&pidStartFuelling);
      return;
    }
    #ifdef _DEBUG_
    Serial.println("Connection failed");
    #endif
    lcd.setCursor(0, 3*8);
    lcd.print("Connection failed   ");
	lcd.display();
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    td5.disconnectFromEcu();
    delay(1000);
    deviceMode = MODE_TO_MENU;
  }
}

//////////////  Mode Instrument //////////////
void modeInstrument()
{
  if(btn_powerup.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To standby MODE");
    #endif
    lcd.setCursor(0, 3*8);
    lcd.print("Shutdown...         ");
	lcd.display();
    td5.disconnectFromEcu();
    lcd.clearDisplay();
    startupMode = STARTUP_MODE_INSTRUMENT;
    deviceMode = MODE_TO_STANDBY;
  }

  if(btn_quit.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    lcd.setCursor(0, 3*8);
    lcd.print("Quit Instrument Mode");
	lcd.display();
    deviceMode = MODE_TO_MENU;
  }

  if(btn_left.sense()==buttons_released)
  {
    //lcdbacklight.decBrightness(10);
  }
  
  if(btn_right.sense()==buttons_released)
  {
    //lcdbacklight.incBrightness(10);
  }

  if(td5.ecuIsConnected())
  {
    td5.instrumentCycle();
  }
}

////////////////  Mode To Menu ////////////////
void modeToMenu()
{
  #ifdef _DEBUG_
  Serial.println("Menu MODE");
  #endif
  menu.setTopMenu();
  deviceMode = MODE_MENU;
}

/////////////////  Mode Menu /////////////////
void modeMenu()
{
  menuChoice = menu.selectMenu();

  if(menuChoice == MAIN_MENU_INSTRUMENT)
  {
    #ifdef _DEBUG_
    Serial.println("To instrument MODE");
    #endif
    deviceMode = MODE_TO_INSTRUMENT;
  }

  if(menuChoice == SUB_MENU_ECU_FAULT_CODES_READ)
  {
    #ifdef _DEBUG_
    Serial.println("To f.c. read MODE");
    #endif
    deviceMode = MODE_TO_FAULT_CODES_READ;
  }

  if(menuChoice == SUB_MENU_ECU_FAULT_CODES_RESET)
  {
    #ifdef _DEBUG_
    Serial.println("To f.c. reset MODE");
    #endif
    deviceMode = MODE_TO_FAULT_CODES_RESET;
  }

  if(menuChoice == SUB_MENU_ECU_FUELLING)
  {
    #ifdef _DEBUG_
    Serial.println("To fuelling MODE");
    #endif
    deviceMode = MODE_TO_FUELLING;
  }

  if(menuChoice == MAIN_MENU_OPTIONS)
  {
    #ifdef _DEBUG_
    Serial.println("To options MODE");
    #endif
    deviceMode = MODE_TO_OPTIONS;
  }
  
  if(menuChoice == MENU_LEFT_BTN)
  {
    //lcdbacklight.decBrightness(10);
  }

  if(menuChoice == MENU_RIGHT_BTN)
  {
    //lcdbacklight.incBrightness(10);
  }
  
  if(menuChoice == MENU_POWER_OFF)
  {
    #ifdef _DEBUG_
    Serial.println("To standby MODE");
    #endif
    if(td5.ecuIsConnected())
    {
      td5.disconnectFromEcu();
    }
    startupMode = STARTUP_MODE_MENU;
    deviceMode = MODE_TO_STANDBY;
  }
}

////////  Mode To Read Fault Codes //////////
void modeToFaultCodesRead()
{
  if(td5.ecuIsConnected())
  {
    #ifdef _DEBUG_
    Serial.println("Fault c. read MODE");
    #endif
    print_read_fault_codes_screen(0, 0, "");
    modeState = 0; // Initialize local state machine
    deviceMode = MODE_FAULT_CODES_READ;
  }
  else
  {
    #ifdef _DEBUG_
    Serial.println("Connecting...");
    #endif
    print_connect_screen("Read Fault Codes");
    if(td5.connectToEcu())
    {
      #ifdef _DEBUG_
      Serial.println("Connected");
      #endif
      lcd.setCursor(0, 16);
      lcd.print("Connected           ");
	  lcd.display();
	  lcd.clearDisplay();
      return;
    }
    #ifdef _DEBUG_
    Serial.println("Connection failed");
    #endif
    lcd.setCursor(0, 16);
    lcd.print("Connection failed   ");
	lcd.display();
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    td5.disconnectFromEcu();
    delay(1000);
    deviceMode = MODE_TO_MENU;
  }  
}

//////////  Mode Fault Codes Read ////////////
void modeFaultCodesRead()
{
  if(btn_quit.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    menu.quitCurrentMenu();
    deviceMode = MODE_MENU;
  }

  if(td5.ecuIsConnected())
  {
    static int currentFaultCode;

    switch(modeState)
    {
      case 0: // read pid
        if(td5.getFaultCodes() != -1)
        {
          currentFaultCode = 1;
          modeState++;
        }
        break;
      
      case 1: // show current fault code   
        #ifdef _FAULT_CODES_STRINGS_  
        char fault_code_buffer[62];
        int faultCode; 
        
        faultCode = td5.getFaultCode(currentFaultCode-1);        
        strcpy_P(fault_code_buffer, (char*)pgm_read_word(&(fault_code[faultCode])));
        print_read_fault_codes_screen(currentFaultCode, td5.getFaultCodesCount(), fault_code_buffer);
        if(strcmp(fault_code_buffer, "Unknown")==0)
        {
          char code[8];
          sprintf(code, "(%d-%d)", (int)(faultCode/8)+1, faultCode-((int)(faultCode/8)*8)+1);
          lcd.print(code);
        }
		lcd.display();
        #endif
        modeState++;
        break;
        
      case 2: // scroll fault codes     
        if(btn_up.sense()==buttons_released)
        {
          if(currentFaultCode > 1)
          {
            currentFaultCode -= 1;
          }
          modeState--;
        }
        if(btn_down.sense()==buttons_released)
        {
          if(currentFaultCode < td5.getFaultCodesCount())
          {
            currentFaultCode += 1;
          }
          modeState--;
        }
        break;
    }
  }

  if(btn_powerup.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To standby MODE");
    #endif
    if(td5.ecuIsConnected())
    {
      td5.disconnectFromEcu();
    }
    startupMode = STARTUP_MODE_MENU;
    deviceMode = MODE_TO_STANDBY;
  }  
}


////////  Mode To Reset Fault Codes ////////
void modeToFaultCodesReset()
{
  if(td5.ecuIsConnected())
  {
    #ifdef _DEBUG_
    Serial.println("Fault c. reset MODE");
    #endif
    print_reset_fault_codes_screen(false);
    modeState = 0; // Initialize local state machine
    deviceMode = MODE_FAULT_CODES_RESET;
  }
  else
  {
    #ifdef _DEBUG_
    Serial.println("Connecting...");
    #endif
    print_connect_screen("Reset Fault Codes");
    if(td5.connectToEcu())
    {
      #ifdef _DEBUG_
      Serial.println("Connected");
      #endif
      lcd.setCursor(0, 16);
      lcd.print("Connected           ");
	  lcd.display();
      return;
    }
    #ifdef _DEBUG_
    Serial.println("Connection failed");
    #endif
    lcd.setCursor(0, 16);
    lcd.print("Connection failed   ");
	lcd.display();
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    td5.disconnectFromEcu();
    delay(1000);
    deviceMode = MODE_TO_MENU;
  }
}

////////  Mode Reset Fault Codes //////////
static boolean yes_no_selection = false;
static boolean execute = false;
void modeFaultCodesReset()
{
  if(btn_quit.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    menu.quitCurrentMenu();
    deviceMode = MODE_MENU;
  }
  
  //if(menuChoice == MENU_LEFT_BTN)
  if(btn_left.sense()==buttons_released)
  {
    print_reset_fault_codes_screen(true);
    yes_no_selection = true;
  }

  if(btn_right.sense()==buttons_released)
  {
    print_reset_fault_codes_screen(false);
    yes_no_selection = false;    
  }  

  if(btn_enter.sense()==buttons_released)
  {
    if(yes_no_selection)
    {
      #ifdef _DEBUG_
      Serial.println("Yes selected");
      #endif
      execute = true;
    }
    else
    {
      #ifdef _DEBUG_
      Serial.println("No selected");
      #endif
      execute = false;
      menu.quitCurrentMenu();
      #ifdef _DEBUG_
      Serial.println("To menu MODE");
      #endif
      deviceMode = MODE_MENU;      
    }
  }  

  if(td5.ecuIsConnected())
  {
    switch(modeState)
    {
      case 0: // read pid
        if(execute)
        {
          int8_t response = 0;
          response = td5.resetFaults();
          if(response > 0)
          {
            #ifdef _DEBUG_
            Serial.println("Reset faults OK");
            #endif
            lcd.setCursor(0, 24);
            lcd.print("Reset OK            ");
			lcd.display();
            modeState++;
          }
          if(response == PID_NEGATIVE_ANSWER)
          {
            #ifdef _DEBUG_
            Serial.println("Reset faults refused");
            #endif
            // reset refused: go back to last menu
            lcd.setCursor(0, 24);
            lcd.print("Reset failed        ");
			lcd.display();
            modeState++;
          }
        }
        break;
     
      case 1: // go back to last menu
        yes_no_selection = false;
        execute = false;
        delay(1000);
        menu.quitCurrentMenu();
        #ifdef _DEBUG_
        Serial.println("To menu MODE");
        #endif
        deviceMode = MODE_MENU;        
    }
  }

  if(btn_powerup.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To standby MODE");
    #endif
    if(td5.ecuIsConnected())
    {
      td5.disconnectFromEcu();
    }
    startupMode = STARTUP_MODE_MENU;    
    deviceMode = MODE_TO_STANDBY;
  }  
}


/////////////  Mode To Fuelling ///////////////
void modeToFuelling()
{
  if(td5.ecuIsConnected())
  {
    #ifdef _DEBUG_
    Serial.println("Fuelling MODE");
    #endif
    print_fuelling_screen(0, logFuelling);
    modeState = 0;
    deviceMode = MODE_FUELLING;
  }
  else
  {
    #ifdef _DEBUG_
    Serial.println("Connecting...");
    #endif
    print_connect_screen("Fuelling Mode");
    if(td5.connectToEcu())
    {
      #ifdef _DEBUG_
      Serial.println("Connected");
      #endif
      lcd.setCursor(0, 16);
      lcd.print("Connected           ");
	  lcd.display();
      td5.getPid(&pidStartFuelling);
      return;
    }
    #ifdef _DEBUG_
    Serial.println("Connection failed");
    #endif
    lcd.setCursor(0, 16);
    lcd.print("Connection failed   ");
	lcd.display();

    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    td5.disconnectFromEcu();
    delay(1000);
    deviceMode = MODE_TO_MENU;
  }
}

//////////////  Mode Fuelling ////////////////
void modeFuelling()
{
  static byte currentFuellingParam;
  static unsigned long initTime;

  if(btn_quit.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    #ifdef _USE_SD_
    if(sd_present && logFuelling)
    {
      #ifdef _DEBUG_
      Serial.println("Fuelling log OFF");
      #endif      
      if (sdLogFile.isOpen())
      { 
        sdLogFile.close();
      }
      logFuelling = false;
    }
    #endif
    menu.quitCurrentMenu();
    deviceMode = MODE_MENU;
  }

  if(btn_powerup.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To standby MODE");
    #endif
    #ifdef _USE_SD_
    if(sd_present && logFuelling)
    {
      #ifdef _DEBUG_
      Serial.println("Fuelling log OFF");
      #endif      
      if (sdLogFile.isOpen())
      { 
        sdLogFile.close();
      }
      logFuelling = false;
    }
    #endif
    lcd.setCursor(0, 24);
    lcd.print("Shutdown...         ");
	lcd.display();
    td5.disconnectFromEcu();
    lcd.clearDisplay();
    startupMode = STARTUP_MODE_MENU;
    deviceMode = MODE_TO_STANDBY;
  }  

  #ifdef _USE_SD_
  if((btn_enter.sense()==buttons_held))
  {
    if(!logFuelling)
    {
      #ifdef _DEBUG_
      Serial.println("Fuelling log ON");
      #endif 
      if(sd_present)
      {
        char file_name[] = "fuel_00.csv";
        // if name exists, create new filename
        for (int i=0; i<100; i++) 
        {
          file_name[5] = i/10 + '0';
          file_name[6] = i%10 + '0';
          if (sdLogFile.open(file_name, O_CREAT | O_EXCL | O_WRITE)) 
          {
            break;
          }
        }

        if (sdLogFile.isOpen()) 
        {
          #ifdef _DEBUG_
          Serial.println("File opening OK.      ");
          #endif 
          sdLogFile.println("Td5OpenCom Fuelling Log file");sdLogFile.println(); 
          sdLogFile.print("Rpm;");
          sdLogFile.print("Speed;");
          sdLogFile.print("Idle spd.;");
          sdLogFile.print("Thr.pot 1;");
          sdLogFile.print("Thr.pot 2;");
          sdLogFile.print("Thr.pot 3;");
          sdLogFile.print("Thr.supp.;");
          sdLogFile.print("Battery;");
          sdLogFile.print("MAF;");
          sdLogFile.print("Amb. pres.;");
          sdLogFile.print("Man. pres.;");
          sdLogFile.print("Air inl.t.;");
          sdLogFile.print("Coolant t.;");
          sdLogFile.print("Fuel t.;");
          sdLogFile.print("ILT mod.;");
          sdLogFile.print("EGR mod.;");
          sdLogFile.print("TWG mod.;");
          sdLogFile.print("Inj.bal.1;");
          sdLogFile.print("Inj.bal.2;");
          sdLogFile.print("Inj.bal.2;");
          sdLogFile.print("Inj.bal.4;");
          sdLogFile.println("Inj.bal.5;");
          logFuelling = true;
          initTime = millis() + 1000;
        } 
        else 
        {
          #ifdef _DEBUG_
          Serial.println("File opening failed...");
          #endif 
          logFuelling = false;
        }    
      }
      print_fuelling_screen(currentFuellingParam, logFuelling);
    }
  }
  
  if(btn_quit.sense()==buttons_held)
  {
    if(logFuelling)
    {
      #ifdef _DEBUG_
      Serial.println("Fuelling log OFF");
      #endif      
      print_fuelling_screen(currentFuellingParam, false);
      if (sdLogFile.isOpen())
      { 
        sdLogFile.close();
      }
      logFuelling = false;
    }
  }
  #endif      
 
  if(btn_left.sense()==buttons_released)
  {
    //lcdbacklight.decBrightness(10);
  }

  if(btn_right.sense()==buttons_released)
  {
    //lcdbacklight.incBrightness(10);
  }

  if(td5.ecuIsConnected())
  {
    td5.fuellingCycle();
    
    switch(modeState)
    {
      case 0: // read pid
          currentFuellingParam = 0;
          modeState++;
        break;
      
      case 1: // show current fuelling param   
        print_fuelling_screen(currentFuellingParam, logFuelling);
        modeState++;
        break;
        
      case 2: // scroll fuelling params
        if(btn_up.sense()==buttons_released)
        {
          if(currentFuellingParam > 0)
          {
            currentFuellingParam -= 1;
          }
          modeState--;
        }
        if(btn_down.sense()==buttons_released)
        {
          if(currentFuellingParam < (MAX_FUELLING_PARAMS-1))
          {
            currentFuellingParam += 1;
          }
          modeState--;
        }
        if(td5.newDataIsAvailable())
        {
          update_fuelling_screen(currentFuellingParam);
        }
        break;
    }
    #ifdef _USE_SD_
    if(logFuelling && (millis() >= initTime))  
    {
      if (sdLogFile.isOpen())
      { 
        sdLogFile.print(pidRPM.getlValue());sdLogFile.print(";");
        sdLogFile.print(pidVehicleSpeed.getbValue(0));sdLogFile.print(";");
        sdLogFile.print(pidRPMError.getlValue());sdLogFile.print(";");
        sdLogFile.print(pidThrottlePosition.getfValue(0));sdLogFile.print(";");
        sdLogFile.print(pidThrottlePosition.getfValue(1));sdLogFile.print(";");
        sdLogFile.print(pidThrottlePosition.getfValue(2));sdLogFile.print(";");
        sdLogFile.print(pidThrottlePosition.getfValue(4));sdLogFile.print(";");
        sdLogFile.print(pidBatteryVoltage.getfValue());sdLogFile.print(";");
        sdLogFile.print(pidTurboPressureMaf.getfValue(2));sdLogFile.print(";");
        sdLogFile.print(pidAmbientPressure.getfValue(1)*100.0F);sdLogFile.print(";");
        sdLogFile.print((pidTurboPressureMaf.getfValue(0)-pidAmbientPressure.getfValue(1))*100.0F);sdLogFile.print(";");
        sdLogFile.print(pidTemperatures.getfValue(1));sdLogFile.print(";");
        sdLogFile.print(pidTemperatures.getfValue(0));sdLogFile.print(";");
        sdLogFile.print(pidTemperatures.getfValue(3));sdLogFile.print(";");
        sdLogFile.print(pidILT.getfValue());sdLogFile.print(";");
        sdLogFile.print(pidEGR.getfValue());sdLogFile.print(";");
        sdLogFile.print(0.0);sdLogFile.print(";");
        sdLogFile.print(pidInjectorsBalance.getlValue(0));sdLogFile.print(";");
        sdLogFile.print(pidInjectorsBalance.getlValue(1));sdLogFile.print(";");
        sdLogFile.print(pidInjectorsBalance.getlValue(2));sdLogFile.print(";");
        sdLogFile.print(pidInjectorsBalance.getlValue(3));sdLogFile.print(";");
        sdLogFile.print(pidInjectorsBalance.getlValue(4));sdLogFile.println();        
        initTime = millis() + 1000;
      }      
    }
    #endif 
  }
}


//////////////  Mode To Remote //////////////
void modeToRemote()
{
  #ifdef _DEBUG_
  Serial.println("Remote MODE");
  #endif
  remote.aknCommand();
  //lcdbacklight.off();

  lcd.setCursor(0, 0);
  lcd.print("Remote mode");
  lcd.display();
  lcd.clearDisplay();
  deviceMode = MODE_REMOTE;
}

///////////////  Mode Remote ///////////////
void modeRemote()
{
  byte command = 0;
  command = remote.getCommand();

  if (command == REMOTE_CONNECT_OBD)  
  {
    #ifdef _DEBUG_
    Serial.println("Connecting...");
    #endif
    if(td5.connectToEcu(false))
    {
      #ifdef _DEBUG_
      Serial.println("Connected");
      #endif
      remote.aknCommand();
      td5.getPid(&pidStartFuelling);
      return;
    }
    #ifdef _DEBUG_
    Serial.println("Connection failed");
    #endif
    remote.aknCommand(false);
    td5.disconnectFromEcu();
  }
  
  if(btn_powerup.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To standby MODE");
    #endif
    lcd.setCursor(0, 24);
    lcd.print("Shutdown...         ");
	lcd.display();
    td5.disconnectFromEcu();
    lcd.clearDisplay();
    deviceMode = MODE_TO_STANDBY;
  }

  if((btn_quit.sense()==buttons_released) || (command == REMOTE_DISCONNECT_BT))
  {
    #ifdef _DEBUG_
    Serial.println("Remote mode disconnected");
    Serial.println("To menu MODE");
    #endif
    remote.aknCommand();
    remote.disconnect();
    lcd.setCursor(0, 24);
    lcd.print("Quit Instrument Mode");
    //lcdbacklight.on();
    lcd.display();    
    lcd.clearDisplay();
    deviceMode = MODE_TO_MENU;
  }
}


////////  Mode To Options ////////
void modeToOptions()
{
  print_options_screen(0);
  modeState = 0;
  deviceMode = MODE_OPTIONS;
}

////////  Mode Options //////////
void modeOptions()
{
  static byte currentOptionsParam;

  if(btn_quit.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To menu MODE");
    #endif
    menu.quitCurrentMenu();
    deviceMode = MODE_MENU;
  }

  switch(modeState)
  {
    case 0: // init
        currentOptionsParam = 0;
        modeState++;
      break;
    
    case 1: // show current options param   
      print_options_screen(currentOptionsParam);
      modeState++;
      break;
      
    case 2: // scroll options params
      if(btn_up.sense()==buttons_released)
      {
        if(currentOptionsParam > 0)
        {
          currentOptionsParam -= 1;
        }
        modeState--;
      }
      if(btn_down.sense()==buttons_released)
      {
        if(currentOptionsParam < (MAX_OPTIONS_PARAMS-1))
        {
          currentOptionsParam += 1;
        }
        modeState--;
      }
      break;
  }

  if(btn_powerup.sense()==buttons_released)
  {
    #ifdef _DEBUG_
    Serial.println("To standby MODE");
    #endif
    if(td5.ecuIsConnected())
    {
      td5.disconnectFromEcu();
    }
    startupMode = STARTUP_MODE_MENU;    
    deviceMode = MODE_TO_STANDBY;
  }  
}


