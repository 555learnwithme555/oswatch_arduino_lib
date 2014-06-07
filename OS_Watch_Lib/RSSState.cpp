/*
Copyright 2014 DoNothingBox LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "RSSState.h"
#ifndef SSD1306_128_64//Always check Class files with Actual #define strings!
  #include <Adafruit_SSD1306.h>
#endif

#ifndef BASESTATE_H//Always check Class files with Actual #define strings!
  #include "BaseState.h"
#endif

static char rss0[20] = "Loading 0";
static char rss0Length = 0;
static char rss1[20] = "Loading 1";
static char rss1Length = 0;
static char rss2[20] = "Loading 2";
static char rss2Length = 0;
static char rss3[20] = "Loading 3";
static char rss3Length = 0;
static char rss4[20] = "Loading 4";
static char rss4Length = 0;
static char rss5[20] = "Loading 5";
static char rss5Length = 0;
static char rss6[20] = "Loading 6";
static char rss6Length = 0;
static char rss7[20] = "Loading 7";
static char rss7Length = 0;
static char rss8[20] = "Loading 8";
static char rss8Length = 0;
static char rss9[20] = "Loading 9";
static char rss9Length = 0;

static char* main_menu[10] = {
  " "," ", " ", " ", " ", " ", " ", " ", " ", " "};
static int main_menu_action[9] = {
  1,2, 3, 4, 5, 6, 7, 8, 9};
static int top_menu_id = 0;
static int hilight_menu_id = 0;

static byte RSS_STATE_APP_ID = 0x03;
static byte RSS_STATE_APP_ACTION_1 = 0x01;
static byte RSS_STATE_APP_ACTION_2 = 0x02;

static String completePhrase = "";
static int totalPackets = 0;
static int currentPacket = 0;
static int totalLines = 0;
static int currentLine = 0;


static int scrollPoint = 0;
static int frameClock = 0;

static int idleTime = 0;





RSSState::RSSState(Adafruit_SSD1306 *screen) : BaseState(_screen){
  STATE_ID = "RSSSTATE";
  _screen =  screen;
  setupMenu();
}

RSSState::~RSSState(){
  
}

void RSSState::setupMenu(){
  top_menu_id = 0;
  hilight_menu_id = 0;
}

void RSSState::render(){
  
  _screen->clearDisplay();
  _screen->setTextSize(1);
  switch (hilight_menu_id) {
  case 0:
    _screen->fillRect(0, 0, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, _screen->height()/3, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, (_screen->height()/3)*2, _screen->width(), _screen->height()/3, WHITE);
    break;
  case 1:
    _screen->drawRect(0, 0, _screen->width(), _screen->height()/3, WHITE);
    _screen->fillRect(0, _screen->height()/3, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, (_screen->height()/3)*2, _screen->width(), _screen->height()/3, WHITE);
    break;
  case 2:
    _screen->drawRect(0, 0, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, _screen->height()/3, _screen->width(), _screen->height()/3, WHITE);
    _screen->fillRect(0, (_screen->height()/3)*2, _screen->width(), _screen->height()/3, WHITE);
    break;
  } 

  if(hilight_menu_id == 0)
    _screen->setTextColor(BLACK);
  else
    _screen->setTextColor(WHITE);

  if(top_menu_id<=8)
  {
    _screen->setCursor(6,6);
    _screen->println(main_menu[top_menu_id]);
  }

  if(hilight_menu_id == 1)
    _screen->setTextColor(BLACK);
  else
    _screen->setTextColor(WHITE);

  if(top_menu_id<=7)
  {
    _screen->setCursor(6, (_screen->height()/3)+6);
    _screen->println(main_menu[top_menu_id+1]);
  }

  if(hilight_menu_id == 2)
    _screen->setTextColor(BLACK);
  else
    _screen->setTextColor(WHITE);

  if(top_menu_id<=6)
  {
    _screen->setCursor(6, ((_screen->height()/3)*2)+6);
    _screen->println(main_menu[top_menu_id+2]);
  }
  //Popup test
  /*
  _screen->fillRect(8, 8, _screen->width()-16, _screen->height()-16, BLACK);
  _screen->fillRect(10, 10, _screen->width()-20, _screen->height()-20, WHITE);
   _screen->setTextColor(BLACK);
  _screen->setCursor(12, 12);
  _screen->println("Error: Please");
  _screen->setCursor(12, 22);
  _screen->println("Connect BLE");*/
  
  _screen->display();
}

void RSSState::btnInterruptAction(boolean isDimmed){
      Serial.println("Btn intterupt requested");
      if(getDisplayDimStatus() == true){
        _screen->dim(false);
        setDisplayDimStatus(false);
      }
      else
      {
        Serial.println("Not dimmed, so change states");

        int currentMenuID = getSelectedMenuID();
        if(getSelectedMenuAction(currentMenuID) == 1)
        {
          
          char *stateID = "TIMESTATE";
          makeChangeRequest(stateID);
        } 
      }
}

