
byte setlower(byte CmdByte)
{
    if ((CmdByte >= 65) && (CmdByte <90)) CmdByte = CmdByte + 32 ; // change to upper case
    return(CmdByte);
}

void flushserial()
{
  do
  {
    if(Serial.available() > 0)
    {
      byte temp = Serial.read() ; // flush down buffer
    } 
  } while(Serial.available()>0);
}


uint32_t getint(uint16_t waittime)
{
      Serial.print ("> ");  
      flushserial();
      Serial.setTimeout(waittime) ;
      return (Serial.parseInt());
}


void ShowStatus()
{

  Serial.print( digitalRead(HOME_SENSOR_PIN));
  Serial.print( digitalRead(A1));
  Serial.print( digitalRead(A2));
  Serial.println( digitalRead(A0));
}

void ParseCmd (byte CmdByte)
{
  byte CmdStatus = 0;
  CmdByte=(setlower(CmdByte)) ;
  switch (CmdByte) {
    case ('0'):
      processTurnoutCommand(200, 1, 1);
    break ;
    case ('1'):
      processTurnoutCommand(201, 1, 1);
    break ;
    case ('2'):
      processTurnoutCommand(202, 1, 1);
    break;
    case ('3'):
      processTurnoutCommand(203, 1, 1);
    break ;
    case ('4'):
      processTurnoutCommand(204, 1, 1);
    break ;
    case ('5'):
      processTurnoutCommand(205, 1, 1);
    break;
    case ('6'):
      processTurnoutCommand(206, 1, 1);
    break ;
    case ('7'):
      processTurnoutCommand(207, 1, 1);
    break ;
    case ('8'):
      processTurnoutCommand(208, 1, 1);
    break;
    case ('9'):
      processTurnoutCommand(209, 1, 1);
    break ;
    case ('s'):
      ShowStatus();
    break ;
    default:            // catch all
    break ;
  }
}
