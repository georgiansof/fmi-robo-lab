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
#define forceStopTime 2000


int startButtonState = HIGH;
int lastStartButtonState = HIGH;

int stopButtonState = HIGH;
long stopButtonConstantStateTimestamp = 0;
int lastStopButtonState = HIGH;

unsigned long lastStartDebounceTime = 0;
unsigned long lastStopDebounceTime = 0;

unsigned long debounceDelay = 50;

long chargingTimerStart = 0;

int blinkingPeriod = 600; /// milisecunde

int blueLedBlinking_25 = false;
int lastBlueLedBlinking_25 = false;
int lastBlink_25 = 0;

int blueLedBlinking_50 = false;
int lastBlueLedBlinking_50 = false;
int lastBlink_50 = 0;

int blueLedBlinking_75 = false;
int lastBlueLedBlinking_75 = false;
int lastBlink_75 = 0;

int blueLedBlinking_100 = false;
int lastBlueLedBlinking_100 = false;
int lastBlink_100 = 0;

int charging = false;

void StartCharging() {
  digitalWrite(PIN_RGB_LED_GREEN, LOW);
  digitalWrite(PIN_RGB_LED_RED, HIGH);
  chargingTimerStart = millis();
  blueLedBlinking_25 = true;
  charging = true;
  Serial.print("Started charging at ");
  Serial.print(millis());
  Serial.print("\n");
}

void StopCharging() {
  blueLedBlinking_25 = false;
  blueLedBlinking_50 = false;
  blueLedBlinking_75 = false;
  blueLedBlinking_100 = false;
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
  Serial.begin(9600);
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

  if((millis() - lastStopDebounceTime) > debounceDelay) {
    if(stopButtonReading != stopButtonState) {
      stopButtonState = stopButtonReading;
      if(stopButtonReading == LOW && lastStopButtonState == HIGH)
        stopButtonConstantStateTimestamp = millis();
        
    }
    else
      if(charging && stopButtonState == LOW && millis() - stopButtonConstantStateTimestamp >= forceStopTime) {
        StopChargingForce();
        stopButtonConstantStateTimestamp = millis();
      }
  }

  if(millis() - chargingTimerStart > chargingTime) {
    if(millis() - chargingTimerStart <= chargingTime + 4000) {
      blueLedBlinking_25 = true;
      blueLedBlinking_50 = true;
      blueLedBlinking_75 = true;
      blueLedBlinking_100 = true;
      if(lastBlueLedBlinking_25 == false) {
        digitalWrite(PIN_BLUE_LED_HUNDRED, HIGH);
        digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, HIGH);
        digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
        digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
        lastBlink_25 = millis();
        lastBlink_50 = millis();
        lastBlink_75 = millis();
        lastBlink_100 = millis();
      }
    }
    else
      StopCharging();
  }

  if(charging) {
    if(millis() - chargingTimerStart <= chargingTime) {
      if(millis() - chargingTimerStart >= chargingTime / 4.0 * 3) {
        blueLedBlinking_100 = true;

        blueLedBlinking_75 = false;
        blueLedBlinking_50 = false;
        blueLedBlinking_25 = false;
        if(lastBlueLedBlinking_100 == false) {
          Serial.print("100 set blinking\n");
          lastBlink_100 = millis();
          digitalWrite(PIN_BLUE_LED_HUNDRED, HIGH);
          digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, HIGH);
          digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
          digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
        }
      }
      else
        if(millis() - chargingTimerStart >= chargingTime / 2.0) {
          blueLedBlinking_75 = true;

          blueLedBlinking_100 = false;
          blueLedBlinking_50 = false;
          blueLedBlinking_25 = false;
          if(lastBlueLedBlinking_75 == false) {
            lastBlink_75 = millis();
            Serial.print("75 set blinking\n");
            digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, HIGH);
            digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
            digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
          }
        }
        else
          if(millis() - chargingTimerStart >= chargingTime / 4.0) {
            blueLedBlinking_50 = true;

            blueLedBlinking_75 = false;
            blueLedBlinking_100 = false;
            blueLedBlinking_25 = false;
            if(lastBlueLedBlinking_50 == false) {
              lastBlink_50 = millis();
              digitalWrite(PIN_BLUE_LED_FIFTY, HIGH);
              digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
              Serial.print("50 set blinking\n");
            }
          }
          else {
            blueLedBlinking_25 = true;
            
            blueLedBlinking_50 = false;
            blueLedBlinking_75 = false;
            blueLedBlinking_100 = false;
            if(lastBlueLedBlinking_25 == false) {
              digitalWrite(PIN_BLUE_LED_TWENTYFIVE, HIGH);
              lastBlink_25 = millis();
              Serial.print("25 set blinking\n");
            }
          }
    }
  }
  
  if(blueLedBlinking_25) {
    if(millis() - lastBlink_25 >= blinkingPeriod) {
      lastBlink_25 = millis();
      digitalWrite(PIN_BLUE_LED_TWENTYFIVE, !digitalRead(PIN_BLUE_LED_TWENTYFIVE));
    }
  }

  if(blueLedBlinking_50) {
    if(millis() - lastBlink_50 >= blinkingPeriod) {
      lastBlink_50 = millis();
      digitalWrite(PIN_BLUE_LED_FIFTY, !digitalRead(PIN_BLUE_LED_FIFTY));
    }
  }

  if(blueLedBlinking_75) {
    if(millis() - lastBlink_75 >= blinkingPeriod) {
      lastBlink_75 = millis();
      digitalWrite(PIN_BLUE_LED_SEVENTYFIVE, !digitalRead(PIN_BLUE_LED_SEVENTYFIVE));
    }
  }

  if(blueLedBlinking_100) {
    if(millis() - lastBlink_100 >= blinkingPeriod) {
      lastBlink_100 = millis();
      digitalWrite(PIN_BLUE_LED_HUNDRED, !digitalRead(PIN_BLUE_LED_HUNDRED));
    }
  }
  lastStartButtonState = startButtonReading;
  lastStopButtonState = stopButtonReading;
  lastBlueLedBlinking_25 = blueLedBlinking_25;
  lastBlueLedBlinking_50 = blueLedBlinking_50;
  lastBlueLedBlinking_75 = blueLedBlinking_75;
  lastBlueLedBlinking_100 = blueLedBlinking_100;
}

