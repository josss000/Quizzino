
// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/**** Type ****/

typedef struct {
  int gameMode;
  int sender; // Who sends the message ? 0 = Master
  int target; // Who is the message for ? 0 = Master
  int info;
} message;

/**** Constants ****/

// Buzzer Id
int buzzerId = 1;

// Wiring
const int pinCE = 9;
const int pinCSN = 10;

const int pinLED = 2;
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
const int MSG_ANSWERED = 7;           // Buzzer sends answer

// Targets
const int TARGET_MASTER = 0;
const int TARGET_ALL = -1;

/**** Variables ****/

RF24 radio(pinCE,pinCSN); // Set up nRF24L01 radio on SPI bus pins

int buzzerMode;  // Stores the game status
int buttonState = HIGH;  // Last button state (default open)
message sendMessage;

//TEMP
int timeLight;











char dataToSend[10] = "Message 0";
char txNum = '0';


unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second



void setup() {

    Serial.begin(9600);
    initTransmission();
    
// init Msg
  sendMessage.sender = buzzerId;
  sendMessage.target = 0;
  sendMessage.info = MSG_ANSWERED;
  
    Serial.println("SimpleTx Starting");
}

//====================

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
}

void initTransmission()
{
  Serial.println("SimpleTx Starting");
  
  // Begin Radio Transmission
  radio.begin();
//  radio.setPayloadSize(PAYLOAD_SIZE); // We don't need to transmit a lot of data
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5); // delay, count
  
  // Open Emitting / Receiving transmission canals
  // This is the Master code, so :
  //   -  It writes on the Master -> Slave channel
  //   -  It listens on the Slave -> Master channel
  radio.openWritingPipe(CANAL_SLAVE_TO_MASTER);
  radio.openReadingPipe(1,CANAL_MASTER_TO_SLAVE);

  radio.startListening(); 
}

//====================

void send() {

    radio.stopListening(); 

    bool rslt;
    rslt = radio.write( &sendMessage, sizeof(sendMessage) );
//    rslt = radio.write( &dataToSend, sizeof(dataToSend) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print("Data Sent ");
    Serial.print(sendMessage.sender);
//    Serial.print(dataToSend);

    if (rslt) {
        Serial.println("  Acknowledge received");
        updateMessage();
    }
    else {
        Serial.println("  Tx failed");
    }

    radio.startListening(); 
}

//================

void updateMessage() {
        // so you can see that new data is being sent
    txNum += 1;
    if (txNum > '9') {
        txNum = '0';
    }
    dataToSend[8] = txNum;
    sendMessage.sender = txNum - '0';
}

