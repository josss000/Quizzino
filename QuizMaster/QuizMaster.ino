
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "Quizzino.h"
#include "Quizzino_Master.h"
#include "Quizzino_Common.h"

// Wiring
const int pinLEDBlue = 2;
const int pinLEDGreen = 3;
const int pinButton = 4;


/**** Variables ****/

int currentMode;  // Stores the game status

int buzzerCount;
int answersList[MAX_NB_ANSWERS];
int answersNb;

//TEMP
int timeLight;
int buttonState = HIGH;  // Last button state (default open)




//===========

void setup() {

  // Open Serial Interface for debugging
  Serial.begin(9600);

  myself = TARGET_MASTER;
  
  initTransmission();

  //  const int currentMode = MODE_REGISTER; // On startup, the master waits for the buzzer to register
  
  //TEMP
  pinMode(pinLEDBlue, OUTPUT);
  pinMode(pinLEDGreen, OUTPUT);
  buzzerCount = 2;
  timeLight = millis();
//  enterGameMode();
  buttonState = HIGH;
  
}

//=============

void loop() {
  // check if buzzer sends a message
  if (radio.available()) 
  {
    readMessage();
  }

  if ((timeLight - millis()) > 2000) {
    turnLightOff(pinLEDGreen);
    turnLightOff(pinLEDBlue);
    }
//  checkForUserInput();
}

void processMessage(message receivedMessage)
{
  switch (receivedMessage.info)
  {
    case MSG_REGISTER:
      // TODO feedback register and set Id
      break;
      
    case MSG_ANSWERED:
      Serial.println("QQn a répondu");
      sendMessage(receivedMessage.sender, MSG_FIRST_ANSWER);
      if (receivedMessage.sender == 1) turnLightOn(pinLEDBlue);
      else turnLightOff(pinLEDGreen);
      timeLight = millis();

      //if (answersNb < MAX_NB_ANSWERS) storeAnswer(receivedMessage.sender);
      break;
  } 
}


void storeAnswer(int buzzerId)
{
  answersNb++;
  answersList[answersNb] = buzzerId;
  //TODO : check if answer not already logged
  
  // only gives feedback for the first answer
  if(answersNb == 1) sendMessage(buzzerId, MSG_FIRST_ANSWER);
}



