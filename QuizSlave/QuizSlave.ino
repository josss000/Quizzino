
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

/**** Variables ****/

int buzzerMode;  // Stores the game status
int buttonState = HIGH;  // Last button state (default open)

//TEMP
int timeLight;

void setup() {

  Serial.begin(9600);

  myself = 13;
  
  initTransmission();
  initLedAndButton();

  //TEMP
  timeLight = millis();
  buzzerMode = MODE_PLAYING;

}

//====================

void loop() {

  int currentButtonState;
  message receivedMessage;
    
  // check button state : pushed or not ?
  currentButtonState = digitalRead(pinButton);

  // check if master sends a message
  if (radio.available()) readMessage;

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

void readMessage()
{
  message receivedMessage;
          Serial.println("Message reçu");

  while (radio.available())
  {
      radio.read( &receivedMessage, sizeof(receivedMessage));
  }
  
  if (receivedMessage.target != TARGET_MASTER) 
  {
    Serial.println("C'est pas pour moi, j'me casse");
    return;
  } // Message is not for me
  else Serial.println("C'est pour moi");
    
  switch (receivedMessage.info)
  {
    case MSG_NEW_ID:
    // buzzerReceives its Id
      myself = receivedMessage.info;
      break;
      
    case MSG_BUZZER_SIGNED_IN:
      // Nothing much to do -> if not received, register & timeout ? //TODO
      break;
      
    case MSG_GAME_ON:
      // Sets Playing mode : if the user pushes the buzzer, a message is sent to the master
      turnLightOff(pinLED);
      buzzerMode = MODE_PLAYING;
      break;
      
    case MSG_FIRST_ANSWER:
      // The buzzer has been pushed first. Lights are on
      turnLightOn(pinLED);
      break;
      
    case MSG_WRONG_ANSWER:
      // Answer is wrong, buzzer is Locked
      buzzerMode = MODE_LOCKED;
      break;
  }
}

