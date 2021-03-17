#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
char* wifi_ssid = "JioFi3_127A1C";
char* wifi_pwd = "0rn36u30sm";
String cse_ip = "onem2m.iiit.ac.in";
#define LED_BUILTIN 2
#define SENSOR 27
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 8;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
String cse_port = "443";
String server = "http://"+cse_ip+":"+cse_port+"/~/in-cse/in-name/";
String createCI(String server, String ae, String cnt, String val)
{
HTTPClient http;
http.begin(server + ae + "/" + cnt + "/");
http.addHeader("X-M2M-Origin", "admin:admin");
http.addHeader("Content-Type", "application/json;ty=4");
http.addHeader("Content-Length", "100");
http.addHeader("Connection", "close");
int code = http.POST("{\"m2m:cin\": {\"cnf\": \"text/plain:0\",\"con\": "+ String(val) +"}}");
http.end();
Serial.print(code);
delay(60000);
}
void IRAM_ATTR pulseCounter()
{
pulseCount++;
}
void check(){
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.println("Connecting to WiFi..");
}
}
void setup()
{
Serial.begin(115200);
pinMode(LED_BUILTIN, OUTPUT);
pinMode(SENSOR, INPUT_PULLUP);
pulseCount = 0;
flowRate = 0.0;
flowMilliLitres = 0;
totalMilliLitres = 0;
previousMillis = 0;
Serial.println("Setup done");
attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
sei();
// Set WiFi to station mode and disconnect from an AP if it was previously connected
WiFi.mode(WIFI_STA);
WiFi.begin(wifi_ssid, wifi_pwd);
check();
}
void loop()
{
check();
currentMillis = millis();
if (currentMillis - previousMillis > interval && WiFi.status() == WL_CONNECTED) {
pulse1Sec = pulseCount;
pulseCount = 0;
// Because this loop may not complete in exactly 1 second intervals we calculate
// the number of milliseconds that have passed since the last execution and use
// that to scale the output. We also apply the calibrationFactor to scale the output
// based on the number of pulses per second per units of measure (litres/minute in
// this case) coming from the sensor.
flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
previousMillis = millis();
// Divide the flow rate in litres/minute by 60 to determine how many litres have
// passed through the sensor in this 1 second interval, then multiply by 1000 to
// convert to millilitres.
flowMilliLitres = (flowRate / 60) * 1000;
// Add the millilitres passed in this second to the cumulative total
totalMilliLitres += flowMilliLitres;
// Print the flow rate for this second in litres / minute
Serial.print("Flow rate: ");
Serial.print(int(flowRate)); // Print the integer part of the variable
Serial.print("L/min");
Serial.print("\t"); // Print tab space
// Print the cumulative total of litres flowed since starting
Serial.print("Output Liquid Quantity: ");
Serial.print(totalMilliLitres);
Serial.print("mL / ");
Serial.print(totalMilliLitres / 1000);
Serial.println("L");
//user_data.printTo(Serial);
// Send data to OneM2M server
String sensor_string = String(int(flowRate)) + "L/min"; // + " Output Liquid Quantity:" +
String(totalMilliLitres)+ "mL/" + String(totalMilliLitres/1000) + "L";
sensor_string = "\"" + sensor_string + "\""; // DO NOT CHANGE THIS LINE
createCI(server, "Team27_Water_flow_monitoring", "node_1" , sensor_string);
delay(15000); // DO NOT CHANGE THIS LINE
}
}
