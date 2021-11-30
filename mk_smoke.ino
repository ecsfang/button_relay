#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#include <arduino-timer.h>

#include "mk_smoke.h"

#define LED_PIN   D4  // Onboard led (lights when relay is on)
#define RELAY_PIN D5  // Set to corresponding relay pin!

int on_time = 5;     // Seconds on
int off_time = 5*60; // Seconds off

auto onTimer = timer_create_default();
auto offTimer = timer_create_default();

// Set these to your desired credentials
const char *ssid = "MK_Smoke";
const char *password = "thereisnospoon";

ESP8266WebServer server(80);

// Handles main page at 192.168.4.1

void handleRoot() {
  String s = MAIN_page;    
  s.replace("@@ON@@", String(on_time));
  s.replace("@@OFF@@", String(off_time));
  server.send(200, "text/html", s);  
}

bool turn_relay_off(void *argument);

bool turn_relay_on(void *argument) {
  Serial.println(F("Turn relay ON!"));
  digitalWrite(LED_PIN, 0);   // Turn LED on
  digitalWrite(RELAY_PIN, 1); // Turn relay on
  onTimer.in(on_time*1000, turn_relay_off); // Set new delay until turn off
  return false;
}

bool turn_relay_off(void *argument) {
  Serial.println(F("Turn relay OFF!"));
  digitalWrite(LED_PIN, 1);   // Turn LED off
  digitalWrite(RELAY_PIN, 0); // Turn relay off
  offTimer.in(off_time*1000, turn_relay_on); // Set new delay until next on-cycle
  return false;
}

// Handle settings
void handleForm() {
  Serial.println(F("Handle form!"));

 String onTime = server.arg("on_time"); 
 String offTime = server.arg("off_time"); 

 int ont = onTime.toInt();
 int oft = offTime.toInt();

 if( ont && oft ) {
   on_time = ont;
   off_time = oft;
   Serial.print("On time:");
   Serial.println(onTime);

   Serial.print("Off time:");
   Serial.println(offTime);
   server.sendHeader("Location", "/");
   server.send(302, "text/plain", "Updated-- Press Back Button");  //This Line Keeps It on Same Page

   onTimer.cancel();
   offTimer.cancel();

   // Restart cycle ...
   turn_relay_off(NULL);
   
 } else {
   // We couldn't read a number from the configuration - so ignore!
   String s = "<a href='/'> Bad argument! </a>";
   server.send(200, "text/html", s); //Send web page
   Serial.println("Bad argument!");
 }
 delay(500);
}

void setup() {
  
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("MK-Smoke!!"));
  
  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  server.on("/", handleRoot);
  server.on("/action_page", handleForm);

  server.begin();

  // Init gpio-pins
  
  pinMode(LED_PIN, OUTPUT); 
  pinMode(RELAY_PIN, OUTPUT); 
  Serial.println(F("Start!"));
  
  // Start the timer in off-position!
  turn_relay_off(NULL);
}

void loop() {
  server.handleClient();
  onTimer.tick(); // tick the timer
  offTimer.tick(); // tick the timer
}
