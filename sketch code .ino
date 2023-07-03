https://wokwi.com/projects/368137328935526401


  //SMARTINTERNZ PROJECT
//SMART PARKING SYSTEM
#include <WiFi.h>//library for wifi
#include <PubSubClient.h>//library for MQtt 

//ESP32 SERVO LIBRARY
#include <ESP32Servo.h>

#define LED 2

void callback(char* subscribetopic, byte* payload, unsigned int payloadLength); 

//-------credentials of IBM Accounts------

#define ORG "3v6pvz"//IBM ORGANITION ID
#define DEVICE_TYPE "sps01"//Device type mentioned in ibm watson IOT Platform
#define DEVICE_ID "7845"//Device ID mentioned in ibm watson IOT Platform
#define TOKEN "12345678"     //Token
String data3;

//-------- Customise the above values --------
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";// Server Name
char publishTopic[] = "iot-2/evt/Data/fmt/json";// topic name and type of event perform and format in which data to be send
char subscribetopic[] = "iot-2/cmd/command/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";// authentication method
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;//client id

//-----------------------------------------
WiFiClient wifiClient; // creating the instance for wificlient
PubSubClient client(server, 1883, callback ,wifiClient); //calling the predefined client id by passing parameter like server id,portand wificredential

Servo s;

const int totalSlots = 2;

//VARIABLES
int lr = 12;
int lg = 14;

int tg = 19;
int eg = 21;

int availableSlots = totalSlots;

void setup() {
  Serial.begin(115200);
  s.attach(4);
  
  pinMode(lr,OUTPUT);
  pinMode(lg,OUTPUT);
  pinMode(tg,OUTPUT);
  pinMode(eg, INPUT);
  pinMode(15,OUTPUT);
  pinMode(2, INPUT);
  pinMode(27, OUTPUT);
  pinMode(26, INPUT);

  wificonnect();
  mqttconnect();
}

void loop() {
       
  digitalWrite(tg,LOW);
  digitalWrite(tg,HIGH);
  delayMicroseconds(10);
  digitalWrite(tg,LOW);
  

  float dur = pulseIn(eg,HIGH);
  float dis = (dur*0.0343)/2;

  if(dis < 200 ){
    s.write(90);
    digitalWrite(lr,LOW);
    digitalWrite(lg,HIGH);
    delay(5000);
  }

  else{
    s.write(0);
    digitalWrite(lg,LOW);
    digitalWrite(lr,HIGH);
    
  }


  digitalWrite(15,LOW);
  digitalWrite(15,HIGH);
  delayMicroseconds(10);
  digitalWrite(15,LOW);

  float dur1 = pulseIn(2,HIGH);
  float dis1 = (dur1*0.0343)/2;

  int slot1, slot2;

  int totalSlots;

  if(dis1 < 200){
    slot1 = 0;
  }

  else{
    slot1 = 1;
  }

  digitalWrite(27,LOW);
  digitalWrite(27,HIGH);
  delayMicroseconds(10);
  digitalWrite(27,LOW);

  float dur2 = pulseIn(26,HIGH);
  float dis2 = (dur2*0.0343)/2;

  if(dis2 < 200){
    slot2 = 0;
  }

  else{
    slot2 = 1;
  }
 
  totalSlots = slot1 + slot2;

  PublishData(totalSlots);
  delay(4000);
  if (!client.loop()) {
    mqttconnect();
  }
  

}

/*.....................................retrieving to Cloud...............................*/

void PublishData(int slots) {
  mqttconnect();//function call for connecting to ibm
  /*
     creating the String in in form JSon to update the data to ibm cloud
  */
  String payload = "{\"slots\":";
  payload += slots;
  payload += "}";

  
  Serial.print("Sending payload: ");
  Serial.println(payload);

  
  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish ok");// if it sucessfully upload data on the cloud then it will print publish ok in Serial monitor or else it will print publish failed
  } else {
    Serial.println("Publish failed");
  }
  
}

void mqttconnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
      
     initManagedDevice();
     Serial.println();
  }
}
void wificonnect() //function defination for wificonnect
{
  Serial.println();
  Serial.print("Connecting to ");

  WiFi.begin("Wokwi-GUEST", "", 6);//passing the wifi credentials to establish the connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initManagedDevice() {
  if (client.subscribe(subscribetopic)) {
    Serial.println((subscribetopic));
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* subscribetopic, byte* payload, unsigned int payloadLength) 
{
  
  Serial.print("callback invoked for topic: ");
  Serial.println(subscribetopic);
  for (int i = 0; i < payloadLength; i++) {
    //Serial.print((char)payload[i]);
    data3 += (char)payload[i];
  } 
  Serial.println("data: "+ data3); 
  if(data3=="lighton")
  {
Serial.println(data3);
digitalWrite(LED,HIGH);
  }
  else if(data3=="{\"command\":\"lighton\"}")
  {
Serial.println(data3);
digitalWrite(LED,HIGH);
  }
  else 
  {
Serial.println(data3);
digitalWrite(LED,LOW);
  }
data3="";  
}