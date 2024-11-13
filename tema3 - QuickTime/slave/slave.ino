#include <SPI.h>

volatile char command = '\0';
volatile bool newCommand = false;

// Button pins for each color and each player
const int player1ButtonRedPin = 4;
const int player1ButtonGreenPin = A0;
const int player1ButtonBluePin = 2;

const int buzzerPin = 3;

const int player2ButtonRedPin = 5;
const int player2ButtonGreenPin = 6;
const int player2ButtonBluePin = 7;

// RGB LED pins for each color and each player
const int player1RedLEDPin = 8;
const int player1GreenLEDPin = 9;
const int player1BlueLEDPin = A2;

const int player2RedLEDPin = A5;
const int player2GreenLEDPin = A4;
const int player2BlueLEDPin = A3;

unsigned long pressStartTime = 0;
bool waitingForPress = false;
bool player1Turn = true;

void setup() {
  Serial.begin(115200);          // For debugging
  SPCR |= bit(SPE);               // Enable SPI in slave mode
  pinMode(MISO, OUTPUT);          // Set MISO as output for SPI
  SPI.attachInterrupt();          // Enable SPI interrupt
  
  // Initialize button pins as input with pull-up resistors
  pinMode(player1ButtonRedPin, INPUT_PULLUP);
  pinMode(player1ButtonGreenPin, INPUT_PULLUP);
  pinMode(player1ButtonBluePin, INPUT_PULLUP);
  pinMode(player2ButtonRedPin, INPUT_PULLUP);
  pinMode(player2ButtonGreenPin, INPUT_PULLUP);
  pinMode(player2ButtonBluePin, INPUT_PULLUP);

  // Initialize LED pins as outputs
  pinMode(player1RedLEDPin, OUTPUT);
  pinMode(player1GreenLEDPin, OUTPUT);
  pinMode(player1BlueLEDPin, OUTPUT);
  pinMode(player2RedLEDPin, OUTPUT);
  pinMode(player2GreenLEDPin, OUTPUT);
  pinMode(player2BlueLEDPin, OUTPUT);
}

// SPI interrupt service routine
ISR(SPI_STC_vect) {
  char receivedChar = SPDR;
  if (receivedChar != '#') {
    command = receivedChar;
    newCommand = true;
  }
}

void loop() {
  if (newCommand) {
    newCommand = false;

    if (command == 'r' || command == 'g' || command == 'b') {
      startButtonChallenge(command);  // Start a new round with color
    } else {
      SPDR = '$';  // Send default response for unrecognized commands
    }
  }

  if (waitingForPress) {
    checkButtonResponse();
  }
}

void startButtonChallenge(char color) {
  Serial.print("Received: ");
  Serial.print((char)SPDR);
  Serial.println(player1Turn ? " for p1" : " for p2");
  pressStartTime = millis();
  waitingForPress = true;

  activateLED(color);  // Activate LED for the current player based on color
}

void checkButtonResponse() {
  unsigned long elapsedTime = millis() - pressStartTime;
  
  int buttonPin;
  if (player1Turn) {
    buttonPin = (command == 'r') ? player1ButtonRedPin :
                (command == 'g') ? player1ButtonGreenPin :
                player1ButtonBluePin;
  } else {
    buttonPin = (command == 'r') ? player2ButtonRedPin :
                (command == 'g') ? player2ButtonGreenPin :
                player2ButtonBluePin;
  }

  if (digitalRead(buttonPin) == HIGH) { 
    waitingForPress = false;
    char score;

    if (elapsedTime <= 300) {
      score = 'a';  // Fast response
    } else if (elapsedTime <= 600) {
      score = 'b';  // Moderate response
    } else if (elapsedTime <= 1000) {
      score = 'c';  // Slow response
    } else {
      score = 'i';  // Missed timing
    }
    
    SPDR = score;  // Send score back via SPI
    if(score != 'i')
      tone(buzzerPin, 659, 200);

    resetLEDs();
    player1Turn = !player1Turn;  // Switch player turns
  } else if (elapsedTime > 900) {  // Timeout condition
    waitingForPress = false;
    SPDR = 'i';  // Send timeout indicator
    resetLEDs();
    player1Turn = !player1Turn;  // Switch player turns
  }
  
  if((player1Turn && (digitalRead(player1ButtonBluePin) == HIGH || digitalRead(player1ButtonRedPin) == HIGH || digitalRead(player1ButtonGreenPin) == HIGH))
  ||
    (!player1Turn && (digitalRead(player2ButtonBluePin) == HIGH || digitalRead(player2ButtonRedPin) == HIGH || digitalRead(player2ButtonGreenPin) == HIGH))) {
    
    tone(buzzerPin, 220, 200);
    resetLEDs();
  }

  
}

void activateLED(char color) {
  resetLEDs();  // Ensure other LEDs are off
  int ledPin;
  
  if (player1Turn) {
    ledPin = (color == 'r') ? player1RedLEDPin :
             (color == 'g') ? player1GreenLEDPin :
             player1BlueLEDPin;
  } else {
    ledPin = (color == 'r') ? player2RedLEDPin :
             (color == 'g') ? player2GreenLEDPin :
             player2BlueLEDPin;
  }

  digitalWrite(ledPin, HIGH);  // Turn on the appropriate color LED for the player
}

void resetLEDs() {
  digitalWrite(player1RedLEDPin, LOW);
  digitalWrite(player1GreenLEDPin, LOW);
  digitalWrite(player1BlueLEDPin, LOW);
  digitalWrite(player2RedLEDPin, LOW);
  digitalWrite(player2GreenLEDPin, LOW);
  digitalWrite(player2BlueLEDPin, LOW);
}