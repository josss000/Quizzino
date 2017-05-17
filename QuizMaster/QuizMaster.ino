
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

    Serial.begin(9600);
    initTransmission();
}

//=============

void loop() {
    getData();
    showData();
}

void initTransmission()
{
  Serial.println("SimpleRx Starting");
  
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
//==============

void getData() {
    if ( radio.available() ) {
        radio.read( &receivedMessage, sizeof(receivedMessage) );
        newData = true;
    }
}

void showData() {
    if (newData == true) {
        Serial.print("Message From : ");
        Serial.println(receivedMessage.sender);
        Serial.print("Info : ");
        Serial.println(receivedMessage.info);
        Serial.print("Target : ");
        Serial.println(receivedMessage.target);
        Serial.print("GameMode : ");
        Serial.println(receivedMessage.gameMode);
        newData = false;
    }
}
