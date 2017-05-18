
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

/**** Type ****/

typedef struct {
  int gameMode;
  int sender; // Who sends the message ? 0 = Master
  int target; // Who is the message for ? 0 = Master
  int info;
} message;

/**** Constants ****/

// Config
const int MAX_NB_ANSWERS = 5;       // Max number of answers stored for each question

// Wiring
const int pinCE = 9;
const int pinCSN = 10;

const int pinLEDBlue = 2;
const int pinLEDGreen = 3;
const int pinButton = 4;

// RF24 Config
const int PAYLOAD_SIZE = 32;

// Transmission Canals
const byte CANAL_MASTER_TO_SLAVE[5] = "RFM2S";
const byte CANAL_SLAVE_TO_MASTER[5] = "RFS2M";

// Quiz modes
const int MODE_REGISTER = 1;    // Buzzer waits for the master to recognize it
const int MODE_PLAYING = 2;     // Buzzer is playing
const int MODE_LOCKED = 3;      // Buzzer is locked : no answer is sent

// Messages
const int MSG_INIT = 0;               // Master waits for buzzer to register
const int MSG_NEW_ID = 1;             // Master gives buzzer its id
const int MSG_BUZZER_SIGNED_IN = 2;   // Master sends feedback : buzzer has been recognized
const int MSG_GAME_ON = 3;            // Master waits for answer
const int MSG_FIRST_ANSWER = 4;       // Master says answer is correct : leds are lit
const int MSG_WRONG_ANSWER = 5;       // Master says answer is wrong : buzzer is locked
const int MSG_REGISTER = 6;           // Buzzer wants to register
const int MSG_ANSWERED = 7;           // Buzzer sends answeranswer

// Targets
const int TARGET_MASTER = 0;
const int TARGET_ALL = -1;

/**** Variables ****/
int currentMode;  // Stores the game status
RF24 radio(pinCE,pinCSN); // Set up nRF24L01 radio on SPI bus pins
int buzzerCount;
int answersList[MAX_NB_ANSWERS];
int answersNb;
message receivedMessage;

//TEMP
int timeLight;
int buttonState = HIGH;  // Last button state (default open)






bool newData = false;

//===========

void setup() {

  // Open Serial Interface for debugging
  Serial.begin(9600);

  initTransmission();

  //  const int currentMode = MODE_REGISTER; // On startup, the master waits for the buzzer to register
  
  //TEMP
  buzzerCount = 2;
  timeLight = millis();
//  enterGameMode();
  buttonState = HIGH;
  
}

//=============

void loop() {
  // check if buzzer sends a message
  if (radio.available()) readMessage();

//  checkForUserInput();
}

void initTransmission()
{
  Serial.println("Quizino Master Starting");
  
  // Begin Radio Transmission
  radio.begin();
//  radio.setPayloadSize(PAYLOAD_SIZE); // We don't need to transmit a lot of data
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(3,5); // delay, count

  // Open Emitting / Receiving transmission canals
  // This is the Master code, so :
  //   -  It writes on the Master -> Slave channel
  //   -  It listens on the Slave -> Master channel
  radio.openWritingPipe(CANAL_MASTER_TO_SLAVE);
  radio.openReadingPipe(1,CANAL_SLAVE_TO_MASTER);

  radio.startListening(); 
}


void turnLightOn(int pin)
{
  digitalWrite(pin, LOW);
}

void turnLightOff(int pin)
{
  digitalWrite(pin, HIGH);
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
    case MSG_REGISTER:
      // TODO feedback register and set Id
      break;
      
    case MSG_ANSWERED:
      Serial.println("QQn a répondu");
      if (answersNb < MAX_NB_ANSWERS) storeAnswer(receivedMessage.sender);
      break;
  }
}

bool sendMessage(int gameMode, int target, int msg)
{
  Serial.println("---");
  Serial.println("Envoi du message");
  radio.stopListening();

  message messageToSend;
  messageToSend.sender = TARGET_MASTER;
  messageToSend.target = target;
  messageToSend.info = msg;
  bool ok = radio.write( &messageToSend, sizeof(messageToSend) );
  
  radio.startListening();

  Serial.print("Message From : ");
  Serial.println(messageToSend.sender);
  Serial.print("Info : ");
  Serial.println(messageToSend.info);
  Serial.print("Target : ");
  Serial.println(messageToSend.target);
  Serial.print("GameMode : ");
  Serial.println(messageToSend.gameMode);
  Serial.println("---");
  Serial.print("Résultat : ");
  if(ok) Serial.println("Envoyé");
  else Serial.println("Raté");
  Serial.println("---");

}


void storeAnswer(int buzzerId)
{
  answersNb++;
  answersList[answersNb] = buzzerId;
      
  // only gives feedback for the first answer
  if(answersNb == 1) sendMessage(MODE_PLAYING, buzzerId, MSG_FIRST_ANSWER);
}



