#include <Arduino.h>
#include <avr/interrupt.h> // Library for working with interrupts

// Constants
const int STARTSTOPBUTTONPIN = 2;
const int DIFFICULTYBUTTONPIN = 3;
const unsigned long ROUND_DURATION_MS = 30000; // 30 seconds
const unsigned long COUNTDOWN_DURATION_MS = 3000; // 3 seconds countdown

// Enum for difficulty
enum Difficulty {EASY = 0, MEDIUM, HARD};

// Global Variables
bool gameStarted = false;
bool countdownActive = false;
unsigned long roundStartTime;
unsigned long countdownStartTime;
Difficulty chosenDifficulty = EASY;
int score = 0;
String currentWord;
String userInput = "";

// Word Dictionary
const char* wordDictionary[] = {
  "apple", "banana", "cherry", "date", "elderberry",
  "fig", "grape", "honeydew", "kiwi", "lemon",
  "mango", "nectarine", "orange", "papaya", "quince",
  "raspberry", "strawberry", "tangerine", "watermelon", "zucchini"
};
const int wordCount = sizeof(wordDictionary) / sizeof(wordDictionary[0]);

// RGB LED Class
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

// Button Class without function pointer
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

// Derived Button Classes
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
void checkInput();

// Instances
RGBLED rgbLed(9, 10, 11); // Example pins for RGB LED
StartStopButton startStopButton(STARTSTOPBUTTONPIN, FALLING, 100);
DifficultyButton difficultyButton(DIFFICULTYBUTTONPIN, FALLING, 100);

// Timer1 ISR for word generation
ISR(TIMER1_COMPA_vect) {
  generateRandomWord();
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

// Update difficulty message and timer configuration
void updateDifficulty() {
  const char* difficultyNames[] = {"Easy mode on!", "Medium mode on!", "Hard mode on!"};
  Serial.println(difficultyNames[chosenDifficulty]);

  uint16_t timerInterval;
  switch (chosenDifficulty) {
    case EASY:   timerInterval = 31250; break; // Approx. 2 seconds
    case MEDIUM: timerInterval = 23438; break; // Approx. 1.5 seconds
    case HARD:   timerInterval = 15625; break; // Approx. 1 second
  }

  cli(); // Disable global interrupts
  TCCR1A = 0; // Reset Timer1 control register A
  TCCR1B = 0; // Reset Timer1 control register B
  TCNT1 = 0;  // Initialize counter value to 0
  OCR1A = timerInterval; // Set compare match register
  TCCR1B |= (1 << WGM12); // Configure timer1 for CTC mode
  TCCR1B |= (1 << CS11);  // Prescaler 8, adjust as needed
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt
  sei(); // Enable global interrupts
}

// Start Countdown Function
void startCountdown() {
  countdownActive = true;
  countdownStartTime = millis();
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
  updateDifficulty(); // Configure timer for word interval based on difficulty
}

// Random Word Generation Function
void generateRandomWord() {
  int randomIndex = random(0, wordCount); // Random index from 0 to wordCount - 1
  currentWord = wordDictionary[randomIndex];
  Serial.print("Type this word: ");
  Serial.println(currentWord);
}

// Check Input Function
void checkInput() {
  if (userInput.equals(currentWord)) {
    score++;
    rgbLed.turnGreen();
    generateRandomWord();
    userInput = ""; // Clear user input after correct word
  } else {
    rgbLed.turnRed(); // Turn red if incorrect
  }
}

// Setup and Main Loop
void setup() {
  Serial.begin(9600);
  rgbLed.turnWhite(); // Initial idle state
  updateDifficulty(); // Set initial difficulty level and configure timer
  
  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(STARTSTOPBUTTONPIN), startStopButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(DIFFICULTYBUTTONPIN), difficultyButtonISR, FALLING);
}

void loop() {
  // Check if button presses need handling
  if (startStopButton.handleButtonPressNextTick) startStopButton.onPress();
  if (difficultyButton.handleButtonPressNextTick) difficultyButton.onPress();

  // Countdown logic
  if (countdownActive) {
    if (millis() - countdownStartTime >= COUNTDOWN_DURATION_MS) {
      startRound(); // Start the round after countdown
    }
  }

  // Round logic
  if (gameStarted && (millis() - roundStartTime >= ROUND_DURATION_MS)) {
    gameStarted = false;
    rgbLed.turnWhite();
    Serial.print("Round ended! Score: ");
    Serial.println(score);
    TCCR1B &= ~(1 << CS11); // Stop Timer1
  }

  // Read serial input
  while (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') { // Enter key pressed
      checkInput();
    } else {
      userInput += ch; // Collect character input
    }
  }
}
