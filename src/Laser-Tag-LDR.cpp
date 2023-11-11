#include "Laser-Tag-LDR.h"

/// SETTING INPUT PINS
/// The exception handling has been added since the number of general pinINs and LDR pinINs mismatch
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

  /// Setting up the Hardware Interrupt to trigger on a RISING edge
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), deathInterrupt, RISING);

  /// Connecting to the WiFi network
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWD);
  Serial.println("Connecting to WiFi...");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(100);
  }
  Serial.println("Connected to the WiFi network!");

  /// API Request to check if the match has started or not
  http.begin((server + "?hasMatchStarted").c_str());

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

/// Here, I am shorting two GPIO pins in order to emulate a hardware interrupt.
/// When a player dies, the shorted interrupt circuit is set to HIGH which triggers the interrupt on core 1, running the following death loop.
/// After the loop is done, the interrupt circuit is set to LOW, and the control flow is returned back to the original loop.
/// The Player remains dead for 8 seconds. This can be tweaked.
void deathInterrupt()
{
  http.GET();
  isPlayerDead = true;
  for (int i = 0; i < 4; i++)
    {
      cute.play(S_BUTTON_PUSHED);
      delay(2000);
    }
  digitalWrite(INT_CALL, LOW);
  isPlayerDead = false;
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
  http.begin((server + "?playerShot=" + PLAYER_ID).c_str());
  while (!isPlayerDead)
  {
    for (int i = 0; i < 3; i++)
    {
      if (analogRead(pinIN_LDR[i]) > 750) /// Set this threshold value based on your Ambient Light, Laser Intensity, distance, etc
      {
        // Serial.println("Player " + (String)PLAYER_ID + " is dead! because of sensor " + (String)pinIN_LDR[i]);
        digitalWrite(INT_CALL, HIGH);
        break;
      }
    }
  }
  http.end();
}

void loop()
{
  /// Every 10 seconds, send a GET request via the designated API to check if the match has started
  /// Note, the API Requests are just dummy requests. You can setup your own http and API structure to handle the loop accordingly
  http.GET();
  int matchStarted = http.getString().toInt();

  while(matchStarted)
  {
    /// Every five minutes, check whether the match has ended or not. 
    /// If it has, set the esp32 in a limbo state until the reset button is pressed or power is turned off
    if ((millis() - lastTime) > 300000)
    {
      http.GET();
      matchStarted = http.getString().toInt();
      if(!matchStarted)
      {
        http.end();
        exit(0);
      }
    }

    /// If Player presses the trigger and has ammo, let them shoot. Otherwise, call the reload subroutine
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

  delay(10000);
}