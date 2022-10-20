/*
 * I, Rawad Haddad, 000777218 certify that this material is my original work. No other person's work has been used without due acknowledgement.
 */

#include <Arduino.h> 

// digital input pin definitions 
#define PIN_PIR D5 
#define PIN_BUTTON D6 

// define delays
#define BLINK_DELAY 62.5
#define BUTTON_DELAY 200 

// define blink timer and counters
#define BLINK_COUNTER 4
#define BLINK_TIMER 10

// define all possible alarm states. 
#define ALARM_DISABLED  0 
#define ALARM_ENABLE    1 
#define ALARM_COUNTDOWN 2 
#define ALARM_ACTIVE    3 

// global Variables
int  iAlarmState = ALARM_DISABLED; 
int ledState = HIGH;
int previousButtonState = HIGH; //previousButtonState
unsigned long lastDebounceTime = 0; //lastDebounceTime

// ************************************************************* 
void setup() { 
  // configure the USB serial monitor 
  Serial.begin(115200); 

  // configure the LED output 
  pinMode(LED_BUILTIN, OUTPUT); 

  // PIR sensor is an INPUT 
  pinMode(PIN_PIR, INPUT); 

  // Button is an INPUT 
  pinMode(PIN_BUTTON, INPUT_PULLUP); 
} 

void controlButton() {
  int buttonState = digitalRead(PIN_BUTTON);
  if (millis() - lastDebounceTime > BUTTON_DELAY)
  {
    analogWrite(LED_BUILTIN, 1023);
    if (buttonState != previousButtonState)
    {
      previousButtonState = buttonState;
      if (buttonState == LOW)
      {
        ledState = !ledState;
        if (iAlarmState == ALARM_DISABLED)
        {
          Serial.println("Alarm Enabled");
          iAlarmState = ALARM_ENABLE;
        }
        else if (iAlarmState == ALARM_COUNTDOWN)
        {
          Serial.println("Alarm Disabled");
          iAlarmState = ALARM_DISABLED;
        }
      }
    }
  }
}

/// @brief Controls the LED blinking per second
/// @param counter number of blinks
void ledBlinker(int counter)
{
  for (int i = 0; i < counter; i++)
  {
    controlButton();
    digitalWrite(LED_BUILTIN, LOW);
    delay(BLINK_DELAY);
    controlButton();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(BLINK_DELAY);
    controlButton();
    Serial.println("Blink");
  }
}

void alarmBlinkTimer() {
  // int seconds = BLINK_TIMER;
  // while (seconds > 0) {
  for (int seconds = BLINK_TIMER; seconds > 0; seconds --) {
    if (iAlarmState == ALARM_DISABLED) {
      break;
    }
    ledBlinker(BLINK_COUNTER);
    Serial.println("Blink Seconds: " + String(seconds));
    // seconds--;
  }
  if (iAlarmState == ALARM_COUNTDOWN) {
    Serial.println("Alarm Active");
    iAlarmState = ALARM_ACTIVE;
  }
}

void alarmController() {
  bool bPIR;
  controlButton();
  bPIR = digitalRead(PIN_PIR);
  if (iAlarmState == ALARM_ENABLE)
  {
    if (bPIR)
    {
      iAlarmState = ALARM_COUNTDOWN;
    }
  }
  if (iAlarmState == ALARM_COUNTDOWN)
  {
    Serial.println("Countdown Started");
    alarmBlinkTimer();
  }
  if (iAlarmState == ALARM_ACTIVE)
  {
    Serial.println("LED On");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop() { 
  // Listen to controller
  alarmController();
} 