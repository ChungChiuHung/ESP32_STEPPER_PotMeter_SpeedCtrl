#include <Arduino.h>
#include <string.h>
#include <math.h>
#include <EEPROM.h>

#define PWM_PIN (33)
#define POT_PIN (34)
#define STP_DIR (19)
#define STP_EN (18)

bool PwmIsEnabled = false;
bool LEDOutput = false;
bool PotValueIsRead = false;

int potValue = 0;
float potRatio = 0;
int nFreq = 0;
int pFreq = 0;

int freq = 8000;
const int PwmChannel = 0;
const int resolution = 8;

void PWMEnable()
{
  ledcSetup(PwmChannel, freq, resolution);
  ledcAttachPin(PWM_PIN, PwmChannel);

  int dutyCycle = 0;
  ledcWrite(PwmChannel, dutyCycle);
  PwmIsEnabled = true;
}

void get_ad_values()
{
  potValue = analogRead(POT_PIN);
  potRatio = ((float)potValue / 4095.0);
  Serial.print(potValue);
  Serial.print(" ");
  Serial.println(potRatio);
}

void rpm_change_by_potRatio()
{
  if (PwmIsEnabled)
  {
    Serial.println();
    pFreq = nFreq;
    nFreq = roundf(freq * potRatio);
    Serial.print("Freq: ");
    Serial.println(nFreq);
    if (nFreq == 0)
    {
      ledcWrite(PwmChannel, 0);
      digitalWrite(STP_EN, 1);
    }
    else
    {
      if (nFreq != pFreq)
      {
        digitalWrite(STP_EN, 0);
        ledcChangeFrequency(PwmChannel, nFreq, resolution);
        ledcWrite(PwmChannel, 100);
      }
      Serial.println("The values of potentiometer is not changed.");
    }
    // ledcWrite(PwmChannel, dutyCycle);
  }
}

void codeForTask1(void *parameter)
{
  for (;;)
  {
    get_ad_values();
    delay(100);
  }
}

void codeForTask2(void *parameter)
{
  for (;;)
  {
    rpm_change_by_potRatio();
    delay(100);
  }
}

TaskHandle_t Task1, Task2;

void setup(void)
{
  // put your setup code here, to run once:
  xTaskCreatePinnedToCore(
      codeForTask1,
      "Get AD Value Task",
      2000,
      NULL,
      2,
      &Task1, 0);

  xTaskCreatePinnedToCore(
      codeForTask2,
      "Change rotating Veloecity",
      2000,
      NULL,
      2,
      &Task2, 1);

  pinMode(PWM_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(STP_DIR, OUTPUT);
  pinMode(STP_EN, OUTPUT);

  digitalWrite(STP_DIR, 1);
  digitalWrite(STP_EN, 1);

  PWMEnable();
  Serial.begin(115200);
  while (!Serial)
    delay(100);
}

void loop()
{
  // put your main code here, to run repeatedly:
}