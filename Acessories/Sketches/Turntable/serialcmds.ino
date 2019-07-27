
byte setlower(byte CmdByte)
{
    if ((CmdByte >= 65) && (CmdByte <90)) CmdByte = CmdByte + 32 ; // change to lower case
    return(CmdByte);
}

byte sethigh(byte CmdByte)
{
    if ((CmdByte >= 97) && (CmdByte <122)) CmdByte = CmdByte - 32 ; // change to upper case
    return(CmdByte);
}

void flushserial()
{
while(Serial.available() > 0)
  {
  byte temp = Serial.read() ; // flush down buffer
  } 
}

uint32_t getint(uint16_t waittime)
{
      Serial.print ("> ");  
      flushserial();
      Serial.setTimeout(waittime) ;
      return (Serial.parseInt());
}

void testcr()
{
  byte i = Serial.peek() ;
  if (i < 0x20)                 // control character
  {
      i = Serial.read() ;       // flush down buffer      
  }
}
byte charwait( int numchar, unsigned long period)
{
  unsigned long waittime = period/10 ;
  while (Serial.available() < numchar) { // wait for input
    delay (10);
    waittime -- ;
    if (waittime == 0) {        // stop it looping forever
      return(0) ;
      }
  }
      return (1) ;
}
void defaultscrn()
{
#ifdef DEBUG_MSG
  Serial.print("\nNMRA DCC 8-Turnout Accessory Decoder. Ver: "); Serial.println(DCC_DECODER_VERSION_NUM,DEC);
  Serial.println(F(" 15th May 2017"));
#endif 
  Serial.println("Loading configuration from EEPROM"); 
  notifyCVEEPROM();
  initPinPulser();
 }
