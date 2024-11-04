class Button {
  const int pin;
  const uint8_t triggerType = FALLING;
  static const unsigned long debouceDelayMs;
  void (*const ISR)(); /// pointer to function triggerTypeed on interrupt

  unsigned long lastInterruptTime = 0;
public:
  volatile bool handleButtonPressNextTick = false;

  Button(const int pin, uint8_t triggerType, void (*ISR)());

  inline bool isOnCooldown() const {return millis() - lastInterruptTime < debouceDelayMs;}
  void resetCooldown();
  inline virtual void onPress() {this->handleButtonPressNextTick = false;}

  virtual ~Button() {}
};

class StartStopButton : public Button {
  public:
    using Button::Button; /// inherit constructor
    virtual void onPress() override;
}

class DifficultyButton : public Button {
  public:
    using Button::Button; /// inherit constructor
    virtual void onPress() override;
}

class RGBLED {
  #define UNCHANGED (int)(-1)
  int redPin, greenPin, bluePin;
  int red, green, blue;

public:
  RGBLED();
  setComponents(int red, int green, int blue);
  turnWhite();

  ///bool beforeGameStartBlink = false;
};

void startStopButtonISR() {
  if(!startStopButton.isOnCooldown())
    startStopButton.resetCooldown(); /// button press will be treated on next loop call
}

void difficultyButtonISR() {
  if(!difficultyButton.isOnCooldown())
    difficultyButton.resetCooldown(); /// button press will be treated on next loop call
}

bool gameStarted = false;
RGBLED rgbLed;
const unsigned long Button::debouceDelayMs = 100;
StartStopButton startStopButton(STARTSTOPBUTTONPIN, FALLING, startStopButtonISR);
DifficultyButton difficultyButton(DIFFICULTYBUTTONPIN, FALLING, difficultyButtonISR);
enum Difficulty {EASY = 0, MEDIUM, HARD};

Difficulty chosenDifficulty;

void setup() {
  /// constructors called automatically

  /// turn the RGB LED white, program is idling at first
  rgbLed.turnWhite();
}

void loop() {
  if(startStopButton.handleButtonPressNextTick)
    startStopButton.onPress();
  if(difficultyButton.handleButtonPressNextTick)
    difficultyButton.onPress();

  if(!gameStarted)
    return;
}

void StartStopButton::onPress() { 
  Button::onPress();
  if(!gameStarted) {
      gameStarted = true;
      ///rgbLed.beforeGameStartBlink = true; 
      TODO

  }
  else {
      TODO
  }
}

void DifficultyButton::onPress() {
  Button::onPress();
  chosenDifficulty = (chosenDifficulty + 1) % 3; /// cycle through easy, medium, hard
}

Button::Button(const int pin, uint8_t triggerType, void (*ISR)()) 
                : pin(pin), triggerType(triggerType), ISR(ISR) {

  pinMode(this->pin, INPUT_PULLUP); /// activate pullup resistance and set pin
  attachInterrupt(digitalPinToInterrupt(this->pin), ISR, triggerType);
}

void Button::resetCooldown() {
  this->handleButtonPressNextTick = true;
  this->lastInterruptTime = millis();
}

RGBLED::RGBLED() {
  /// set all RGB LED components pins as outputs.
  pinMode(this->redPin, OUTPUT);
  pinMode(this->greenPin, OUTPUT);
  pinMode(this->bluePin, OUTPUT);
  red = green = blue = LOW;
}

RGBLED::turnWhite() {
  /// set all RGB components to HIGH to get white color
  digitalWrite(this->redPin, HIGH);
  digitalWrite(this->greenPin, HIGH); 
  digitalWrite(this->bluePin, HIGH);
  red = green = blue = HIGH;
}

void RGBLED::setComponents(int red, int green, int blue) {
  if(red != UNCHANGED) {
    digitalWrite(this->redPin, red);
    this->red = red;
  }

  if(green != UNCHANGED) {
    digitalWrite(this->greenPin, green);
    this->green = green;
  }

  if(blue != UNCHANGED) {
    digitalWrite(this->bluePin, blue);
    this->blue = blue;
  }
}