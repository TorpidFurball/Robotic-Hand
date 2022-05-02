#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[]={0xC8,0xC9,0xA3,0xC9,0x39,0xCC};


//Flexreadings
#define flexPinzero 33
#define flexPinone 32
#define flexPintwo 35
#define flexPinthree 34
#define flexPinfour 39
#define flexPinfive 36

//readingsmoothing
int value;
int avgZero;
int avgOne;
int avgTwo;
int avgThree;
int avgFour;
int avgFive;
int x;

//values to be sent
float pressure=0;
float Wzero;
float Fone;
float Ftwo;
float Fthree;
float Ffour;
float Ffive;

//pressure system
float incomingpressure=0;
int motorPin=23;
String success;

typedef struct struct_message{
    float Wzero;
    float Fone;
    float Ftwo;
    float Fthree;
    float Ffour;
    float Ffive;
    float pressure;
} struct_message; 

struct_message MyGloveData;
struct_message TheirHandData;

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

void OnDataRecv(const uint8_t *mac,const uint8_t *MyGloveData, int len){
  memcpy(&TheirHandData, MyGloveData, sizeof(TheirHandData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingpressure=TheirHandData.pressure;

}

void setup(){
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
  Serial.println(digitalRead(motorPin));
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

  digitalWrite(motorPin, LOW); 
  if (incomingpressure>4.5){
    digitalWrite(motorPin, HIGH);
    MyGloveData.Wzero=180;
    MyGloveData.Fone=180;
    MyGloveData.Ftwo=180;
    MyGloveData.Fthree=0;
    MyGloveData.Ffour=0;
    MyGloveData.Ffive=0;
    delay(5000);
  }    

  getReadings();
  Serial.println(incomingpressure);
  Serial.println(digitalRead(motorPin));
  //valuesfromHand
  MyGloveData.Wzero=Wzero;
  MyGloveData.Fone=Fone;
  MyGloveData.Ftwo=Ftwo;
  MyGloveData.Fthree=Fthree;
  MyGloveData.Ffour=Ffour;
  MyGloveData.Ffive=Ffive; 

  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &MyGloveData, sizeof(MyGloveData));

  if (result ==ESP_OK){
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  //Serial.println("Finger Pressure=");
  //Serial.println(Wzero);
  //Serial.println(Fone);
  //Serial.println(Ftwo);
  //Serial.println(Fthree);
  //Serial.println(Ffour);
  //Serial.println(Ffive);

  delay(10);
}

void getReadings(){
  x=0;
  avgZero=0;
  avgOne=0;
  avgTwo=0;
  avgThree=0;
  avgFour=0;
  avgFive=0;
  while (x<15){
    value = analogRead(flexPinzero);      
    avgZero=value+avgZero;
    value = analogRead(flexPinone);
    avgOne=value+avgOne;
    value = analogRead(flexPintwo);
    avgTwo=value+avgTwo;
    value = analogRead(flexPinthree);
    avgThree=value+avgThree;
    value = analogRead(flexPinfour);
    avgFour=value+avgFour;
    value = analogRead(flexPinfive);
    avgFive=value+avgFive;
    x++;
  } 

  Wzero = map(avgZero/x, 1500, 2000, 90,180);
  Fone = map(avgOne/x,1700,2700,40,168);
  Ftwo = map(avgTwo/x,1200,2350,50,168);
  Fthree = map(avgThree/x,1850,2700,150,50);
  Ffour = map(avgFour/x,1375,2465,180,39);
  Ffive = map(avgFive/x,1500,2550,120,26);
  if (Wzero<0) {
    Wzero=0;
  }
  if (Wzero>180){
    Wzero=180;
  }
  if (Fone<0) {
    Fone=0;
  }
  if (Fone>180){
    Fone=180;
  }
  if (Ftwo<0) {
    Ftwo=0;
  }
  if (Ftwo>180){
    Ftwo=180;
  }
  if (Fthree<0) {
    Fthree=0;
  }
  if (Fthree>180){
    Fthree=180;
  }
  if (Ffour<0) {
    Ffour=0;
  }
  if (Ffour>180){
    Ffour=180;
  }
  if (Ffive<0) {
    Ffive=0;
  }
  if (Ffive>180){
    Ffive=180;
  }
  pressure=0;


}
