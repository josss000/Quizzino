
// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"
#include "Quizzino.h"
#include "Quizzino_Common.h"

// Wiring
const int pinLED = 2;
const int pinButton = 4;
const int pinBuzzId = 6;
/**** Variables ****/

int buzzerMode;  // Stores the game status
int buttonState = HIGH;  // Last button state (default open)

//TEMP
int timeLight;
const int pinID = 6;

void setup() {

  Serial.begin(9600);

  myself = 13;
  
  initTransmission();
  initLedAndButton();

  //TEMP
  pinMode(pinButton, INPUT);
  timeLight = millis();
  buzzerMode = MODE_PLAYING;
  pinMode(pinBuzzId, INPUT);

  if (digitalRead(pinBuzzId) == HIGH)  myself = 1;
  else myself = 2;
  Serial.print("BuzzerId : ");
  Serial.println(myself);
}

//====================

void loop() {

  // check if master sends a message
  if (radio.available()) 
  {
    readMessage();
  }
  int currentButtonState;
    
  // check button state : pushed or not ?
  currentButtonState = digitalRead(pinButton);

  if (currentButtonState != buttonState && currentButtonState == LOW)
  {
    // Button has been pushed
    Serial.print("Réponse :");
    Serial.println(myself);
    Serial.println("");
    sendMessage(TARGET_MASTER, MSG_ANSWERED);
    
    timeLight = millis();   
  }
  
  buttonState = currentButtonState;

  // TEMP
  if (millis() - timeLight > 200)  turnLightOff(pinLED);
}

void initLedAndButton()
{
    pinMode(pinLED, OUTPUT);
    pinMode(pinButton, INPUT);
    turnLightOff(pinLED);
}

void processMessage(message receivedMessage)
{
  switch (receivedMessage.info)
  {
    case MSG_NEW_ID:
      Serial.println("New ID");
      // buzzerReceives its Id
      myself = receivedMessage.info;
      break;
      
    case MSG_BUZZER_SIGNED_IN:
      Serial.println("Signed in");
      // Nothing much to do -> if not received, register & timeout ? //TODO
      break;
      
    case MSG_GAME_ON:
      Serial.println("Gameon");
      // Sets Playing mode : if the user pushes the buzzer, a message is sent to the master
      turnLightOff(pinLED);
      buzzerMode = MODE_PLAYING;
      break;
      
    case MSG_FIRST_ANSWER:
      Serial.println("First");
      // The buzzer has been pushed first. Lights are on
      turnLightOn(pinLED);
      break;
      
    case MSG_WRONG_ANSWER:
      Serial.println("Wrong");
      // Answer is wrong, buzzer is Locked
      buzzerMode = MODE_LOCKED;
      break;
    default:
      
      Serial.println("Oups");
      break;
  }
}

