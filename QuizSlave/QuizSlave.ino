
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
//  if (radio.available()) readMessage;

  if (currentButtonState != buttonState && currentButtonState == LOW)
  {
    // Button has been pushed
    Serial.print("Réponse :");
    Serial.println(buzzerId);
    Serial.println("");
    sendAnswer();

    timeLight = millis();   
  }
  
  buttonState = currentButtonState;

  // TEMP
  if (millis() - timeLight > 200)  turnLightOff();
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


void sendAnswer()
{
  radio.stopListening();

  message messageToSend;
  messageToSend.sender = buzzerId;
  messageToSend.target = TARGET_MASTER;
  messageToSend.info = MSG_ANSWERED;
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

//================

void initLedAndButton()
{
    pinMode(pinLED, OUTPUT);
    pinMode(pinButton, INPUT);
    turnLightOff();
}

void turnLightOn()
{
  digitalWrite(pinLED, LOW);
}

void turnLightOff()
{
  digitalWrite(pinLED, HIGH);  
}

