#include "SIMCOM.h" 
#include <SoftwareSerial.h>
//#include <string> 
//using namespace std;
long int TimetosndNewPacket_in_msec=0;
unsigned char apn[] = "airtelgprs.com";
unsigned char apn_usrnme[]="";
unsigned char apn_pswd[]="";
//unsigned char Remote_IP_Domain[]="";
unsigned char Remote_IP_Domain[]="io.adafruit.com";
 int Remote_Port=1883; 
unsigned char srvr_pass[] = "";   //depends on server
 unsigned char srvr_usrname[]= "";   //depends on server 
 unsigned char ClientID[]="";   //It is compulsory
 unsigned char topic[]="";   //It is compulsory
 unsigned char DataToPublish[]="{'result': {'ip':'192.168.29.194','id':'T9999','Date':'2022-11-14','Time':'18:12:14','Temp':39.51,'VOC':329.00,'CO':3.03,'CO2':1111.00,'O3':0.00,'RH':25.63,'PM1':103,'PM25':167,'O3':182}}";    ////It is compulsory
 long int TimetosndNewPacket_in_sec=30;
 long int MillisTime=0;

unsigned char stage=0;
unsigned char count=0;
unsigned char setcmdcnt=0;
unsigned char setcommand=0;
unsigned char comeagain=0;
unsigned char Makeconn=0;
unsigned char DataToComm=0;
unsigned char init_step=0;
long int init_time=0;


void setup() {
  Serial.begin(9600);
  Serial.println("Initiallizing...and wait for 5 sec");
  delay(5000);
  #if ARDUINO
    Set_BaudRate();
  #endif
  reset_module();
  SendAnyCommand((unsigned char*)"AT+CREG=0\r\n", (unsigned char*)"OK");
  delay(500);
  _readSerial();
  SendCmd=0;
  cnt=1;
  main_start=1;
 TimetosndNewPacket_in_msec=TimetosndNewPacket_in_sec*1000;
}

void loop() {
  if(SendCmd==0)
  { 
    SendCmd=1;
    switch(cnt)
    {
      case 1:
        delay(1000);
        ATCheck();
        break;
      
      case 2:
        SendAnyCommand((unsigned char*)"ATE0\r\n" , (unsigned char*)"OK");  
        break;
  
      case 3:
        SimCardCheck();  
        break;
  
      case 4:
        PDP_activated=1;
        SetPDPContext(apn); 
        break;
  
      case 5:
        GSM_Registration();  
        break;
  
      case 6:
        Check_GPRS();
        break;
  
      case 7:
        Set_GPRS();  
        break;
  
      case 8:
        Check_PDP();
        break;
  
      case 9:
        Set_PDP();  
        break;
  
      case 10:
        SendAnyCommand((unsigned char*)"AT+CIPMODE=1\r\n" ,  (unsigned char*)"OK"); 
        break;
      
      case 11:
        SendAnyCommand((unsigned char*)"AT+CIPCCFG=3,2,1024,0\r\n" , (unsigned char*)"OK");
        break;
       
      case 12:
        start_task(apn,apn_usrnme,apn_pswd);
        break;
       
      case 13:
        BringToWireless();
        break;
       
      case 14:
        GET_localIP(); 
        break;
       
      case 15:
        start_Connection(Remote_IP_Domain,Remote_Port);
        break;

      case 16:
        Send_Data(ClientID , srvr_usrname, srvr_pass ,topic, DataToPublish);   
        break;
       
      case 17:
        SwitchDataToCommMode();
        break;
      
      case 18:
        CloseConnection();
        break;
        
      case 19:
        SendAnyCommand((unsigned char*)"AT+CFUN=1,1\r\n" , (unsigned char*)"OK");
        break;
        
      case 20:
        DeactiveGPRS_PDP();
        break;
      
      default:
        cnt=1;
        SendCmd=0;
        break; 
    }
  }
  else if(SendCmd==1 && Analyse==1)
  {
    Analyse=0;
    SendCmd=0;
    Error_cnt=0;
    if(cnt==1 ||cnt==2|| cnt==4 || cnt==7 || cnt==9 || cnt==10 ||cnt==11 || cnt==12 || cnt==13 || cnt==14 || cnt==15 || cnt==17) 
    {
      cnt++;
    }
    else if(cnt==3)
    {
      if((_buffer.indexOf("+CPIN: READY"))==-1)
      {
        cnt=1;
      }
      else if(PDP_activated==1)
      {
        cnt+=2;
      }
      else
      {
        cnt++;
      }
    }
    else if(cnt==5)
    {
      index_place=_buffer.indexOf("+CREG: 0,1");
      index_place=index_place+9;
      if((_buffer[index_place]=='1') || (_buffer[index_place]=='5'))
      {
        cnt++;
      }
      else
      {
        cnt=1;
      }
    }
    else if(cnt==6)
    {
      if((_buffer.indexOf("+CGATT: 1"))!=-1)
      {
        cnt +=2;
      }
      else
      {cnt++;}
    }
    else if(cnt==8)
    {
      index_place=_buffer.indexOf("+CGACT: ");
      index_place=index_place+10;
      if(_buffer[index_place]=='0') 
      {cnt++;}
      else 
      {cnt+=2;}
    }
    else if(cnt==16)
    {
      if(_buffer[0]==0x20 && _buffer[1]==0x02 && _buffer[2]==0x00 && _buffer[3]==0x00)
      {
        cnt++;
      }
      else
      {
        cnt+=2;
      }
    }
    else if(cnt==18)
    {
      cnt+=2;
    }
    else if(cnt==19)
    {
      Serial.println("Full functionality");
      PDP_activated=0;
      cnt=1;
      delay(10000);
    } 
    else if(cnt==20)
    {
      cnt=1;
    }
    else
    {
      cnt=1;
    }
  }

  if((_buffer.indexOf("CLOSE"))!=-1)
  {
    _buffer="";
    cnt=20;
    SendCmd=0;
    Analyse=0;
  }
if(SendCmd==1 && Analyse==0)
{
  _readSerial();
}

  MillisTime=millis();  //2000
  if(SendCmd==1 && Analyse==0)
  {
    if(init_step==0)
    {
      init_step=1;
      init_time=MillisTime;
    } 
    //init_time+=MillisTime;
    if(((MillisTime)>=(init_time+30000))&&(init_step==1))
    {
       cnt=1;
       init_step=0;
       SendCmd=0; 
       Analyse=0;
    }
  }
  else
  {
    init_step=0;
  }
 
}
