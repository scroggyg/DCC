#include <NmraDcc.h>
#include "config.h"
#include "PinPulser.h"
#include "Adafruit_MCP23008.h"      // I2C library

// This Example shows how to use the library as a DCC Accessory Decoder to drive 8 Pulsed Turnouts

// You can print every DCC packet by un-commenting the line below
#define NOTIFY_DCC_MSG

// You can print every notifyDccAccTurnoutOutput call-back by un-commenting the line below
#define NOTIFY_TURNOUT_MSG

// You can also print other Debug Messages uncommenting the line below
#define DEBUG_MSG

// Un-Comment the line below to force CVs to be written to the Factory Default values
// defined in the FactoryDefaultCVs below on Start-Up
//#define FORCE_RESET_FACTORY_DEFAULT_CV

// Un-Comment the line below to Enable DCC ACK for Service Mode Programming Read CV Capablilty 
//#define ENABLE_DCC_ACK  15  // This is A1 on the Iowa Scaled Engineering ARD-DCCSHIELD DCC Shield

// Un-Comment the line below to Enable DCC Learning
//#define DCC_LEARN

#define NUM_TURNOUTS 8              // Set Number of Turnouts (Pairs of Pins)
#define ACTIVE_OUTPUT_STATE HIGH// Set the ACTIVE State of the output to Drive the Turnout motor electronics HIGH or LOW 

#define DCC_DECODER_VERSION_NUM 11  // Set the Decoder Version - Used by JMRI to Identify the decoder

struct CVPair
{
  uint16_t  CV;
  uint8_t   Value;
};

#define DCC_PIN 6 // Currently set DCC input pin
#define DCCINT 2 // Selected interrupt
#define CV_ACCESSORY_DECODER_OUTPUT_PULSE_TIME 2  // CV for the Output Pulse ON ms
#define CV_ACCESSORY_DECODER_CDU_RECHARGE_TIME 3  // CV for the delay in ms to allow a CDU to recharge
#define CV_ACCESSORY_DECODER_ACTIVE_STATE      4  // CV to define the ON Output State 

// To set the Turnout Addresses for this board you need to change the CV values for CV1 (CV_ACCESSORY_DECODER_ADDRESS_LSB) and 
// CV9 (CV_ACCESSORY_DECODER_ADDRESS_MSB) in the FactoryDefaultCVs structure below. The Turnout Addresses are defined as: 
// Base Turnout Address is: ((((CV9 * 64) + CV1) - 1) * 4) + 1 
// With NUM_TURNOUTS 8 (above) a CV1 = 1 and CV9 = 0, the Turnout Addresses will be 1..8, for CV1 = 2 the Turnout Address is 5..12

CVPair FactoryDefaultCVs [] =
{
  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        1},    // CV 1 Board Address (lower 6 bits) 
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        3},    // CV 1 Board Address (lower 6 bits)  = 9
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        5},    // CV 1 Board Address (lower 6 bits)  = 17
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        7},    // CV 1 Board Address (lower 6 bits)  = 25
// {CV_ACCESSORY_DECODER_ADDRESS_LSB,        9},    // CV 1 Board Address (lower 6 bits)  = 33
// {CV_ACCESSORY_DECODER_ADDRESS_LSB,        11},    // CV 1 Board Address (lower 6 bits)  = 41  
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        13},    // CV 1 Board Address (lower 6 bits)  = 49
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        15},    // CV 1 Board Address (lower 6 bits)  = 57
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        17},    // CV 1 Board Address (lower 6 bits)  = 65
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        19},    // CV 1 Board Address (lower 6 bits)  = 73
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        21},    // CV 1 Board Address (lower 6 bits)  = 81
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        23},    // CV 1 Board Address (lower 6 bits)  = 89
//  {CV_ACCESSORY_DECODER_ADDRESS_LSB,        25},    // CV 1 Board Address (lower 6 bits)  = 97
  {CV_ACCESSORY_DECODER_ADDRESS_MSB,        0},   // CV 9 Board Address (Upper 3 bits)
  {CV_ACCESSORY_DECODER_OUTPUT_PULSE_TIME, 50},   // x 10mS for the output pulse duration
  {CV_ACCESSORY_DECODER_CDU_RECHARGE_TIME, 30},   // x 10mS for the CDU recharge delay time
  {CV_ACCESSORY_DECODER_ACTIVE_STATE,    ACTIVE_OUTPUT_STATE},
};

