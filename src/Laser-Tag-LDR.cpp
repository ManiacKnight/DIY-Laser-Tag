#include <Arduino.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <CuteBuzzerSounds.h>

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

/// SETTING UP GAME VARIABLES
int ammo = 12; // Set this value based on the number of bullets you want to give to the player. Make sure to set it in reload() as well
bool isPlayerDead = false;
int pinIN_LDR[3] = {LDR_PIN_FRONT1, LDR_PIN_FRONT2, LDR_PIN_BACK};
int pinIN[1] = {TRIGGER};
int pinOUT[2] = {LASER_PIN, VIBRATION};

/// SETTING UP WIFI AND SERVER VARIABLES
WiFiMulti wifiMulti;
HTTPClient http;
string server = "https://YOURGAMESERVER.com" // Replace this with your game server URL

    /// DEFINING TASK HANDLES FOR DUAL CORE PROGRAMMING AND SOFWATE INTERRUPTING
    TaskHandle_t ArmourTask,
       softwareINT;

/// SETTING INPUT PINS
void setPinIN()
{
  for (int i = 0; i < sizeof(pinIN) / sizeof(pinIN[0]); i++)
  {
    try
    {
      pinMode(pinIN[i], INPUT);
      pinMode(pinIN_LDR[i], INPUT);
    }

    catch (const std::exception &e)
    {
      continue;
    }
  }
}

/// SETTING OUTPUT PINS
void setPinOUT()
{
  for (int i = 0; i < (sizeof(pinOUT) / sizeof(pinOUT[0])); i++)
  {
    // Serial.println("Pin " + (String)pinOUT[i] + " set to OUTPUT");
    pinMode(pinOUT[i], OUTPUT);
  }
}

void setup()
{
  Serial.begin(115200);
  cute.init(BUZZER);

  setPinIN();
  setPinOUT();

  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWD);
  Serial.println("Connecting to WiFi...");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(100);
  }
  Serial.println("Connected to the WiFi network!");

  /*String matchTimer = server + "?matchTimerAPIRequest";
  http.begin(server.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();*/

  /// Running code for the Armour, containing player LDRs, on Core 0
  /// With this, both the LaserTag and Armor code will run simultaneously
  xTaskCreatePinnedToCore(
      ArmourLoop,
      "Code running Armour Loop",
      1000,
      NULL,
      1,
      &ArmourTask,
      0);
}

void reload()
{
  digitalWrite(VIBRATION, HIGH);
  cute.play(S_HAPPY);
  delay(5000);
  ammo = 12;
  digitalWrite(VIBRATION, LOW);
}

void ArmourLoop(void *parameter)
{
  while (true)
  {
    for (int i = 0; i < 3; i++)
    {
      if (analogRead(pinIN_LDR[i]) > 750) // Set this threshold value based on your Ambient Light, Laser Intensity, distance, etc
      {
        // Serial.println("Player " + (String)PLAYER_ID + " is dead! because of sensor " + (String)pinIN_LDR[i]);
        isPlayerDead = true;
        break;
      }
    }
  }
}

void loop()
{
  if (!isPlayerDead)
  {
    if (digitalRead(TRIGGER) == HIGH && ammo != 0)
    {
      Serial.println("Shooting laser!");
      digitalWrite(LASER_PIN, HIGH);
      digitalWrite(VIBRATION, HIGH);
      delay(150);
      digitalWrite(VIBRATION, LOW);
      digitalWrite(LASER_PIN, LOW);
      ammo--;
    }

    else if (!ammo)
    {
      reload();
    }
  }

  else
  {
    for (int i = 0; i < 4; i++)
    {
      cute.play(S_BUTTON_PUSHED);
      delay(2000);
    }
    isPlayerDead = false;
  }
}