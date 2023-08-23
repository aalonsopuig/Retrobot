/*
Library:       LM75.h
Purpose:       Manage one or more LM75 I2C Temperature Sensor
Developer:     Quark Robotics
Date:          February 2011

FUNCTIONS:

-LM75_TempRead(byte LM75Address) returns the temperature value of the selected
LM75 chip in ºC in a signed integer8 value

CONFIGURATION:

LM75 have four pins for configuring the address. They are A0, A1 and A2. 
connecting any one of them to Vcc means it will value 1, and connecting it to
GND means it will value 2. As a result, the address of an LM75 chip is
1001-A2A1A0-0. For example, if A0=A1=A2=0 then the address is 10010000 and if 
A0=A1=A2=1, then address is 10011110.
*/


signed int LM75_TempRead(byte LM75Address)       //
//Read the temperature:
//DataHigh contains the signed ºC figure
//MSB of DataLow contains the decimal value as LM75 have 0.5ºC sensibility
//we ignore this sensibility in this function, so the function returns
//the ºC value either positive or negative as per LM75 caracteristics.
{
   byte DataHigh=0, DataLow=0;         //Bytes read
   
   i2c_start();
   i2c_write(LM75Address);
   i2c_write(0x00);                    // Pointer Byte
   i2c_start();
   i2c_write(LM75Address|1);
   DataHigh = i2c_read();              // with ACK
   DataLow = i2c_read(0);              // with NAK
   i2c_stop();
   if (DataHigh>150) DataHigh=0; //Error in data. Better return 0.
   return (DataHigh);
}


