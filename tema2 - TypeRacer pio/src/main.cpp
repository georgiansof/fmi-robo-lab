#include <Arduino.h>
#include <avr/interrupt.h> 

// Constants
const int STARTSTOPBUTTONPIN = 2;
const int DIFFICULTYBUTTONPIN = 3;
const unsigned long ROUND_DURATION_MS = 30000;
const unsigned long COUNTDOWN_DURATION_MS = 3000; // countdown
const unsigned long ERROR_DISPLAY_DURATION_MS = 500; //  error display duration

// Enum for difficulty
enum Difficulty {EASY = 0, MEDIUM, HARD};

// Global Variables
bool gameStarted = false;
bool countdownActive = false;
bool errorDisplayed = false;
unsigned long roundStartTime;
unsigned long countdownStartTime;
unsigned long errorDisplayStartTime;
Difficulty chosenDifficulty = EASY;
int score = 0;
String currentWord;
String userInput = "";
volatile int timerOverflowCounter = 0;

unsigned long lastBlinkTime = 0;
bool ledOnDuringCountdown = false;


// Word Dictionary
const char* wordDictionary[] = {
  "apple", "banana", "cherry", "date", "elderberry",
  "fig", "grape", "honeydew", "kiwi", "lemon",
  "mango", "nectarine", "orange", "papaya", "quince",
  "raspberry", "strawberry", "tangerine", "watermelon", "zucchini"
};
const int wordCount = sizeof(wordDictionary) / sizeof(wordDictionary[0]);

class RGBLED {
  const int redPin, greenPin, bluePin;

public:
  RGBLED(int rPin, int gPin, int bPin)
      : redPin(rPin), greenPin(gPin), bluePin(bPin) {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
  }

  void turnWhite() {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
  }

  void turnGreen() {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
  }

  void turnRed() {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
  }

  void turnOff() {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
  }
};

class Button {
protected:
  const int pin;
  const uint8_t triggerType;
  const unsigned long debounceDelayMs;
  unsigned long lastInterruptTime = 0;

public:
  volatile bool handleButtonPressNextTick = false;

  Button(int p, uint8_t t, unsigned long debounce) : pin(p), triggerType(t), debounceDelayMs(debounce) {
    pinMode(pin, INPUT_PULLUP);
  }

  bool isOnCooldown() const { return millis() - lastInterruptTime < debounceDelayMs; }

  void resetCooldown() {
    handleButtonPressNextTick = true;
    lastInterruptTime = millis();
  }

  virtual void onPress() {
    handleButtonPressNextTick = false;
  }
};


class StartStopButton : public Button {
public:
  using Button::Button;
  void onPress() override;
};

class DifficultyButton : public Button {
public:
  using Button::Button;
  void onPress() override;
};

// Function Prototypes
void startStopButtonISR();
void difficultyButtonISR();
void startCountdown();
void startRound();
void updateDifficulty();
void generateRandomWord();
void checkCharacterInput(char ch);
void handleBackspace();
void resetErrorDisplay();

// Instances
RGBLED rgbLed(4, 5, 6); // Example pins for RGB LED
StartStopButton startStopButton(STARTSTOPBUTTONPIN, FALLING, 1000);
DifficultyButton difficultyButton(DIFFICULTYBUTTONPIN, FALLING, 1000);

// Timer1 ISR for word generation with software counter
ISR(TIMER1_COMPA_vect) {
  static int counter = 0;

  if (gameStarted) {
    counter++;
    
    if (counter >= timerOverflowCounter) {
      generateRandomWord();
      counter = 0; // Reset counter after generating the word
    }
  }
}

// Interrupt Service Routines
void startStopButtonISR() {
  if (!startStopButton.isOnCooldown()) {
    startStopButton.resetCooldown();
  }
}

void difficultyButtonISR() {
  if (!difficultyButton.isOnCooldown()) {
    difficultyButton.resetCooldown();
  }
}

// Button Methods
void StartStopButton::onPress() {
  Button::onPress();
  if (!gameStarted && !countdownActive) {
    startCountdown(); // Start the 3-second countdown
  } else if (gameStarted) {
    gameStarted = false;
    rgbLed.turnWhite();
    Serial.println("Game stopped.");
    Serial.print("Score: ");
    Serial.println(score);
    TCCR1B &= ~(1 << CS11); // Stop Timer1
  }
}

