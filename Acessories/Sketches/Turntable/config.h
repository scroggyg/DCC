 /*
 * Bobuino DCC Configuration file for 1284P
 * Copyright Lumenid Limited 2018
 */
 /*
 * EEPROM Memory map
 */
 #define ROCO             // Used to indicate that system is connected to Roco 
#define ver_add           2
#define ser_add           ver_add+2
#define ADDRESS_LSB       ser_add+2   
#define ADDRESS_MSB       ADDRESS_LSB+1
#define PULSE_TIME        ADDRESS_MSB+2
#define RECHARGE_TIME     PULSE_TIME+1
#define ACTIVE_STATE      RECHARGE_TIME+1
#define debug_add         ACTIVE_STATE+1 
/* Outputs and inputs */             
#define ENT1A 25
#define ENT1B 24
#define ENT2A 28
#define ENT2B 27
#define ENT3A 8
#define ENT3B 19
#define ENT4A 29
#define ENT4B 26
#define ENT5A 13
#define ENT5B 12
#define ENT6A 11
#define ENT6B 10
#define ENT7A 7
#define ENT7B 30
#define ENT8A 5
#define ENT8B 4
#define XP_DIR 17
#define red     20         // Fault LED
#define amber   21          // Status LED
#define LEARNINGBUTTON A0          // Learn input


// I2C defines
#define PCA9557 0x18          // I2C address
#define  PCA9557_INP_PORT_REG    0x00  // input port
/**
 * Output port is read/write, writing to input bits has no effect.
 * This internal reg outputs are qualified by the config pin dir register
 * Reading this reg returns the value of this internal register, NOT
 * the actual device pin. So reading this only confirms its setting.
 * Read the input port to read the actual value on all device I/O pins.
 */
#define PCA9557_OUT_PORT_REG    0x01  // output port
/**
 * Polarity inversion register is read/write.
 * invert polarity of input port if bit is 1
 * POR=0xF0
 */
#define PCA9557_INP_INVERT_REG  0x02
/**
 * Config register sets each bit's mode: 1=input, 0=output
 * POR = 0xFF (all I/O bits are inputs)
 */
 #define PCA9557_CONFIG_REG  0x03
#define maxleds     3
#define maxturnouts 8