CVPair EEPROMCVs [] =
{
  {CV_ACCESSORY_DECODER_ADDRESS_LSB,       (EEPROM.read(ADDRESS_LSB))},    // CV 1 Board Address (lower 6 bits) 
  {CV_ACCESSORY_DECODER_ADDRESS_MSB,       (EEPROM.read(ADDRESS_MSB))},   // CV 9 Board Address (Upper 3 bits)
  {CV_ACCESSORY_DECODER_OUTPUT_PULSE_TIME, (EEPROM.read(PULSE_TIME))},   // x 10mS for the output pulse duration
  {CV_ACCESSORY_DECODER_CDU_RECHARGE_TIME, (EEPROM.read(RECHARGE_TIME))},   // x 10mS for the CDU recharge delay time
  {CV_ACCESSORY_DECODER_ACTIVE_STATE,      (EEPROM.read(ACTIVE_STATE))},
};

uint8_t FactoryDefaultCVIndex = 0;
uint8_t EEPROMCVIndex = 0;
// Defaults used in serial commands to allow setting of values
uint8_t recharge_time= 30;
uint8_t pulse_time=50;
uint8_t address_LSB = 1;
uint8_t address_MSB = 0;
uint16_t verno=11;
uint16_t serno=1;
// Values to control the status LED
uint16_t status_ontime = 500;
uint16_t status_offtime = 500;
unsigned long next_status = status_ontime;
bool status_led = true;

bool active_state = HIGH;
uint8_t debug_lvl = 1;

// This is the Arduino Pin Mapping to Turnout Addresses with 2 pins per turnout 
// A1 is missing in the sequence as it is used for the DCC ACK
// The Pins are defined in Pairs T=Thrown, C=Closed (Digitrax Notation)  
//   base address 1T 1C 2T 2C 3T 3C 4T  4C  5T  5C  6T  6C  7T  7C  8T   8C
//byte outputs[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19};
//   pins         D3 D4 D5 D6 D7 D8 D9 D10 D11 D12 D13  A0  A2  A3  A4  A5 
byte outputs[] = { ENT1A, ENT1B,ENT2A, ENT2B, ENT3A, ENT3B,ENT4A, ENT4B, ENT5A, ENT5B, ENT6A, ENT6B, ENT7A, ENT7B,ENT8A, ENT8B} ;
//   pins         D3 D4 D5 D6 D7 D8 D9 D10 D11 D12 D13  A0  A2  A3  A4  A5  

Adafruit_MCP23008 mcp;
NmraDcc  Dcc ;
DCC_MSG  Packet ;
PinPulser pinPulser;
uint16_t BaseTurnoutAddress;

// This function is called whenever a normal DCC Turnout Packet is received
void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
{
#ifdef  NOTIFY_TURNOUT_MSG
  Serial.print("notifyDccAccTurnoutOutput: Turnout: ") ;
  Serial.print(Addr,DEC) ;
  Serial.print(" Direction: ");
  Serial.print(Direction ? "Closed" : "Thrown") ;
  Serial.print(" Output: ");
  Serial.print(OutputPower ? "On" : "Off") ;
#endif
  if(( Addr >= BaseTurnoutAddress ) && ( Addr < (BaseTurnoutAddress + NUM_TURNOUTS )) && OutputPower )
  {
    uint16_t pinIndex = ( (Addr - BaseTurnoutAddress) << 1 ) + Direction ;
    pinPulser.addPin(outputs[pinIndex]);
#ifdef  NOTIFY_TURNOUT_MSG
    Serial.print(" Pin Index: ");
    Serial.print(pinIndex,DEC);
    Serial.print(" Pin: ");
    Serial.print(outputs[pinIndex],DEC);
#endif
  }
#ifdef  NOTIFY_TURNOUT_MSG
  Serial.println();
#endif
}

