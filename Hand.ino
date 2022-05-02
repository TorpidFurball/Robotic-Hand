#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

uint8_t broadcastAddress[]={0x30,0xC6,0xF7,0x23,0x86,0xE0};

Servo myservozero;
Servo myservoone;
Servo myservotwo;
Servo myservothree;
Servo myservofour;
Servo myservofive;

//Servo pin definition
#define ServoPinZero 14
#define ServoPinOne 12
#define ServoPinTwo 13
#define ServoPinThree 32
#define ServoPinFour 33
#define ServoPinFive 27

//values to be sent
float pressure;
float Wzero;
float Fone;
float Ftwo;
float Fthree;
float Ffour;
float Ffive;

//incoming vars
float incomingWzero=180;
float incomingFone=180;
float incomingFtwo=180;
float incomingFthree=0;
float incomingFfour=0;
float incomingFfive=0;

//Pressure Sensor reading
int pressurePin=35;
float involt=5.0;
float rtwo=1000;
float rone;
float x;
float vout;


String success;

typedef struct struct_message {
    float Wzero;
    float Fone;
    float Ftwo;
    float Fthree;
    float Ffour;
    float Ffive;
    float pressure;
} struct_message; 

//typedef struct struct_messageOne{
//  float F1;
//  float F2;
//  float F3;
//
//} struct_messageOne;
//typedef struct struct_messageTwo{
//  float pressure
//} struct_messageTwo;

struct_message TheirGloveData;
struct_message MyHandData;

void OnDataSent( const uint8_t *mac_addr,esp_now_send_status_t status){
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status==ESP_NOW_SEND_SUCCESS ? "Delivery Success": "Delivery Fail");
  if (status==0){
    success="Delivery Success";
  }
  else{
    success="Delivery Fail";
  }
}

void OnDataRecv(const uint8_t *mac,const uint8_t *MyHandData, int len){
  memcpy(&TheirGloveData, MyHandData, sizeof(TheirGloveData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingWzero = TheirGloveData.Wzero;
  incomingFone = TheirGloveData.Fone;
  incomingFtwo = TheirGloveData.Ftwo;
  incomingFthree = TheirGloveData.Fthree;
  incomingFfour = TheirGloveData.Ffour;
  incomingFfive = TheirGloveData.Ffive;
}

void setup(){
  Serial.begin(115200);
  pinMode(pressurePin, INPUT);
  myservozero.attach(ServoPinZero);
  myservoone.attach(ServoPinOne);
  myservotwo.attach(ServoPinTwo);
  myservothree.attach(ServoPinThree);
  myservofour.attach(ServoPinFour);
  myservofive.attach(ServoPinFive);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel=0;
  peerInfo.encrypt=false;

  if (esp_now_add_peer(&peerInfo) !=ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
  getReadings();
 //valuesfromHand
  MyHandData.pressure=pressure;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &MyHandData, sizeof(MyHandData));

  if (result ==ESP_OK){
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  //finger 1
  myservozero.write(incomingWzero);
  myservoone.write(incomingFone);
  myservotwo.write(incomingFtwo);
  myservothree.write(incomingFthree);
  myservofour.write(incomingFfour);
  myservofive.write(incomingFfive);

  delay(10);
}

void getReadings(){
  x=analogRead(pressurePin);
  vout=x*involt/4096;
  //rone=(involt*rtwo/vout)-rtwo;
  Serial.println(vout);
  pressure=vout;
  Wzero=0;
  Fone=0;
  Ftwo=0;
  Fthree=0;
  Ffour=0;
  Ffive=0;
}
