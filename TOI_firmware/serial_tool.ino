/*
 * This file is part of TOI_firmware.
 *
 * Copyright (C) 2015  D.Herrendoerfer
 *
 *   TOI_firmware is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   TOI_firmware is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with TOI_firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

/*Uncomment this to see all serial traffic*/ 
//#define SER_DEBUG 1
/*Uncomment this to see serial errors*/ 
//#define SER_ERROR 1

int find(char* str, int timeout)
{
  unsigned long end = millis() + timeout;
  int sp = 0;
  char tmp;
  
  while ( (long)(millis() - end ) < 0) {
    if (Serial1.available() > 0){
      tmp = Serial1.read();
      
#ifdef SER_DEBUG
      Serial.print(tmp);
#endif      
      if (str[sp] == tmp){
        sp++;
        if (sp == strlen(str)){
          return 1;
        }
      } else {
        sp=0;
      }
    }
  }
  return 0;
}

int expect(char* expectstr, int timeout) 
{
//  Serial1.setTimeout(timeout); 
  if(find(expectstr,5000)){
#ifdef SER_DEBUG
    Serial.print("RECEIVED: ");
    Serial.println(expectstr);
#endif
    return 0;
  } 

#if defined SER_DEBUG || SER_ERROR
  Serial.print("ERROR DID NOT RECEIVE: ");
  Serial.println(expectstr);
#endif

  /*Check if the ESP8266 is stuck */
  Serial1.print("AT\r\n");
  if (!find("OK\r\n",2000)){
#if defined SER_DEBUG || SER_ERROR
    Serial.println("ERROR, ESP STUCK: ");
#endif
    Serial1.print("\r\nAT\r\n");
    if (!find("OK\r\n",2000)){
#if defined SER_DEBUG || SER_ERROR
      Serial.println("ERROR, ESP still STUCK: ");
#endif
    } else {
      reinit = 1;
#if defined SER_DEBUG || SER_ERROR
      Serial.println("RECOVER, ESP DID RESPOND. ");
#endif
    }
  }

  return 1;
}

int send_expect(char* sendstr, char* expectstr, int timeout) 
{
  Serial1.println(sendstr);
#ifdef SER_DEBUG
  Serial.print("Sent: ");
  Serial.println(sendstr);  
#endif
  
  return expect(expectstr,timeout);
}

int send_expect(String sendstr, char* expectstr, int timeout) 
{
  Serial1.println(sendstr);
#ifdef SER_DEBUG
  Serial.print("Sent: ");
  Serial.println(sendstr);  
#endif
  
  return expect(expectstr, timeout);
}

void send(char* sendstr) 
{
  Serial1.print(sendstr);
#ifdef SER_DEBUG
  Serial.print("Sent: ");
  Serial.println(sendstr);  
#endif
}

void send(int val) 
{
  char sendstr[6];
  sprintf(sendstr,"%i",val);
  Serial1.print(sendstr);
#ifdef SER_DEBUG
  Serial.print("Sent: ");
  Serial.println(sendstr);  
#endif
}


void serial_flush()
{
  while(Serial1.available()>0) {
    Serial1.read();
    delay(10);
  }
}

int send_dump(char* sendstr)
{
  char tmp;
  
  Serial1.println(sendstr);
  Serial.println(F("--------------- DUMP ---------------"));
  Serial.print(F("Sent: "));
  Serial.println(sendstr);

  while (tmp != '\n') {
    if (Serial1.available()>0) {
      tmp=Serial1.read();
      Serial.write(tmp);
    }
  }
  tmp=0;
  while (tmp != '\n') {
    if (Serial1.available()>0) {
      tmp=Serial1.read();
      Serial.write(tmp);
    }
  }
  tmp=0;
  while (tmp != '\n') {
    if (Serial1.available()>0) {
      tmp=Serial1.read();
      Serial.write(tmp);
    }
  }
  Serial.println(F("--------------- DUMP ---------------"));
  serial_flush();
}

int serial_read(char* buffer, int offset, int length)
{
  int count=0;
  
//  if (Serial1.available() == 0)
//    return 0;
  
  while(Serial1.available() > 0) {
    buffer[offset]=Serial1.read();
//    Serial.write(buffer[offset]);
    buffer[offset+1]=0; // HACK !!!
    count++;
    /* Kind of a neat hack to keep mem consumption low:
     * bail out at the end of each line, because we may
     * safely ignore a lot of things                     */
    if (buffer[offset] == '\n')
      break;
    offset++;
    if (offset == length)
      break;
  }

  return count;
}
