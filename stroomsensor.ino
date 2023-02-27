
// ACS712 current sensor with ESP32 Microcontroller

#include <LiquidCrystal_I2C.h>
#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)

// WiFi
const char* ssid = "*********";                 // Your personal network SSID
const char* wifi_password = "*********"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.0.176";  // IP of the MQTT broker
const char* amps_topic = "homeassistant/sensor/stroomsensor/amps";
const char* watt_topic = "homeassistant/sensor/stroomsensor/watt";
const char* mqtt_username = "r****"; // MQTT username
const char* mqtt_password = "*******"; // MQTT password
const char* clientID = "client_stroomsensor"; // MQTT client ID

const int sensorIn = 16;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 100;           // this the 20A version of the ACS712 -use 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
 
// initialize the LCD library with I2C address and LCD size
LiquidCrystal_I2C lcd (0x27, 16,2); 

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}
 
void setup() {
  Serial.begin (9600); 
  Serial.println ("ACS712 current sensor"); 
  // Initialize the LCD connected 
lcd. init ();
// Turn on the backlight on LCD. 
lcd. backlight ();
lcd.print ("ACS712 current");
lcd. setCursor (0, 1);
lcd.print ("sensor");
delay(1000);
lcd.clear();  
}
 
void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);
  Serial.println (""); 
  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = ((VRMS * 1000)/mVperAmp)-3.43; //0.3 is the error I got for my sensor
  float h = Watt;
  float t = AmpsRMS; 

 // MQTT can only transmit strings
  String hs="Watt: "+String((float)h)+" W ";
  String ts="Amps: "+String((float)t)+" A ";  

  // PUBLISH to the MQTT Broker (topic = Amps, defined at the beginning)
  if (client.publish(amps_topic, String(t).c_str())) {
    Serial.println("Amps sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Amps failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(amps_topic, String(t).c_str());
  }
  // PUBLISH to the MQTT Broker (topic = Watt, defined at the beginning)
  if (client.publish(watt_topic, String(h).c_str())) {
    Serial.println("Watts sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Watts failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(watt_topic, String(h).c_str());
  }
  
  Serial.print(AmpsRMS);
  Serial.print(" Amps RMS  ---  ");
  Watt = (AmpsRMS*230/1.2);
  // note: 1.2 is my own empirically established calibration factor
// as the voltage measured at D34 depends on the length of the OUT-to-D34 wire
// 240 is the main AC power voltage – this parameter changes locally
  Serial.print(Watt);
  Serial.println(" Watts");
  lcd. setCursor (0, 0);
  lcd.print (AmpsRMS);
lcd.print (" Amps ");
//Here cursor is placed on first position (col: 0) of the second line (row: 1) 
lcd. setCursor (0, 1);
lcd.print (Watt);
lcd.print (" watt ");
delay (100);
//  client.disconnect();  // disconnect from the MQTT broker
  delay(1000*60);       // print new values every 1 Minute

}
 
// ***** function calls ******
float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 3.3)/4096.0; //ESP32 ADC resolution 4096
      
   return result;
 }