/*
  if (debug_lvl && 0x08) {
    clear_tft();
    switch (mode) {
    case ('c'):
      tft.println(F("LumenID Panel PoC Revision 1"));
      break; 
    case('p'):
      tft.println(F("Printer Mode"));
      break;
    default :                 // normally 'T' or blank eeprom
      tft.println(F("Setup Mode"));
      break;
    }
    tft.print(F("Sno.= "));
    tft.print(serno) ;
    tft.print(F(" Ver= "));
    tft.println(verno) ;
    tft.print(F("Debug= "));
    tft.println(debug_lvl) ;
    tft.print(F("Built 15/05/2017"));
}
}
/*
void set_led_result(char Led, int period, int on_time, int off_time, uint8_t ptr)
{
  uint8_t red =0;
  uint8_t green=0;
  uint8_t amber=0;
 switch(ptr) {
  case(1):
    red=rLED1;
    green=gLED1;
    amber=aLED1;    
  break;
  case(2):
    red=rLED2;
    green=gLED2;
    amber=aLED2;   
  break;
  case(3):
    red=rLED3;
    green=gLED3;
    amber=aLED3; 
  break;
  case(4):
    red=rLED4;
    green=gLED4;
    amber=aLED4; 
  break;  
 }
 switch(Led) {
  case('r'):
    set_output(red,period,on_time,off_time);
    set_output(green,0,0,0);        // Turn off green
    set_output(amber,0,0,0);        // Turn off Amber 
  break;
  case('g'):
    set_output(green,period,on_time,off_time);
    set_output(red,0,0,0);        // Turn off green
    set_output(amber,0,0,0);        // Turn off Amber 
  break;
  case('a'):
    set_output(amber,period,on_time,off_time);
    set_output(green,0,0,0);        // Turn off green
    set_output(red,0,0,0);        // Turn off Amber 
  break;
  default:
  break;  
 }
}

void show_status(uint8_t cstat, uint8_t ptr)
{
  tft.setCursor(0,16);
  switch (cstat) {
    case (0):
    Serial.println(F("Card Valid"));
    tft.println(F("Card Valid"));
    set_led_result('g',40,0,0,ptr);
    break;
    case (1):
    Serial.println(F("Close to Expiry"));
    tft.println(F("Close to Expiry"));
    set_led_result('g',40,5,5,ptr);
    break;
    case (2):
    Serial.println(F("Wrong TZ"));
    tft.println(F("Wrong TZ"));
    set_led_result('r',40,0,0,ptr);
    break;
    case (3):
    Serial.println(F("Lost & Stolen Card"));
    tft.println(F("Lost & Stolen Card"));
    set_led_result('r',40,0,0,ptr);
    break;
    default:
    Serial.println(F("Invalid Card"));
    tft.println(F("Invalid Card"));
    set_led_result('r',40,0,0,ptr);
    break;
 }
}

void show_cards()
{
  Serial.println(F("Card Table contents"));
  for (int i=0; i < maxcards; i++)
  {
    Serial.print(F(" len = "));
    Serial.print( cardtable[i].clen); 
    Serial.print(F(" Facility code "));
    Serial.print( cardtable[i].fac);
    Serial.print(F(" Variable "));
    Serial.print( cardtable[i].var);       
    Serial.print(F(" Outcome = "));
    show_status(cardtable[i].cstatus,1);  
  }
}

void load_cards()
{
  uint32_t temp;
  uint8_t ptr;
  Serial.println(F("Enter Card details"));
  Serial.print(F(" Card no. "));
  ptr = getint(10000);
  ptr = constrain(ptr,0,(maxcards-1));
  Serial.println(ptr) ;
  Serial.print(F(" len "));
  temp=getint(10000);
  cardtable[ptr].clen =   constrain(temp,26,44);
  Serial.println(cardtable[ptr].clen) ;
  Serial.print(F(" Facility "));
  cardtable[ptr].fac = getint(20000);
  Serial.println(cardtable[ptr].fac) ;
  Serial.print(F(" Variable "));
  cardtable[ptr].var = getint(20000);
  Serial.println(cardtable[ptr].var) ;
  Serial.print(F(" Outcome (0,1,2,3) "));
  temp=getint(10000);  
  cardtable[ptr].cstatus = constrain(temp,0,4);
  Serial.println(cardtable[ptr].cstatus) ;
  EEPROM.put(0x100,cardtable);
}

// Change to 8 relays
byte RelaySet() 
{
  byte outcome = 0;
  byte byteread = 0 ;
  outcome = charwait(8, 1000) ;    // wait for 10 seconds for 2 characters
  if (outcome == 0) return(0);
  byteread = Serial.read();     // First relay command
  if (byteread == '1') {
    set_output(Rl11,100,2,4);}
    if (debug_lvl && 0x01)
    {
      Serial.println("Relay1 set 10 secs 33% duty cycle");
    }
  else {
    set_output(Rl11,0,0,0); }
   byteread = Serial.read();     // Second relay command
  if (byteread == '1') {
        set_output(Rl21,100,0,0); }
  else {
    set_output(Rl21,0,0,0); }
    return (1) ;  
      byteread = Serial.read();     // Third relay command
  if (byteread == '1') {
    set_output(Rl12,100,0,0);}
  else {
    set_output(Rl12,0,0,0); }
   byteread = Serial.read();     // Fourth relay command
  if (byteread == '1') {
        set_output(Rl22,100,0,0); }
  else {
    set_output(Rl22,0,0,0); }
    return (1) ; 
  byteread = Serial.read();     // Fifth relay command
  if (byteread == '1') {
    set_output(Rl13,100,0,0);}
  else {
    set_output(Rl13,0,0,0); }
   byteread = Serial.read();     // Sixth relay command
  if (byteread == '1') {
        set_output(Rl23,100,0,0); }
  else {
    set_output(Rl23,0,0,0); }
    return (1) ;  
      byteread = Serial.read();     // Seventh relay command
  if (byteread == '1') {
    set_output(Rl14,100,0,0);}
  else {
    set_output(Rl14,0,0,0); }
   byteread = Serial.read();     // Eight & last relay command
  if (byteread == '1') {
        set_output(Rl24,100,0,0); }
  else {
    set_output(Rl24,0,0,0); }
    return (1) ;  
}
void ShowStatus()
{
  if (debug_lvl && 0x01)  Serial.print(F("Inputs  = "));   
  for (uint8_t i =0; i<(maxains-1);i++)
  {
    Serial.print( analogins[i].average);
    Serial.print(" ");
  }
  Serial.println(" ");
  if (debug_lvl && 0x01) Serial.print(F("4 state = "));
  for (uint8_t i =0; i<(maxains-1);i++)
  {
    Serial.print( analogins[i].four_state);
  }
  Serial.println(" ");
  if (debug_lvl && 0x01 )  Serial.println(F("Analog Input structures"));   
  for (uint8_t i =0; i<(maxains);i++)
  {
    Serial.print(" Sample buffer = ");
    for (uint8_t j=0; j < maxsamples; j++)
    {
      Serial.print( analogins[i].samples[j]);
      Serial.print("  ");
    }
    Serial.print(" Average Value = ");
    Serial.print( analogins[i].average);
    Serial.print(" Four state = ");
    Serial.print( analogins[i].four_state);
    Serial.print(" Valid Flag  ");
    Serial.println( analogins[i].valid);
  }
     Serial.print(" Sample ptr = ");
     Serial.println( ansample); 
  if (debug_lvl && 0x01) Serial.print(F("VFLT    = "));
  Serial.println(analogins[16].average);
  if (debug_lvl && 0x01) Serial.print(F("Relays  = "));
  Serial.print( timers[Rl11].pinstate);
  Serial.print( timers[Rl12].pinstate);
  Serial.print( timers[Rl21].pinstate);
  Serial.print( timers[Rl22].pinstate);
  Serial.print( timers[Rl13].pinstate);
  Serial.print( timers[Rl23].pinstate);
  Serial.print( timers[Rl14].pinstate);
  Serial.println( timers[Rl24].pinstate);
}

*/
void ConfigMenu()
{
  byte temp = 0;
 // tft.setCursor(0,0);
  Serial.println(F("Turnout DCC Configuration Menu")) ;
  Serial.print (F(" s - set serial number : "));
  Serial.println(serno);
  Serial.print (F(" v - set version number : " ));
  Serial.println(verno);  
  Serial.print (F(" l - set debug level 0 (off) 1(Serial), 2(Verbose), 4(OSDP), 8(TFT) : "));
  Serial.println(debug_lvl); 
  Serial.println (F(" a - Set (A)ddress"));  
  Serial.println (F(" p - (P)ulse time"));
  Serial.println (F(" r - (R)echarge time")); 
  Serial.println (F(" d - (D)ump EEPROM"));
  Serial.println (F(" e - (E)rase EEPROM Contents"));
  Serial.println (F(" m - redraw (M)enu"));  
  Serial.println (F(" q - (Q)uit and do not update EEPROM"));
  Serial.println (F(" x - save EEPROM and e(X)it"));
  Serial.println (F("Enter command")); 
  Serial.print (F("> "));  
  flushserial();
}

