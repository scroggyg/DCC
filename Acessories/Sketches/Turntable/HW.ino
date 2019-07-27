/*
 * Hardware Specific calls
 */


void init_HW()
{
  pinMode(ENT1A,OUTPUT);
  pinMode(ENT1B,OUTPUT);
  pinMode(ENT2A,OUTPUT);
  pinMode(ENT2B,OUTPUT);
  pinMode(ENT3A,OUTPUT);
  pinMode(ENT3B,OUTPUT);
  pinMode(ENT4A,OUTPUT);
  pinMode(ENT4B,OUTPUT);
  pinMode(ENT5A,OUTPUT);
  pinMode(ENT5B,OUTPUT);
  pinMode(ENT6A,OUTPUT);
  pinMode(ENT6B,OUTPUT);
  pinMode(ENT7A,OUTPUT);
  pinMode(ENT7B,OUTPUT);
  pinMode(ENT8A,OUTPUT);
  pinMode(ENT8B,OUTPUT);
  pinMode(amber,OUTPUT);
  pinMode(red,OUTPUT);
  pinMode(XP_DIR,OUTPUT);
  digitalWrite(ENT1A,LOW);
  digitalWrite(ENT1B,LOW);
  digitalWrite(ENT2A,LOW);
  digitalWrite(ENT2B,LOW);
  digitalWrite(ENT3A,LOW);
  digitalWrite(ENT3B,LOW);
  digitalWrite(ENT4A,LOW);
  digitalWrite(ENT4B,LOW);
  digitalWrite(ENT5A,LOW);
  digitalWrite(ENT5B,LOW);
  digitalWrite(ENT6A,LOW);
  digitalWrite(ENT6B,LOW);
  digitalWrite(ENT7A,LOW);
  digitalWrite(ENT7B,LOW);
  digitalWrite(ENT8A,LOW);
  digitalWrite(ENT8B,LOW);
  digitalWrite(amber,LOW);
  digitalWrite(red,HIGH);
  digitalWrite(XP_DIR,LOW);             // RS485 in listen mode
}
void I2CSetup()
  {
  mcp.begin();      // use default address 0
  for (uint8_t i =0;i<8;i++) {
    mcp.pinMode(i, OUTPUT); 
    mcp.digitalWrite(i,HIGH);}
  Serial.println(" I2C Initialisation complete");
  }
