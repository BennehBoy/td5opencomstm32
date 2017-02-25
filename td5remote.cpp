/*
Td5remote.cpp -
 
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
//#include <avr/pgmspace.h>
#include "td5remote.h"
#include "td5strings.h"

boolean read_byte(byte * b)
{
  int readData;
  boolean success = true;
  byte t=0;

  //digitalWrite(ledPin, HIGH);
  while(t != READ_ATTEMPTS  && (readData=remoteSerial.read())==-1) 
  {
    delay(1);
    t++;
  }
  //digitalWrite(ledPin, LOW);

  if (t >= READ_ATTEMPTS) 
  {
    success = false;
  }

  if (success)
  {
    *b = (byte) readData;
  }

  return success;
}

void write_byte(byte b)
{
  remoteSerial.write(b);
}

void remote_send_byte(byte b)
{
  if (b < 16) 
  {
    Serial.print("0"); 
  }        
  Serial.print(b, HEX); 
  Serial.print(" "); 
}

///////////////////////////////////////////////////
//                Class Td5Remote                //
///////////////////////////////////////////////////
Td5Remote::Td5Remote()
{
  remoteConnected = false;
  command = 0x00;
}

void Td5Remote::init()
{
  remoteSerial.begin(57600);
  pinMode(btEnablePin, OUTPUT);
  btModuleOff();
}

void Td5Remote::btModuleOn()
{
  digitalWrite(btEnablePin, HIGH);    
}

void Td5Remote::btModuleOff()
{
  digitalWrite(btEnablePin, LOW);  
}

void Td5Remote::connect()
{
  remoteConnected = true;
}

void Td5Remote::disconnect()
{
  remoteConnected = false;
}

byte Td5Remote::recvCommand()
{
  boolean gotData = false;
  byte responseIndex = 0;
  byte dataCaught = '\0';

  if(remoteSerial.available() > 0) 
  {
    while(read_byte(&dataCaught))
    {
      gotData = true;        
      #ifdef _DEBUG_
      Serial.print("Received: ");Serial.println(dataCaught,HEX);
      #endif
      if(responseIndex > REMOTE_MAX_COMMAND_LENGTH)  
      {
        #ifdef _DEBUG_
        Serial.print("Command too long");
        #endif          
        return REMOTE_COMM_FAIL;  
      }
      commandBuffer[responseIndex] = dataCaught;
      responseIndex++;
    }
      
    // here verify checksum and frame length
    if(checksum(commandBuffer, responseIndex-1) != commandBuffer[responseIndex-1])
    {
      #ifdef _DEBUG_
      Serial.println("Wrong checksum");
      #endif      
      aknCommand(false);
      return REMOTE_COMM_FAIL;
    }
    if(gotData && (responseIndex > 1))
    {
      #ifdef _DEBUG_
      Serial.print("Command: ");Serial.println(commandBuffer[1],HEX);
      #endif
      command = commandBuffer[1];
      return command;
    }
    else
    {
      #ifdef _DEBUG_
      Serial.println("Wrong telegram structure");
      #endif
      aknCommand(false);      
      return REMOTE_COMM_FAIL;
    }
  } 
  return REMOTE_NO_COMM;
}

byte Td5Remote::checksum(byte *data, byte len)
{
  byte crc=0;
  for(byte i=0; i<len; i++)
    crc=crc+data[i];
  return crc;
}

void Td5Remote::aknCommand(boolean positiveAnswer)
{
  byte buffer[3];
  if(positiveAnswer)
  { 
    buffer[0] = 0x01;
    buffer[1] = commandBuffer[1] + 0x20;
    sendData(buffer, 2);
  }
  else
  {
    buffer[0] = 0x01;
    buffer[1] = 0x7F;
    buffer[2] = commandBuffer[1];
    sendData(buffer, 3);
  }
  #ifdef _DEBUG_
  Serial.println("Reset actual command");
  #endif  
  command = 0x00; 
}

byte Td5Remote::getCommand()
{
  if(command == 0x00)
  {
    return recvCommand();   
  }
  else
  {
    return command;
  }
}

void Td5Remote::sendData(byte* data, byte length)
{
  for(int i=0; i < length; i++)
  {
    write_byte(data[i]);
  }
  write_byte(checksum(data, length));
}


