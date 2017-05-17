
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

/**** Type ****/

typedef struct {
  int gameMode;
  int sender; // Who sends the message ? 0 = Master
  int target; // Who is the message for ? 0 = Master
  int message;
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
const byte CANAL_MASTER_TO_SLAVE[4] = "CM2S";
const byte CANAL_SLAVE_TO_MASTER[4] = "CS2M";

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

//TEMP
int timeLight;
int buttonState = HIGH;  // Last button state (default open)







char dataReceived[10]; // this must match dataToSend in the TX
bool newData = false;

//===========

void setup() {

    Serial.begin(9600);

    Serial.println("SimpleRx Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, CANAL_SLAVE_TO_MASTER);
    radio.startListening();
}

//=============

void loop() {
    getData();
    showData();
}

//==============

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;
    }
}

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(dataReceived);
        newData = false;
    }
}
