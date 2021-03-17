#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

char* wifi_ssid = "surajHotSpot";
char* wifi_pwd = "onem2mlab";
String cse_ip = "192.168.43.59";
////////////////////////////////////////
//Sensor Setup
//------------------------------------//

#include <DHT.h>
uint8_t DHTPIN = 4;
//our sensor is DHT11 type
#define DHTTYPE DHT11
//create an instance of DHT sensor
DHT dht(DHTPIN, DHTTYPE);

//------------------------------------//

////////////////////////////////////////

StaticJsonBuffer<200> jsonBuffer;
JsonObject& user_data = jsonBuffer.createObject();
JsonObject& temp_user_data = jsonBuffer.createObject();
JsonObject& sensor_data = jsonBuffer.createObject();

String cse_port = "8080";
String server = "http://"+cse_ip+":"+cse_port+"/~/in-cse/in-name/";

String createCI(String server, String ae, String cnt, String val)
{
  HTTPClient http;
  http.begin(server + ae + "/" + cnt + "/");
//  String postRequest = String() + "POST " + url + " HTTP/1.1\r\n" +
//                       "Host: " + CSE_IP + ":" + CSE_HTTP_PORT + "\r\n" +
//                       "X-M2M-Origin: " + CSE_M2M_ORIGIN + "\r\n" +
//                       "Content-Type: application/json;ty=" + ty + "\r\n" +
//                       "Content-Length: " + rep.length() + "\r\n"
//                       "Connection: close\r\n\n" +
//                       rep;
//                       
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "100");
  http.addHeader("Connection", "close");
  int code = http.POST("{\"m2m:cin\": {\"cnf\": \"text/plain:0\",\"con\": "+ String(val) +"}}");
  http.end();
  Serial.print(code);
  delay(300);
}

void setup()
{
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(wifi_ssid, wifi_pwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  //LED ON CODE INDICATING CONN TO WIFI

  //call begin to start DHT sensor
  pinMode(DHTPIN, INPUT);
  dht.begin();

  Serial.println("Setup done");
}

void loop()
{
  //Serial.print(n);
  //Serial.println(" networks found");

  //Take DHT sensor state
  //float t = 0.2;
  float t = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("");
  //float h = 0.1;
  float h = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println("");

  //user_data.printTo(Serial);


  //To GET values
  //http.begin("http://192.168.0.113:8080/~/in-cse/in-name/verte001/DATA/la");
  //http.begin("http://192.168.0.113:8080/~/in-cse");
  //int httpCode = http.GET();

  t = 27;
  h = 24;

  // when single sensor gives multiple values
  String sensor1 = "(" + String(t) + "," + String(h) + ")"; 

  //when single sensor gives single value
  String sensor2 = String(10); 

  // Make it a single string
  String sensor_string = sensor1 + "," + sensor2;

  // Make it OneM2M complaint
  sensor_string = "\"" + sensor_string + "\""; // DO NOT CHANGE THIS LINE

  // Send data to OneM2M server
  createCI(server, "Team0_abc", "node_1", sensor_string);
  delay(15000); // DO NOT CHANGE THIS LINE
}