void RSSState::btnDownAction(boolean isDimmed){
    if(getDisplayDimStatus() == true){
      _screen->dim(false);
      setDisplayDimStatus(false);
    }
    else
    {
      if(hilight_menu_id>=2)
          top_menu_id++;
      else
          hilight_menu_id++;
      _screen->clearDisplay();
      scrollPoint = 0;
      idleTime = 0;
      render();
    }
}

void RSSState::btnUpAction(boolean isDimmed){
      if(getDisplayDimStatus() == true){
        _screen->dim(false);
        setDisplayDimStatus(false);
      }
      else
      {
        if(hilight_menu_id<=0)
          top_menu_id--;
        else
          hilight_menu_id--;
        if(top_menu_id<=0)
          top_menu_id = 0;
        if(hilight_menu_id<=0)
          hilight_menu_id = 0;   
        _screen->clearDisplay();
        scrollPoint = 0;
        idleTime = 0;
        render();
      }
}

void RSSState::btnBackAction(boolean isDimmed){
      _screen->clearDisplay();
     char *stateID = "MENUSTATE";
     makeChangeRequest(stateID);
}

int RSSState::getSelectedMenuID(){
  return top_menu_id+hilight_menu_id;
}

int RSSState::getSelectedMenuAction(int id){
  return main_menu_action[id];
}

void RSSState::incomingMessageCallback(const struct ble_msg_attributes_value_evt_t *msg) {
 
  
      Serial.print("RSS MSG: ");
      Serial.println(msg -> value.data[1]);
      

    if(msg -> value.data[0] == 0x03)//RSS return
    {
       if(msg -> value.data[1] == 0x01)//Initial String message
      {
         currentLine = 1;
         totalLines = byteToInteger(msg -> value.data[2]) +1;
         Serial.println("::::: BEGINNING OF GROUP MESSAGE :::::");
         Serial.print("Total Lines Expected: ");
         Serial.println(totalLines);
         uint8_t length = (byte) 6;
         uint8_t data_packet[6] = {length,currentLine, 1,1,1,1};
         getBluetoothManager().transmitMessage(0x03, 0x02, data_packet);
      }
      
      else if(msg -> value.data[1] == 0x02)//Initial String message
      {
        totalPackets = byteToInteger(msg -> value.data[2]);
        Serial.println("::::: BEGINNING OF PACKET MESSAGE :::::");
        Serial.print("Total Packets Expected: ");
        Serial.println(totalPackets);
        //reset complete phrase
        completePhrase = "";
        currentPacket = 0;

        byte test = msg -> value.data[3];
        int incrementor = 3;
        while(test != NULL && incrementor<20){
            test = msg -> value.data[incrementor];
            completePhrase+=char(test);
            Serial.println(char(test));
            incrementor++;
        }
        currentPacket = 1;
        Serial.println(completePhrase);
        if(currentPacket<totalPackets){
            uint8_t length = (byte) 6;
            uint8_t data_packet[6] = {length,currentPacket, 1,1,1,1};
            getBluetoothManager().transmitMessage(0x03, 0x03, data_packet);
        }
        else
        {
            Serial.println("ERROR: Package only 1 packet long");          
        }

        
      }
      else if(msg -> value.data[1] == 0x03)//new packet
      {
        Serial.print("New Packet incoming: ");
        Serial.println(currentPacket);

        byte test = msg -> value.data[3];
        int incrementor = 3;
        while(test != NULL && incrementor<20){
            test = msg -> value.data[incrementor];
            completePhrase+=char(test);
            Serial.println(char(test));
            incrementor++;
        }
        currentPacket += 1;
        Serial.println(completePhrase);
        if(currentPacket<totalPackets){
 
            uint8_t length = (byte) 6;
            uint8_t data_packet[6] = {length,currentPacket, 1,1,1,1};
            getBluetoothManager().transmitMessage(0x03, 0x03, data_packet);
        }
        else
        {
            //Serial.println("");
  
  
            //completePhrase.toCharArray(main_menu[currentLine-1], 150);
            
  
            if(currentLine == 1)
            {
              completePhrase.toCharArray(rss0, 20);
              main_menu[0] = rss0;
            }
            
            else if(currentLine == 2)
            {
              completePhrase.toCharArray(rss1, 20);
              main_menu[1] = rss1;
            }
             else if(currentLine == 3)
            {
              completePhrase.toCharArray(rss2, 20);
              main_menu[2] = rss2;
            }   
             else if(currentLine == 4)
            {
              completePhrase.toCharArray(rss3, 20);
              main_menu[3] = rss3;
            } 
             else if(currentLine == 5)
            {
              completePhrase.toCharArray(rss4, 20);
              main_menu[4] = rss4;
            }   
            
            else if(currentLine == 6)
            {
              completePhrase.toCharArray(rss5, 20);
              main_menu[5] = rss5;
            }     
            else if(currentLine == 7)
            {
              completePhrase.toCharArray(rss6, 20);
              main_menu[6] = rss6;
            }
            else if(currentLine == 8)
            {
              completePhrase.toCharArray(rss7, 20);
              main_menu[7] = rss7;
            }          
             else if(currentLine == 9)
            {
              completePhrase.toCharArray(rss8, 20);
              main_menu[8] = rss8;
            }  
            else if(currentLine == 10)
            {
              completePhrase.toCharArray(rss9, 20);
              main_menu[9] = rss9;
            }      
            Serial.println("::::: END OF PACKET MESSAGE :::::");
            currentLine++;
            
            if(currentLine>=totalLines)
            {
                 Serial.println("::::: END OF ALL MESSAGES :::::");
                 render();
            }
            else
            {
                 uint8_t length = (byte) 6;
                 uint8_t data_packet[6] = {length,currentLine, 1,1,1,1};
                 getBluetoothManager().transmitMessage(0x03, 0x02, data_packet);
            }
        }
      }
      
      
      /*
      for(int i=2;i<= sizeof(msg -> value.data);i++)
      {

          Serial.print("str: ");
          Serial.println(msg -> value.data[i]);
      }*/
    }
    
    
  render();
  
}