byte ParseConfig (int CmdByte)
{
  byte CmdStatus = 0;
  byte outcome = 1;
  uint32_t temp;
  CmdByte=(setlower(CmdByte)) ;
  switch (CmdByte) {
 /*
    case ('m'):
      testcr();
      Serial.println(" Enter operation mode t/c/p "); 
      outcome = charwait(1, 10000) ;
      if (outcome ==1)
       {
        temp = setlower(Serial.read());
        if ((temp == 't') || (temp == 'c') || (temp == 'p'))
        { mode = temp;}
       }
    break ;
 */
    case ('s'):
      testcr();
      Serial.print(F(" New Serial number ?"));
      temp = getint(10000);
      if (temp >0) {serno=temp;}
      Serial.println( serno) ;
    break ;
    case ('v'):
      testcr();
      Serial.println(F(" Enter firmware version ")); 
      temp =getint(10000);
      if (temp >0) {verno=temp;}
      Serial.println( verno) ;
    break ;
     case ('a'):
      testcr();
      Serial.print(F(" New Address number ?"));
      temp = getint(10000);
      if (temp >0) {address_LSB=temp &255;}
      Serial.println( address_LSB) ;
    break ;
    case ('r'):
      testcr();
      Serial.println(F(" Enter recharge time ")); 
      temp =getint(10000);
      if (temp >0) {recharge_time=temp & 255;}
      Serial.println( recharge_time) ;
    break ;
     case ('p'):
      testcr();
      Serial.println(F(" Enter pulse time ")); 
      temp =getint(10000);
      if (temp >0) {pulse_time=temp & 255;}
      Serial.println( pulse_time) ;
    break ;
    case ('l'):
      testcr();
      Serial.println(F(" New level 0-15 "));
      debug_lvl = getint(10000);
      debug_lvl = debug_lvl & 0x0f;
      Serial.println( debug_lvl) ;
    break;
/*
    case ('r'):
      ConfigMenu();
    break;
    case ('n'):
      NetworkMenu();
    break;
        case ('t'):
      show_cards();
    break;
    case ('a'):
      load_cards();
 */
    case ('d'):
      dump_eeprom();
    break;

    break;
    case ('e') :
       Serial.println (F("\n\rErasing EEPROM, Are you sure?")) ;
       testcr();
       CmdStatus = charwait(1, 10000) ;    // wait for 10 seconds for 1 character
       if ( CmdStatus == 1) {
       
        temp = setlower(Serial.read());
        if (temp == 'y')  {
          Serial.println (F("Erasing EEPROM")) ;
          clear_eeprom() ;
          Serial.println (F("EEPROM Erased")) ;  }     
       }
       else Serial.println (F("EEPROM Not changed")) ; 
       Serial.print ("> ");  
    break;
    case ('q'):
      Serial.println (F("\r\nExiting menu, not saved!")) ;
      outcome = 0;
    break ;
    case ('x'):
      store_eeprom();
      Serial.println (F("Exiting menu, EEPROM Updated!")) ;
      defaultscrn();
      outcome = 0;
      break;
    default:            // catch all
    break ;
  }
  return (outcome) ;
}

