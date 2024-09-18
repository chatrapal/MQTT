#include <string.h>
#include <Arduino.h>
using namespace std;

#define NODEMCU      1
#define ARDUINO      0
#define RX_PIN 10
#define TX_PIN 11
#define RESET_PIN 5
#define LED true
#define LED_PIN 13


//extern char Remote_IP_Domain[];
//extern int Remote_Port; 
//extern char srvr_pass[];
//extern char srvr_usrname[];
//extern char ClientID[];
//extern char topic[];
//extern char DataToPublish[];

extern char gprs_Tx[];
extern char gprs_Rx[];
extern String _buffer;
 extern unsigned char Hold_Time;
 extern unsigned char SendCmd;
 extern unsigned char cnt;
 extern unsigned char Analyse;
 extern unsigned char main_start;
 extern int index_place;
 extern unsigned char temp;
 extern unsigned char Error_cnt;
 extern unsigned char PDP_activated;
  void _readSerial();
  void ATCheck();
  void SendAnyCommand(unsigned char *comm,unsigned char *response);
  void Set_BaudRate(); 
  void reset_module();  
  void signalQuality(); 
  void SimCardCheck();
  void NetwrkSrchMode();
  void NetwrkModeSetTo_2G();
  void Operator_Selection();
  void GSM_Registration();
  void SetPDPContext(unsigned char *APN);
  void DeactiveGPRS_PDP();
  void Check_GPRS();
  void Set_GPRS();
  void Set_PDP();
  void start_task(unsigned char *APN, unsigned char *usrname, unsigned char *pswd);
  void BringToWireless();
  void GET_localIP();
  void start_Connection(unsigned char *domain_IP, int port);
  void start_Connection_Q(unsigned char *Remote_IP_Domain,int Remote_Port);
  void Send_Data(unsigned char *clientID,unsigned char *usrnme, unsigned char *pass, unsigned char *topic, unsigned char *Data);
  void DataToCmdMode();
  void Close_Connection();
  bool SendDataToSever();
  void CloseConnection();
  bool SwitchDataToCommMode();
  void Check_PDP();
  void PDP_Address();
  void Transfer_Cmmd();
  int mqtt_encode_length(unsigned char *buf, int len);
     
