// @author PoofyPloop

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
int previousButtonState = HIGH;
unsigned long lastDebounceTime = 0;

/// @brief Called at start-up
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

/// @brief Controls button state and functionality
void controlButton() {

  // read button state
  int buttonState = digitalRead(PIN_BUTTON);

  // Sets initial LED state
  analogWrite(LED_BUILTIN, 1023);

  // check if the button's state changed
  if (buttonState != previousButtonState) {
    // Makes sure there's a 200ms delay between debounces to avoid button spamming
    if ((millis() - lastDebounceTime) > BUTTON_DELAY) {
      // updates the value of the previous state of the button 
      previousButtonState = buttonState;
      if (buttonState == LOW)
      {
        // Swaps LED state
        ledState = !ledState;
        if (iAlarmState == ALARM_DISABLED)
        {
          Serial.println("Alarm Enabled");
          iAlarmState = ALARM_ENABLE;
        }
        // Disables the alarm if button is pressed during countdown
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
    // multiple controlButton() functions just to detect button press while in blink. This way you don't have to press button between blinks 
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

/// @brief Controls the 10 secound countdown when motion is detected
void alarmBlinkTimer() {
  for (int seconds = BLINK_TIMER; seconds > 0; seconds --) {
    if (iAlarmState == ALARM_DISABLED) {
      break;
    }
    ledBlinker(BLINK_COUNTER);
    Serial.println("Blink Seconds: " + String(seconds));
  }
  if (iAlarmState == ALARM_COUNTDOWN) {
    Serial.println("Alarm Active");
    iAlarmState = ALARM_ACTIVE;
  }
}

/// @brief Controls main functionality of the alarm system
void alarmController() {
  bool bPIR;
  controlButton();

  // Reads PIR sensor
  bPIR = digitalRead(PIN_PIR);

  // Alarm conditions
  if (iAlarmState == ALARM_ENABLE)
  {
    // Starts 10 seconds countdown if motion is detected
    if (bPIR)
    {
      iAlarmState = ALARM_COUNTDOWN;
      Serial.println("Countdown Started");
      // Listen for countdown timer
      alarmBlinkTimer();
    }
  }
  else if (iAlarmState == ALARM_ACTIVE)
  {
    Serial.println("LED On");
    // Turns on LED
    digitalWrite(LED_BUILTIN, LOW);
  }
}

/// @brief Function that loops forever
void loop() { 
  // Listen for alarm control
  alarmController();
} 
