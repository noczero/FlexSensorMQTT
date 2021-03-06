
/**
Bismillah
---------
Test Flex Sensor
- Hardware :
Wemos D1 Mini
Resistor 37K between A0 and Ground
- Library : 
ESP8266WiFi - WiFi Connection
PubSubClient - MQTT
- To Do :
Read Analog PIN (done)
Convert to Degree (done)
Configure WiFi (done)
Send to MQTT (done)
*/

// ESP Library
#include <ESP8266WiFi.h>
// MQTT Library
#include <PubSubClient.h>

#define flexPIN A0
#define LED_BUILTIN 2

const float VCC = 4.98; // Wemos D1 Voltage
const float Resistor_Divider = 36500.0; // ohm resistance
const float STRAIGHT_RES = 25000.00; // got from flexR when flex is straight
const float BEND_RES = 90000.00; // got from flexR when flex is bend

// Setup WiFI and Broker
// Wifi Connection
const char* wifiSSID = "GARUDA JUARA";
const char* wifiPassword = "bismillahamin";
//#define wifiSSID "ZeroInside02"
//#define wifiPassword "testingbray"
  
// MQTT Define
// const char* mqttServerIP = "192.168.43.9";
const char* mqttServerIP = "192.168.43.2";
//#define mqttServerIP "192.168.137.1"
const int mqttPort = 1883;
//#define mqttPort "1883"

//char* device1= "flex/degree"; // pub & syv
#define device1 "flex/degree"
#define topic_ADC "flex/adc"
#define topic_V "flex/volt"
#define topic_R "flex/ohm"

WiFiClient myESP; // myESP become WIFI
PubSubClient client(myESP);


void wifiSetup(){
  WiFi.begin(wifiSSID,wifiPassword);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Waiting, connection to Wifi..");
    Serial.print("SSID : "); Serial.println(wifiSSID);
    
    // give notification LED
    // notifLED(LED2,200);
  }
  Serial.println("Connected to the WiFI Network "); 
  Serial.print("Connected Network "); Serial.println(wifiSSID);
  Serial.print("IP Local "); Serial.println(WiFi.localIP());
}


char dataPublish[50];
void publishMQTT(char* topics, String data){
   
   data.toCharArray(dataPublish, data.length() + 1);
    
   client.publish(topics, dataPublish);
}

void reconnect(){
  // MQTT Begin
  while(!client.connected()){
    Serial.println("Connecting to MQTT Server..");
    Serial.print("IP MQTT Server : "); Serial.println(mqttServerIP);
    //notifLED(LED3,300);
    bool hasConnection = client.connect("ZeroESP-1"); // connect(id,username,password) -> true if connect
    if(hasConnection){
      Serial.println("Success connected to MQTT Broker");
    } else {
      Serial.print("Failed connected");
      Serial.println(client.state());
      delay(2000);
      Serial.println("Try to connect...");
    }
  }
  //client.publish(device1, "Reconnecting"); // acc
  //client.subscribe(device1);  

}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.println("--------");
  Serial.println("Message Arrived");
  Serial.print("Topic :"); Serial.println(topic);
  Serial.print("Message : ");
  String pesan = "";
  for(int i=0; i < length; i++){
    Serial.print((char)payload[i]);

    pesan += (char)payload[i];

  }
  Serial.println();

  Serial.print("Pesan masuk :");
  Serial.println(pesan);
  Serial.println("--------");
}

int flexADC = 0;
float flexV = 0.0 , flexR = 0.0 , angle = 0.0;
float readingFlex(){
  flexADC = analogRead(flexPIN);
  flexV = flexADC * VCC / 1023.0;
  flexR = Resistor_Divider * (VCC / flexV - 1.0);
  //Serial.println("Flex Res : " + String(flexR));
  // map value to degrees
  float angles = map(flexR, STRAIGHT_RES , BEND_RES,0 , 90.0);

  // filter min value
  //Serial.println("RAW Angle : " + String(angle)); 
  if(angles >= 0 && angles < 90) {
    return angles;
  } else if (angles >= 90) {
    return 90.0;
  } else {
    return 0;
  }
}

void blink_led(){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("initialised");
  pinMode(flexPIN , INPUT); // set flexPIN to input mode
  
  wifiSetup(); // configure wifi

  Serial.print(F("WiFi connected! IP address: "));
  
  //Initialize MQTT Connection
  client.setServer(mqttServerIP, mqttPort);
  //client.setCallback(callback); // callback for incoming message
  pinMode(LED_BUILTIN, OUTPUT); // led

}
float prev_angle = 0.0;
void loop() {
  // put your main code here, to run repeatedly:

  if(!client.connected()){
    reconnect();
    digitalWrite(LED_BUILTIN, HIGH);
  }

  Serial.println("Degree - " + String(readingFlex()));
  client.loop(); //looping forever
  
  angle = readingFlex();
  if(prev_angle != angle){
    publishMQTT(device1,String(angle)); // publish value
  
    if (angle >= 0 && angle <= 90){
      publishMQTT(topic_ADC,String(flexADC));
      publishMQTT(topic_V , String(flexV));
      publishMQTT(topic_R , String(flexR));
    }
    prev_angle = angle;
  }
  blink_led();

  //delay(240);
}
