#define PIN_BUTON_STOP 2
#define PIN_BUTON_START 3
#define PIN_RGB_LED_GREEN 4
#define PIN_RGB_LED_BLUE 5
#define PIN_RGB_LED_RED 6
#define PIN_BLUE_LED_HUNDRED 7
#define PIN_BLUE_LED_SEVENTYFIVE 8
#define PIN_BLUE_LED_FIFTY 9
#define PIN_BLUE_LED_TWENTYFIVE 10

#define chargingTime 12000
#define FORCE_STOP_TIME 1000
#define FOUR_LEDS_BLINKING_TIME 4000

int startButtonState = HIGH;
int lastStartButtonState = HIGH;

int stopButtonState = HIGH;
long stopButtonConstantStateTimestamp = 0;
int lastStopButtonState = HIGH;

unsigned long lastStartDebounceTime = 0;
unsigned long lastStopDebounceTime = 0;

unsigned long debounceDelay = 50;
unsigned long debounceDelayStop = FORCE_STOP_TIME;

long chargingTimerStart = 0;

int blinkingPeriod = 600; /// milisecunde

int blueLedBlinking25 = false;
int lastblueLedBlinking25 = false;
int lastBlink25 = 0;

int blueLedBlinking50 = false;
int lastblueLedBlinking50 = false;
int lastBlink50 = 0;

int blueLedBlinking75 = false;
int lastblueLedBlinking75 = false;
int lastBlink75 = 0;

int blueLedBlinking100 = false;
int lastblueLedBlinking100 = false;
int lastBlink100 = 0;

int charging = false;

void StartCharging() {
  digitalWrite(PIN_RGB_LED_GREEN, LOW);
  digitalWrite(PIN_RGB_LED_RED, HIGH);
  chargingTimerStart = millis();
  blueLedBlinking25 = true;
  charging = true;
  //Serial.print("Started charging at ");
  //Serial.print(millis());
  //Serial.print("\n");
}

void StopCharging() {
  blueLedBlinking25 = false;
  blueLedBlinking50 = false;
  blueLedBlinking75 = false;
  blueLedBlinking100 = false;
  digitalWrite(PIN_BLUE_LED_TWENTYFIVE, 0);
  digitalWrite(PIN_BLUE_LED_FIFTY, 0);
  digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, 0);
  digitalWrite(PIN_BLUE_LED_HUNDRED, 0);
  digitalWrite(PIN_RGB_LED_RED, 0);
  digitalWrite(PIN_RGB_LED_GREEN, 1);
  charging = false;
}

void StopChargingForce() {
  chargingTimerStart = (int)millis() - chargingTime;
}

void setup() {
  ///Serial.begin(9600);
  pinMode(PIN_BUTON_STOP, INPUT_PULLUP);
  pinMode(PIN_BUTON_START, INPUT_PULLUP);
  for(int i = 4; i <= 10; ++i)
    pinMode(i, OUTPUT);

  digitalWrite(PIN_RGB_LED_GREEN, HIGH);
}