void initPinPulser(void)
{
  BaseTurnoutAddress = (((Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_MSB) * 64) + Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_LSB) - 1) * 4) + 1  ;
   
  uint16_t onMs              = Dcc.getCV(CV_ACCESSORY_DECODER_OUTPUT_PULSE_TIME) * 10;
  uint16_t cduRechargeMs     = Dcc.getCV(CV_ACCESSORY_DECODER_CDU_RECHARGE_TIME) * 10;
  uint8_t  activeOutputState = Dcc.getCV(CV_ACCESSORY_DECODER_ACTIVE_STATE);

#ifdef DEBUG_MSG
  Serial.print("initPinPulser: DCC Turnout Base Address: "); Serial.print(BaseTurnoutAddress, DEC);
  Serial.print(" Active Pulse: "); Serial.print(onMs);  
  Serial.print("ms CDU Recharge: "); Serial.print(cduRechargeMs);
  Serial.print("ms Active Output State: "); Serial.println(activeOutputState ? "HIGH" : "LOW" );
#endif  

  // Step through all the Turnout Driver pins setting them to OUTPUT and NOT Active State
  for(uint8_t i = 0; i < (NUM_TURNOUTS * 2); i++)
  {
  	digitalWrite(outputs[i], !activeOutputState); // Set the Output Inactive before the direction so the 
  	pinMode( outputs[i], OUTPUT );                // Pin doesn't momentarily pulse the wrong state
	}

  // Init the PinPulser with the new settings 
  pinPulser.init(onMs, cduRechargeMs, activeOutputState);
}
// Routine to show status
void show_status()
{
unsigned long current_time;
current_time = millis();
if (current_time > next_status)
  {
    if (status_led == true)
    { status_led = false;
      next_status = current_time+status_offtime;
//      Serial.print(" Off Time: "); Serial.print(current_time);  
      digitalWrite(amber,HIGH); }
    else
    { status_led = true;
      next_status = current_time+status_ontime;
//      Serial.print(" On Time: "); Serial.println(current_time);  
      digitalWrite(amber,LOW); }    
  }
}
void setup()
{
  Serial.begin(115200);
  init_HW();
// Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up 
 // Dcc.pin(DCCINT, DCC_PIN,1);
    Dcc.pin(digitalPinToInterrupt(DCC_PIN), DCC_PIN, 1);
// Call the main DCC Init function to enable the DCC Receiver
//  Dcc.init( MAN_ID_DIY, DCC_DECODER_VERSION_NUM, CV29_ACCESSORY_DECODER, 0 );
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );
#ifdef DEBUG_MSG
  Serial.print("\nNMRA DCC 8-Turnout Accessory Decoder. Ver: "); Serial.println(DCC_DECODER_VERSION_NUM,DEC);
#endif  

#ifdef FORCE_RESET_FACTORY_DEFAULT_CV
  Serial.println("Resetting CVs to Factory Defaults");
  notifyCVResetFactoryDefault();
 #else
  Serial.println("Loading configuration from EEPROM"); 
  notifyCVEEPROM();
#endif

  if( FactoryDefaultCVIndex == 0)	// Not forcing a reset CV Reset to Factory Defaults so initPinPulser
	  initPinPulser();  
 #ifdef DCC_LEARN
 setup_LEARN();
 #endif
 next_status = millis()+status_ontime;        // LED is on set time for next check
}

