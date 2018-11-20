// See the comment for flashing instruction.
#include <ESP8266WiFi.h>
#include <controller/controller.h>

#include <rws_wifi.h>

#define ONBOARD_LED_GREEN 12
#define ONBOARD_LED_BLUE  13
#define ONBOARD_LED_RED   15
#define ESP8266_LED 2

#define DELAY_MS 5

ADC_MODE(ADC_VCC);

controller ctrl;

void setup() 
{
  pinMode(ONBOARD_LED_GREEN, OUTPUT);
  pinMode(ONBOARD_LED_BLUE, OUTPUT);
  pinMode(ONBOARD_LED_RED, OUTPUT);
  pinMode(ESP8266_LED, OUTPUT);
  Serial.begin(115200);
  delay(10000);

  // Print Wi-Fi Information
  WiFi.printDiag(Serial);

  // Print VCC voltage
  Serial.print("Current VCC: ");
  Serial.println(ESP.getVcc());
}

void loop() 
{
  digitalWrite(ONBOARD_LED_RED, HIGH);
  digitalWrite(ESP8266_LED, HIGH);
  Serial.println("LED on");
  delay(DELAY_MS);
  
  digitalWrite(ONBOARD_LED_RED, LOW);
  digitalWrite(ESP8266_LED, LOW);
  Serial.println("LED off");
  delay(DELAY_MS);
  
  digitalWrite(ONBOARD_LED_GREEN, HIGH);
  digitalWrite(ESP8266_LED, HIGH);
  Serial.println("LED on");
  delay(DELAY_MS);
  
  digitalWrite(ONBOARD_LED_GREEN, LOW);
  digitalWrite(ESP8266_LED, LOW);
  Serial.println("LED off");
  delay(DELAY_MS);
  
  digitalWrite(ONBOARD_LED_BLUE, HIGH);
  digitalWrite(ESP8266_LED, HIGH);
  Serial.println("LED on");
  delay(DELAY_MS);
  
  digitalWrite(ONBOARD_LED_BLUE, LOW);
  digitalWrite(ESP8266_LED, LOW);
  Serial.println("LED off");
  delay(DELAY_MS);
}