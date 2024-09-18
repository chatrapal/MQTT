
#include "SIMCOM.h"
#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
int test=0;
String _buffer = "";
char gprs_Tx[200];
char gprs_Rx[100];
 unsigned char Hold_Time=0;
 unsigned char SendCmd=0;
 unsigned char cnt=0;
 unsigned char Analyse=0;
 unsigned char main_start;
 unsigned char PDP_activated;
 int index_place;
 unsigned char temp;
 unsigned char Error_cnt=0;

void SendAnyCommand(unsigned char *comm, unsigned char *response)
{
  sprintf(gprs_Tx,(const char*)comm);
  sprintf(gprs_Rx,(const char*)response);
  Transfer_Cmmd();
}
void Set_BaudRate()
{
  SoftwareSerial SIM(RX_PIN,TX_PIN);
  pinMode(OUTPUT,RESET_PIN);
  SIM.begin(9600);
  #if (LED) 
    pinMode(OUTPUT,LED_PIN);
  #endif
  _buffer.reserve(255); //reserve memory to prevent intern fragmention
  Serial.println("Baud Rate set");
}

void _readSerial()
{
  _buffer="";
  delay(80);
  unsigned char found=0;
  #if NODEMCU
    while(Serial.available() && found==0)
    {
      found=1;
      _buffer=Serial.readString();
    }
 #elif ARDUINO
    while (SIM.available() && found==0) 
    {
      found=1;
      _buffer=SIM.readString();
      Serial.println(_buffer); //For debugging   
    }
  #endif
   delay(20);
  if(found==1 && main_start==1)
  {
    int m=0;
    found=0;
    if(_buffer.indexOf(gprs_Rx)!=-1)
    {
      Analyse=1;
    }
    else if((_buffer.indexOf("ERR")!=-1)&&cnt!=3)
    {
      if(++Error_cnt >= 3)
      {
        Error_cnt=0;
        cnt=19;
      }
      SendCmd=0;
      Analyse=0;
    }
    else if((_buffer.indexOf("ERR")!=-1))
    {
      cnt=1;
      SendCmd=0;
      Analyse=0; 
    }
  }
}

void reset_module()
{
  #if (LED)
    digitalWrite(LED_PIN,HIGH);
  #endif 
  digitalWrite(RESET_PIN,LOW);
  delay(100);
  digitalWrite(RESET_PIN,HIGH);
  delay(1500);
  /// wait for the module response
  #if ARDUINO 
    Serial.print(F("AT send\r\n"));
    SIM.print(F("AT\r\n"));
  #elif NODEMCU
    Serial.print(F("AT\r\n"));
  #endif
  delay(150);
  _readSerial();
   int count=0;
  while (_buffer.indexOf("OK")==-1 ){
    #if ARDUINO 
      Serial.print("Module is not responding count:");
      Serial.println(count++);
      SIM.print(F("AT\r\n"));
    #elif NODEMCU
      Serial.print(F("AT\r\n"));
    #endif
    delay(150);
    _readSerial();
  }
   count=0;
  #if ARDUINO
    Serial.print(F("AT Response: OK\r\n"));
    SIM.print(F("ATE0\r\n"));
    Serial.print(F("ATE0\r\n"));
    delay(150);
    _readSerial();
    Serial.print(F("AT+IPR=9600\r\n")); 
    SIM.print(F("AT+IPR=9600\r\n")); 
    delay(150);
    _readSerial();
    Serial.print(F("AT&W\r\n")); 
    SIM.print(F("AT&W\r\n")); 
    delay(150);
    _readSerial();
  #elif NODEMCU
    Serial.print(F("ATE0\r\n"));
    delay(150);
    _readSerial();
    Serial.print(F("AT+IPR=9600\r\n")); 
    delay(150);
    _readSerial(); 
    Serial.print(F("AT&W\r\n")); 
    delay(150);
    _readSerial(); 
  #endif
  #if (LED)
    digitalWrite(LED_PIN,0);
  #endif 
}

void ATCheck()
{
  const __FlashStringHelper *command_tx = F("AT\r\n");
  const __FlashStringHelper *command_rx = F("OK");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
}
void Transfer_Cmmd()
{
  #if ARDUINO
     Serial.print(gprs_Tx);
     SIM.print (gprs_Tx);
  #elif NODEMCU
     Serial.print(gprs_Tx);
  #endif
}
void signalQuality()
{ 
  const __FlashStringHelper *command_tx = F("AT+CSQ\r\n");
  const __FlashStringHelper *command_rx = F("OK");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
}

