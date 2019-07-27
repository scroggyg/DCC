
void clear_eeprom ()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0xff);
  }
}
/*
void read_eeprom()
{
  verno =(( EEPROM.read(ver_add)) | (( EEPROM.read(ver_add+1 ) << 8)));
  serno =(( EEPROM.read(ser_add)) | (( EEPROM.read(ser_add+1 ) << 8))) ;
  verno =(( EEPROM.read(SERIAL_LSB)) | (( EEPROM.read(SERIAL_LSB+1 ) << 8)));
  serno =(( EEPROM.read(ser_add)) | (( EEPROM.read(ser_add+1 ) << 8))) ;
  serno =(( EEPROM.read(ser_add)) | (( EEPROM.read(ser_add+1 ) << 8))) ;
  ACTIVE_STATE = ( EEPROM.read(ACTIVE_STATE));
  debug_lvl=( EEPROM.read(debug_add));
  EEPROM.get(0x200,cardtable);
  Load_key();                           // Load all the fixed keys  
}
*/
void store_eeprom()
{
  EEPROM.update (ver_add, (verno & 0xff)) ;
  EEPROM.update (ver_add +1, ((verno & 0xff00) >>8)) ;
  EEPROM.update (ser_add, (serno & 0xff)) ;
  EEPROM.update (ser_add +1, ((serno & 0xff00) >>8)) ;
  EEPROM.update (ADDRESS_LSB, (address_LSB)) ;
  EEPROM.update (ADDRESS_MSB, (address_MSB)) ;
  EEPROM.update (RECHARGE_TIME, (recharge_time)) ;
  EEPROM.update (PULSE_TIME, (pulse_time)) ; 
  EEPROM.update (ACTIVE_STATE, (active_state)) ;    
  EEPROM.update (debug_add, debug_lvl);
}

void dump_eeprom()
{
  Serial.print("00 ");
  for (int i = 0 ; i < 32 ; i++) { //EEPROM.length() ; i++) {
    Serial.print(EEPROM.read(i), HEX);
    Serial.print(" ");
    if ((i & 0x01f)== 0x1f) {
    Serial.println(""); 
    Serial.print(i+1, HEX);
    Serial.print(" ");
    }
  }
}
