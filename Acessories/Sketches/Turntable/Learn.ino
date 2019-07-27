#define DCC_LEARN
/**
 * this is just a function to show via the onboard PCB led, the state of the decoder
 */
void showAcknowledge(int nb) {
  for (int i=0;i<nb;i++) {
    digitalWrite(red, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(red, LOW);    // turn the LED off by making the voltage LOW
    delay(100);               // wait for a second
  }
}
/*
 * standard arduino initialisation function
 * we initialize lights, button, and relay pins
 */
void setup_LEARN() {
  // put your setup code here, to run once:
  pinMode(LEARNINGBUTTON,INPUT_PULLUP); 
  Serial.println("Address Learn enabled");
  showAcknowledge(3);  // Flash 3 times

}