void loop()
{
    if (Serial.available() >0) {  
    ParseCmd (Serial.read()); }
#ifdef DCC_LEARN
  static int learningbuttonOldval=0,learningbuttonVal=0;
#endif
  // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  Dcc.process();
  
  pinPulser.process();
  
  if( FactoryDefaultCVIndex && Dcc.isSetCVReady())
  {
    FactoryDefaultCVIndex--; // Decrement first as initially it is the size of the array
    uint16_t cv = FactoryDefaultCVs[FactoryDefaultCVIndex].CV;
    uint8_t val = FactoryDefaultCVs[FactoryDefaultCVIndex].Value;
#ifdef DEBUG_MSG
    Serial.print("loop: Write Default CV: "); Serial.print(cv,DEC); Serial.print(" Value: "); Serial.println(val,DEC);
#endif     
    Dcc.setCV( cv, val );
    
    if( FactoryDefaultCVIndex == 0)	// Is this the last Default CV to set? if so re-initPinPulser
	    initPinPulser();
  }
#ifdef DCC_LEARN
  // check if the learning button has been enabled
  learningbuttonVal=digitalRead(LEARNINGBUTTON);
  if (learningbuttonOldval!=learningbuttonVal) {
    learningMode=learningbuttonVal;
    if (learningMode==HIGH) showAcknowledge(3);
  }
  learningbuttonOldval=learningbuttonVal;
#endif
  show_status();
}

void notifyCVChange(uint16_t CV, uint8_t Value)
{
#ifdef DEBUG_MSG
  Serial.print("notifyCVChange: CV: ") ;
  Serial.print(CV,DEC) ;
  Serial.print(" Value: ") ;
  Serial.println(Value, DEC) ;
#endif  

  Value = Value;  // Silence Compiler Warnings...

  if((CV == CV_ACCESSORY_DECODER_ADDRESS_MSB) || (CV == CV_ACCESSORY_DECODER_ADDRESS_LSB) ||
		 (CV == CV_ACCESSORY_DECODER_OUTPUT_PULSE_TIME) || (CV == CV_ACCESSORY_DECODER_CDU_RECHARGE_TIME) || (CV == CV_ACCESSORY_DECODER_ACTIVE_STATE))
		initPinPulser();	// Some CV we care about changed so re-init the PinPulser with the new CV settings
}

void notifyCVResetFactoryDefault()
{
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset 
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs)/sizeof(CVPair);
};

void notifyCVEEPROM()
{
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset 
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
//  FactoryDefaultCVIndex = sizeof(EEPROMCVs)/sizeof(CVPair);
  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs)/sizeof(CVPair);
};

// This function is called by the NmraDcc library when a DCC ACK needs to be sent
// Calling this function should cause an increased 60ma current drain on the power supply for 6ms to ACK a CV Read 
#ifdef  ENABLE_DCC_ACK
void notifyCVAck(void)
{
#ifdef DEBUG_MSG
  Serial.println("notifyCVAck") ;
#endif
  // Configure the DCC CV Programing ACK pin for an output
  pinMode( ENABLE_DCC_ACK, OUTPUT );

  // Generate the DCC ACK 60mA pulse
  digitalWrite( ENABLE_DCC_ACK, HIGH );
  delay( 10 );  // The DCC Spec says 6ms but 10 makes sure... ;)
  digitalWrite( ENABLE_DCC_ACK, LOW );
}
#endif

#ifdef  NOTIFY_DCC_MSG
void notifyDccMsg( DCC_MSG * Msg)
{
  Serial.print("notifyDccMsg: ") ;
  for(uint8_t i = 0; i < Msg->Size; i++)
  {
    Serial.print(Msg->Data[i], HEX);
    Serial.write(' ');
  }
 Serial.print("Base Address: ") ; 
 Serial.print((((Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_MSB) * 64) + Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_LSB) - 1) * 4) + 1 );
 Serial.println();
}
#endif
