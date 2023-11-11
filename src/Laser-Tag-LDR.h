#include <Arduino.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "CuteBuzzerSounds.h"

/// DEFINING WIFI/SERVER RELATED STUFF
#define WIFI_SSID "SSID"
#define WIFI_PASSWD "PASSWORD"
#define PLAYER_ID 1

/// DEFINING PIN NUMBERS
#define LDR_PIN_BACK 31
#define LDR_PIN_FRONT1 32
#define LDR_PIN_FRONT2 12
#define LASER_PIN 27
#define TRIGGER 35
#define VIBRATION 34
#define BUZZER 33
#define INTERRUPT_PIN 26
#define INT_CALL 25

/// SETTING UP GAME VARIABLES
int ammo = 12; // Set this value based on the number of bullets you want to give to the player. Make sure to set it in reload() as well
bool isPlayerDead = false;
const int pinIN_LDR[3] = {LDR_PIN_FRONT1, LDR_PIN_FRONT2, LDR_PIN_BACK};
const int pinIN[1] = {TRIGGER};
const int pinOUT[3] = {LASER_PIN, VIBRATION, INT_CALL};
unsigned long lastTime = 0;

//Func Prototypes
void ArmourLoop(void *parameter);
void reload();
void deathInterrupt();

/// SETTING UP WIFI AND SERVER VARIABLES
WiFiMulti wifiMulti;
HTTPClient http;
String server = "https://YOURGAMESERVER.com"; // Replace this with your game server URL

/// DEFINING TASK HANDLES FOR DUAL CORE PROGRAMMING AND SOFWATE INTERRUPTING
TaskHandle_t ArmourTask,
  softwareINT;