void RSSState::updateDisplay(long lastUpdateTime){

  idleTime += lastUpdateTime;
  if(idleTime>1500){
    frameClock+=lastUpdateTime;
    if(frameClock>=(1000/30)){
      frameClock = 0;
      scrollPoint+=2;
    }
  }
  
  _screen->clearDisplay();
  _screen->setTextSize(1);
  switch (hilight_menu_id) {
  case 0:
    _screen->fillRect(0, 0, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, _screen->height()/3, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, (_screen->height()/3)*2, _screen->width(), _screen->height()/3, WHITE);
    break;
  case 1:
    _screen->drawRect(0, 0, _screen->width(), _screen->height()/3, WHITE);
    _screen->fillRect(0, _screen->height()/3, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, (_screen->height()/3)*2, _screen->width(), _screen->height()/3, WHITE);
    break;
  case 2:
    _screen->drawRect(0, 0, _screen->width(), _screen->height()/3, WHITE);
    _screen->drawRect(0, _screen->height()/3, _screen->width(), _screen->height()/3, WHITE);
    _screen->fillRect(0, (_screen->height()/3)*2, _screen->width(), _screen->height()/3, WHITE);
    break;
  } 
  
  int scrollOffset = 0;

  if(hilight_menu_id == 0)
    _screen->setTextColor(BLACK);
  else
    _screen->setTextColor(WHITE);

  if(top_menu_id<=8)
  {
    if(hilight_menu_id == 0)
          scrollOffset = scrollPoint;
    else
         scrollOffset = 0;
         
    _screen->setCursor(6-scrollOffset,6);
    _screen->println(main_menu[top_menu_id]);
  }

  if(hilight_menu_id == 1)
    _screen->setTextColor(BLACK);
  else
    _screen->setTextColor(WHITE);

  if(top_menu_id<=7)
  {
   if(hilight_menu_id == 1)
          scrollOffset = scrollPoint;
    else
         scrollOffset = 0;
    
    _screen->setCursor(6-scrollOffset, (_screen->height()/3)+6);
    _screen->println(main_menu[top_menu_id+1]);
  }

  if(hilight_menu_id == 2)
    _screen->setTextColor(BLACK);
  else
    _screen->setTextColor(WHITE);

  if(top_menu_id<=6)
  {
    
        if(hilight_menu_id == 2)
          scrollOffset = scrollPoint;
    else
         scrollOffset = 0;
    
    _screen->setCursor(6-scrollOffset, ((_screen->height()/3)*2)+6);
    _screen->println(main_menu[top_menu_id+2]);
  }
  //Popup test
  /*
  _screen->fillRect(8, 8, _screen->width()-16, _screen->height()-16, BLACK);
  _screen->fillRect(10, 10, _screen->width()-20, _screen->height()-20, WHITE);
   _screen->setTextColor(BLACK);
  _screen->setCursor(12, 12);
  _screen->println("Error: Please");
  _screen->setCursor(12, 22);
  _screen->println("Connect BLE");*/
  
  _screen->display();
  
  
}



void RSSState::sync(){
    if(getBluetoothManager().getState() == 4 || getBluetoothManager().getState() == 5)
  {
      uint8_t length = (byte) 6;
      uint8_t data_packet[6] = {length,100,100,100,100,254};
      getBluetoothManager().transmitMessage(RSS_STATE_APP_ID, RSS_STATE_APP_ACTION_1, data_packet);
  }
  else
  {
    _screen->clearDisplay();
    _screen->setTextSize(1);
    _screen->fillRect(8, 8, _screen->width()-16, _screen->height()-16, BLACK);
    _screen->fillRect(10, 10, _screen->width()-20, _screen->height()-20, WHITE);
    _screen->setTextColor(BLACK);
    _screen->setCursor(12, 12);
    _screen->println("Error: Please");
    _screen->setCursor(12, 22);
    _screen->println("Connect Phone");
    _screen->display();
  }
}