void set_config()
{
  byte outcome = 0;
  byte CmdByte = 0;
  ConfigMenu();
  do
  {
    outcome = 0;
    outcome = charwait(1, 30000) ;    // wait for 30 seconds for 1 character
    if (outcome == 1) 
    {
      CmdByte = Serial.read();
      if ((CmdByte >0x2f) && (CmdByte <0x80))
      {
      outcome = ParseConfig(CmdByte); }
    }
    else
    {
  Serial.println(F(" Menu Timed out "));        
    }
  }
  while (outcome == 1) ;
  Serial.print("\r"); 
  defaultscrn();
}


void ParseCmd (int CmdByte)
{
  byte CmdStatus = 0;
  CmdByte=(setlower(CmdByte)) ;
  switch (CmdByte) {
    case ('c'):
      set_config();
      break;
 /*
    case ('o'):
      OSDP_menu();
      break;
    case ('r') :
      if (debug_lvl && 0x01) Serial.println(F("Setting Relays"));
      CmdStatus= (RelaySet());
      if (CmdStatus == 0) {
        if (debug_lvl && 0x01) Serial.println(F("Setting timed out")); }
      else 
      {
        if (debug_lvl && 0x01) Serial.println(F("Relays set")); }     
    break;
        case ('s'):
      ShowStatus () ;
    break ;
*/
    default:            // catch all
    break ;
  }
}