void SimCardCheck()
{
   const __FlashStringHelper *command_tx = F("AT+CPIN?\r\n");
  const __FlashStringHelper *command_rx = F("READY");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print (gprs_Tx);

}

void GSM_Registration()
{
  const __FlashStringHelper *command_tx = F("AT+CREG?\r\n");
  const __FlashStringHelper *command_rx = F("+CREG:");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
  
}

void SetPDPContext(unsigned char *APN)
{
  const __FlashStringHelper *command_tx = F("AT+CGDCONT=1,\"IP\",\"%s\"\r\n");
  const __FlashStringHelper *command_rx = F("OK");
  sprintf_P(gprs_Tx, (const char *)command_tx,APN);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print (gprs_Tx);
}

void DeactiveGPRS_PDP()
{
  const __FlashStringHelper *command_tx = F("AT+CIPSHUT\r\n");
  const __FlashStringHelper *command_rx = F("SHUT OK");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
 // Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

void Check_GPRS()
{
  const __FlashStringHelper *command_tx = F("AT+CGATT?\r\n");
  const __FlashStringHelper *command_rx = F("+CGATT:");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

void Set_GPRS()
{
  const __FlashStringHelper *command_tx = F("AT+CGATT=1\r\n");
  const __FlashStringHelper *command_rx = F("OK");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}
void start_task(unsigned char *APN, unsigned char *usrname, unsigned char *pswd)
{
  const __FlashStringHelper *command_tx = F("AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n");
  const __FlashStringHelper *command_rx = F("OK");
  sprintf_P(gprs_Tx, (const char *)command_tx, APN,usrname,pswd);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

void BringToWireless()
{
  const __FlashStringHelper *command_tx = F("AT+CIICR\r\n");
  const __FlashStringHelper *command_rx = F("OK");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

void GET_localIP()
{
  const __FlashStringHelper *command_tx = F("AT+CIFSR\r\n");
  const __FlashStringHelper *command_rx = F(".");
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

void Send_Data(unsigned char *clientID,unsigned char *usrnme, unsigned char *pass, unsigned char *topic, unsigned char *Data)
{
    delay(2000);
    String temp="";
    unsigned char packet[500];
    int index_place=0;
    int i=0;
    unsigned char Connect_Field=0x10;
    unsigned char Connect_Field_len;
    unsigned char Publish_field=0x30;
    int Publish_field_len;
    unsigned char Pass_Len[2];
    unsigned char usrnme_len[2];
    unsigned char topic_len[2];
    unsigned char clientID_len[2];
    int Data_len=0;
    unsigned char Procol_Nme_Lvl[7]={0x00,0x04,'M','Q','T','T',0x04};
    
    unsigned char usenme_field=0x80; //1000 0000
    unsigned char pswd_field=0x40; // 0100 0000
    unsigned char Clean_Session=0x02; //0000 0010
    unsigned char connect_flag=0x00;
    unsigned char buff[4];
    unsigned char keepAlive[2]={0x00,0x3C};
    
    clientID_len[0] = 0x00;
    clientID_len[1] = strlen((const char*)clientID);
    
    Pass_Len[0] = 0x00;
    Pass_Len[1] = strlen((const char*)pass);
    
    usrnme_len[0] = 0x00;
    usrnme_len[1] = strlen((const char*)usrnme);
    
    topic_len[0]=0x00;
    topic_len[1]=strlen((const char*)topic);
    
    Data_len=strlen((const char*)Data);
    
    if(Pass_Len[1]==0 || usrnme_len[1]==0) //Make Connect flag;
    {
      connect_flag = Clean_Session; 
    }
    else
    {
      connect_flag = pswd_field | usenme_field |Clean_Session;
    }
    
    //The time to fill the everything in single packet;
    
    index_place=0;
    packet[index_place++]=Connect_Field;  //0
    
    if(Pass_Len[1]!=0x00 || usrnme_len[1]!=0x00)
    { 
      Connect_Field_len = 10 + 2 + clientID_len[1] + 2 + Pass_Len[1] + 2 + usrnme_len[1];
    }
    else
    {
      Connect_Field_len = 10 + 2 + clientID_len[1];
    }
    
    packet[index_place++]=Connect_Field_len;  //1
    
    
    memcpy(packet+index_place,Procol_Nme_Lvl,7);  //2-8
    index_place+=7;
    
    packet[index_place++]=connect_flag; //9
    
    packet[index_place++]=keepAlive[0];  //10
    packet[index_place++]=keepAlive[1]; //11
    //Serial.println("temp:   ");
    //Serial.write(packet[index_place-1]);
    
    packet[index_place++]=clientID_len[0]; //12
    packet[index_place++]=clientID_len[1];  //13
    
    memcpy(packet+index_place,clientID,clientID_len[1]);
    index_place+=clientID_len[1];
    
    
    if(Pass_Len[1]!=0x00 || usrnme_len[1]!=0x00)
    {
      packet[index_place++]=usrnme_len[0];
      packet[index_place++]=usrnme_len[1];
      memcpy(packet+index_place,usrnme,usrnme_len[1]);
      index_place+=usrnme_len[1];
    
      
      packet[index_place++]=Pass_Len[0];
      packet[index_place++]=Pass_Len[1];
      memcpy(packet+index_place,pass,Pass_Len[1]);
      index_place+=Pass_Len[1];
    
    }
    
    packet[index_place++]=Publish_field;
    
    Publish_field_len= 2+ topic_len[1] + Data_len;
    if(Publish_field_len>127)
    {
      int byte=mqtt_encode_length(buff,Publish_field_len);
      for(int i=0;i<byte;i++)
    //for(int i=byte-1;i>=0;i--)
    {
        packet[index_place++]=buff[i];
      }
  }
  else
  {
     packet[index_place++]=Publish_field_len;
  }
    
    packet[index_place++]=topic_len[0];
    packet[index_place++]=topic_len[1];
    
    memcpy(packet+index_place,topic,topic_len[1]);
    index_place+=topic_len[1];
    
 
    
    
    memcpy(packet+index_place,Data,Data_len);
    index_place+=Data_len;
    //packet[index_place++]=0x1A;
    #if ARDUINO
      i=0;
      while(i<index_place)
      {
        //Serial.print(" ");
        SIM.write(packet[i]);
        i++;
      }
      i=0;
      while(i<index_place)
      {
        Serial.write(packet[i]);
        i++;
      }
    #elif NODEMCU
      i=0;
      while(i<index_place)
      {
        Serial.write(packet[i]);
        i++;
      }
    #endif
    delay(5000);
    Analyse=1;

}

void start_Connection(unsigned char *domain_IP, int port)
{
  const __FlashStringHelper *command_tx = F("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n");
  const __FlashStringHelper *command_rx = F("CONNECT");
  sprintf_P(gprs_Tx, (const char *)command_tx, domain_IP,port);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);  
}

void  CloseConnection()
{
  const __FlashStringHelper *command_tx = F("AT+CIPCLOSE\r\n");
  const __FlashStringHelper *command_rx = F("CLOSE"); 
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

bool SwitchDataToCommMode()
{
  const __FlashStringHelper *command_tx = F("+++");
  const __FlashStringHelper *command_rx = F("OK"); //Successfull operation 
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

void Set_PDP()
{
  const __FlashStringHelper *command_tx = F("AT+CGACT=1\r\n");
  const __FlashStringHelper *command_rx = F("OK"); //Successfull operation 
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}


void  Check_PDP()
{
  const __FlashStringHelper *command_tx = F("AT+CGACT?\r\n");
  const __FlashStringHelper *command_rx = F("+CGACT:"); //Successfull operation 
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}

void PDP_Address()
{
  const __FlashStringHelper *command_tx = F("AT+CGPADDR=1\r\n");
  const __FlashStringHelper *command_rx = F("+CGPADDR: 1"); //Successfull operation 
  sprintf_P(gprs_Tx, (const char *)command_tx);
  sprintf_P(gprs_Rx, (const char *)command_rx);
  Transfer_Cmmd();
  //Serial.print(gprs_Tx);
  //SIM.print(gprs_Tx);
}


int mqtt_encode_length(unsigned char *buf, int len) {
    int bytes = 0;
    int MAX_LEN_BYTES=4;
    do {
        if (bytes + 1 > MAX_LEN_BYTES)
            return bytes;
        short d = len % 128;
        len /= 128;
        /* if there are more digits to encode, set the top bit of this digit */
        if (len > 0)
            d |= 128;
        buf[bytes++] = d;
    } while (len > 0);
    return bytes;
}
