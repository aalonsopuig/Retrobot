/*
Library:       SRF02.h
Purpose:       Manage one or more SRF02 I2C Ultrasonic Sensor
Developer:     Quark Robotics
Date:          July 2011

FUNCTIONS:

-SRF02_Distance_16(byte SRF02Address) returns the distance in cm to the
obstacle in the selected SRF02 device in int16 format
-SRF02_Distance_8(byte SRF02Address) returns the distance in cm to the
obstacle in the selected SRF02 device in byte format

*/


int16 SRF02_Distance_16(byte DeviceAddress)       
//Read the Distance in cm and return it in int16 format
{
   byte DataHigh=0, DataLow=0;         //Bytes read
   int16 Distance;                      //Value measured in cm
   
   i2c_start();
   i2c_write(DeviceAddress);
   i2c_write(0x00);                    // Register for commands
   i2c_write(0x51);                    // Start measure un cm
   i2c_stop();
   delay_ms(70);                       // Time for SRF02 to calculate distance
   i2c_start();
   i2c_write(DeviceAddress);
   i2c_write(0x02);                    // Register to start reading
   i2c_start();
   i2c_write(DeviceAddress|1);
   DataHigh = i2c_read();              // with ACK
   DataLow = i2c_read(0);              // with NAK
   i2c_stop();
   Distance = (DataHigh<<8);           // Calculate the range
   Distance += DataLow;
   return (Distance);
}

byte SRF02_Distance_8(byte DeviceAddress)       //
//Return the Distance in cm as a byte (m�x 255cm)
{
   byte DataHigh=0, DataLow=0;         //Bytes read
   byte Distance;                      //Value measured in cm
   
   i2c_start();
   i2c_write(DeviceAddress);
   i2c_write(0x00);                    // Register for commands
   i2c_write(0x51);                    // Start measure un cm
   i2c_stop();
   delay_ms(70);                       // Time for SRF02 to calculate distance
   i2c_start();
   i2c_write(DeviceAddress);
   i2c_write(0x02);                    // Register to start reading
   i2c_start();
   i2c_write(DeviceAddress|1);
   DataHigh = i2c_read();              // with ACK
   DataLow = i2c_read(0);              // with NAK
   i2c_stop();
   if (DataHigh>0) Distance=255;
   else Distance=DataLow;
   return (Distance);
}