void loop() {
  int startButtonReading = digitalRead(PIN_BUTON_START);
  int stopButtonReading = digitalRead(PIN_BUTON_STOP);

  if(startButtonReading != lastStartButtonState) 
    lastStartDebounceTime = millis();

  if(stopButtonReading != lastStopButtonState)
    lastStopDebounceTime = millis();

  if((millis() - lastStartDebounceTime) > debounceDelay) {
    if(startButtonReading != startButtonState) {
      startButtonState = startButtonReading;

      if(startButtonState == LOW && !charging)
        StartCharging();
    }
  }

  if((millis() - lastStopDebounceTime) > debounceDelayStop) {
    if(stopButtonReading != stopButtonState) {
      stopButtonState = stopButtonReading;
      if(stopButtonReading == LOW && lastStopButtonState == HIGH)
        stopButtonConstantStateTimestamp = millis();
        
    }
    else
      if(charging && stopButtonState == LOW && millis() - stopButtonConstantStateTimestamp >= FORCE_STOP_TIME) {
        StopChargingForce();
        stopButtonConstantStateTimestamp = millis();
      }
  }

  if(millis() - chargingTimerStart > chargingTime) {
    if(millis() - chargingTimerStart <= chargingTime + FOUR_LEDS_BLINKING_TIME) {
      blueLedBlinking25 = true;
      blueLedBlinking50 = true;
      blueLedBlinking75 = true;
      blueLedBlinking100 = true;
      if(lastblueLedBlinking25 == false) {
        digitalWrite(PIN_BLUE_LED_HUNDRED, HIGH);
        digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, HIGH);
        digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
        digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
        lastBlink25 = millis();
        lastBlink50 = millis();
        lastBlink75 = millis();
        lastBlink100 = millis();
      }
    }
    else
      StopCharging();
  }

  if(charging) {
    if(millis() - chargingTimerStart <= chargingTime) {
      if(millis() - chargingTimerStart >= chargingTime / 4.0 * 3) {
        blueLedBlinking100 = true;

        blueLedBlinking75 = false;
        blueLedBlinking50 = false;
        blueLedBlinking25 = false;
        if(lastblueLedBlinking100 == false) {
          ///Serial.print("100 set blinking\n");
          lastBlink100 = millis();
          digitalWrite(PIN_BLUE_LED_HUNDRED, HIGH);
          digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, HIGH);
          digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
          digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
        }
      }
      else
        if(millis() - chargingTimerStart >= chargingTime / 2.0) {
          blueLedBlinking75 = true;

          blueLedBlinking100 = false;
          blueLedBlinking50 = false;
          blueLedBlinking25 = false;
          if(lastblueLedBlinking75 == false) {
            lastBlink75 = millis();
           /// Serial.print("75 set blinking\n");
            digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, HIGH);
            digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
            digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
          }
        }
        else
          if(millis() - chargingTimerStart >= chargingTime / 4.0) {
            blueLedBlinking50 = true;

            blueLedBlinking75 = false;
            blueLedBlinking100 = false;
            blueLedBlinking25 = false;
            if(lastblueLedBlinking50 == false) {
              lastBlink50 = millis();
              digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
              digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
              ///Serial.print("50 set blinking\n");
            }
          }
          else {
            blueLedBlinking25 = true;
            
            blueLedBlinking50 = false;
            blueLedBlinking75 = false;
            blueLedBlinking100 = false;
            if(lastblueLedBlinking25 == false) {
              digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
              lastBlink25 = millis();
             /// Serial.print("25 set blinking\n");
            }
          }
    }
  }
  
  if(blueLedBlinking25) {
    if(millis() - lastBlink25 >= blinkingPeriod) {
      lastBlink25 = millis();
      digitalWrite(PIN_BLUE_LED_TWENTYFIVE, !digitalRead(PIN_BLUE_LED_TWENTYFIVE));
    }
  }

  if(blueLedBlinking50) {
    if(millis() - lastBlink50 >= blinkingPeriod) {
      lastBlink50 = millis();
      digitalWrite(PIN_BLUE_LED_FIFTY, !digitalRead(PIN_BLUE_LED_FIFTY));
    }
  }

  if(blueLedBlinking75) {
    if(millis() - lastBlink75 >= blinkingPeriod) {
      lastBlink75 = millis();
      digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, !digitalRead(PIN_BLUE_LED_SEVENTYFIVE));
    }
  }

  if(blueLedBlinking100) {
    if(millis() - lastBlink100 >= blinkingPeriod) {
      lastBlink100 = millis();
      digitalWrite(PIN_BLUE_LED_HUNDRED, !digitalRead(PIN_BLUE_LED_HUNDRED));
    }
  }
  lastStartButtonState = startButtonReading;
  lastStopButtonState = stopButtonReading;
  lastblueLedBlinking25 = blueLedBlinking25;
  lastblueLedBlinking50 = blueLedBlinking50;
  lastblueLedBlinking75 = blueLedBlinking75;
  lastblueLedBlinking100 = blueLedBlinking100;
}

