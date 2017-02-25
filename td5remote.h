/*
Td5remote.h -
 
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

#ifndef Td5Remote_h
#define Td5Remote_h

#include "td5defs.h"
#include "td5comm.h"

#define REMOTE_READ_ATTEMPTS             5
#define REMOTE_MAX_COMMAND_LENGTH        8

// Class Td5Remote
class Td5Remote
{
public:
  Td5Remote();
  void init();
  void btModuleOn();
  void btModuleOff();
  void connect();
  void disconnect();
  void sendData(byte* buffer, byte length = 1);
  boolean isConnected(){ return remoteConnected; };
  void aknCommand(boolean positiveAnswer = true);
  byte getCommand();

private:
  byte checksum(byte *data, byte len);
  byte recvCommand();
protected:
  boolean remoteConnected;
  byte commandBuffer[REMOTE_MAX_COMMAND_LENGTH];
  byte command;
};

#endif


#define REMOTE_NO_COMM          0x00
#define REMOTE_COMM_FAIL        0x01
#define REMOTE_CONNECT_OBD      0xB0
#define REMOTE_DISCONNECT_OBD   0xB1
#define REMOTE_CONNECT_BT       0xB2
#define REMOTE_DISCONNECT_BT    0xB3

#define REMOTE_INSTRUMENT_FAST  0xB4
#define REMOTE_INSTRUMENT_SLOW  0xB5