void DifficultyButton::onPress() {
  Button::onPress();
  chosenDifficulty = static_cast<Difficulty>((chosenDifficulty + 1) % 3);
  updateDifficulty();
}

void updateDifficulty() {
  const char* difficultyNames[] = {"Easy mode on!", "Medium mode on!", "Hard mode on!"};
  Serial.println(difficultyNames[chosenDifficulty]);

  timerOverflowCounter = 0;
  uint16_t timerInterval = 15625; 
  
  switch (chosenDifficulty) {
    case EASY:
      timerOverflowCounter = 150;
      break;
    case MEDIUM:
      timerOverflowCounter = 100;   
      break;
    case HARD:
      timerOverflowCounter = 80;   
      break;
  }

  cli(); // Disable global interrupts
  TCCR1A = 0; 
  TCCR1B = 0;
  TCNT1 = 0;  
  OCR1A = timerInterval;
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS11) | (1 << CS10);  
  TIMSK1 |= (1 << OCIE1A);
  sei(); // Enable global interrupts
}

// Start Countdown Function
void startCountdown() {
  countdownActive = true;
  countdownStartTime = millis();
  lastBlinkTime = millis(); // Initialize last blink time
  ledOnDuringCountdown = false;
  rgbLed.turnOff(); // Ensure the LED is off at the start
  Serial.println("Countdown started...");
}

// Start Round Function
void startRound() {
  countdownActive = false;
  gameStarted = true;
  score = 0;
  rgbLed.turnGreen();
  roundStartTime = millis();
  Serial.println("Round started!");
  updateDifficulty();
  
  generateRandomWord(); // Generate the first word immediately
}

// Random Word Generation Function
void generateRandomWord() {
  if(!gameStarted)
    return;
  int randomIndex = random(0, wordCount);
  currentWord = wordDictionary[randomIndex];
  Serial.print("\nType this word: ");
  Serial.println(currentWord);
  userInput = ""; // Clear user input for new word
}

// Character Input Check Function
void checkCharacterInput(char ch) {
  if (userInput.length() < currentWord.length()) {
    if (ch == currentWord[userInput.length()]) {
      userInput += ch; 
      if (userInput.equals(currentWord)) { 
        score++;
        rgbLed.turnGreen();
        generateRandomWord();
      }
    } else {
      rgbLed.turnRed();
      errorDisplayStartTime = millis();
      errorDisplayed = true;
    }
  }
}

// Handle Backspace
void handleBackspace() {
  if (userInput.length() > 0) {
    userInput.remove(userInput.length() - 1);
  }
}

// Reset Error Display after 500 ms
void resetErrorDisplay() {
  if (errorDisplayed && millis() - errorDisplayStartTime >= ERROR_DISPLAY_DURATION_MS) {
    rgbLed.turnGreen();
    errorDisplayed = false;
  }
}

void setup() {
  Serial.begin(9600);
  rgbLed.turnWhite(); 
  updateDifficulty();
  attachInterrupt(digitalPinToInterrupt(STARTSTOPBUTTONPIN), startStopButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(DIFFICULTYBUTTONPIN), difficultyButtonISR, FALLING);
}

void loop() {
  if (startStopButton.handleButtonPressNextTick) startStopButton.onPress();
  if (difficultyButton.handleButtonPressNextTick) difficultyButton.onPress();

  if (countdownActive) {
    // Blink the LED every 500 ms
    if (millis() - lastBlinkTime >= 500) { // 500 ms interval for blinking
      lastBlinkTime = millis();
      ledOnDuringCountdown = !ledOnDuringCountdown;
      if (ledOnDuringCountdown) {
        rgbLed.turnWhite(); // Turn on LED (white) during countdown
      } else {
        rgbLed.turnOff();   // Turn off LED
      }
    }

    // Check if countdown is over
    if (millis() - countdownStartTime >= COUNTDOWN_DURATION_MS) {
      startRound(); // End countdown and start the round
    }
  }

  if (gameStarted && (millis() - roundStartTime >= ROUND_DURATION_MS)) {
    gameStarted = false;
    rgbLed.turnWhite();
    Serial.print("Round ended! Score: ");
    Serial.println(score);
    TCCR1B &= ~(1 << CS11);
  }

  resetErrorDisplay(); 

  while (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == '\b') { 
      handleBackspace();
    } else {
      checkCharacterInput(ch); 
    }
  }
}
