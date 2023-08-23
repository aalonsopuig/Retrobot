/*
Library:       MCP23016.h
Purpose:       Use of MCP23016 Chip (16 bit IO expander via I2C)
Developer:     Quark Robotics
Date:          June 2010
Compiler:      CCS PCH v4.057

This library manage the MCP23016 Chip (16 bit IO expander via I2C). This device
have two eight-bit ports (PortA and PortB). The way to works with this ports is
by writing and reading registers. There are 6 couples of registers (see section
of #DEFINES). Each element of the couple is refered to each of the ports.
The registers could be accesed (for read or write) either individualy or in
couples.

Read below the information about Functions, Configuration and Warnings.

FUNCTIONS:
MCP23016_Reg_Write(byte Reg, Data): Write a byte to a register
MCP23016_Reg_Write16(byte Reg, unsigned int16 Data): Write data to registers in 
16 bit mode (a couple of registers at a time). 
PCF8574_Reg_Read(byte Reg): Read data from the register
PCF8574_Reg_Read16(byte Reg): Read data from a couple of registers
MCP23016_output_high (int16 Pin): Set a pin or set of pins at PortA and PortB
MCP23016_output_low (int16 Pin): clear a pin or set of pins at PortA and PortB
MCP23016_input(int16 Pin): Get the value of a pin at PortA and PortB. If more 
than one pin is requested, the function return true if any of the pins is active

Examples:

   //write 0b00001111 (0F) at IODIR0 register
   MCP23016_Reg_Write(IODIR0, 0b00001111);
   
   //write FF at IODIR1 and 0F at IODIR0 register
   MCP23016_Reg_Write16(IODIR0, 0xFF0F);
   
   //read the values at PortA (GP0 register)
   data=PCF8574_Reg_Read(GP0);
   
   //read the values at PortA (GP0) and PortB (GP1)
   data16=PCF8574_Reg_Read16(GP0);
   
   //set the Pin 3 at PortA not changing the rest of the bits
   MCP23016_output_high (MCP23016_PIN_A3)

   //clear the bits 3 and 5 at PortB not changing the rest of the bits
   MCP23016_output_low (MCP23016_PIN_B3|MCP23016_PIN_B5)

   //Read bit 3 of PortA
   data=MCP23016_input(MCP23016_PIN_A3)

CONFIGURATION:
(1) Include the I2C preprocessor command at your main program. Something like:
#use i2c(Master,Slow,sda=PIN_C4,scl=PIN_C3)
(2) Ensure you use the correct I2C address. 
MCP23016 address: |0|1|0|0|A2|A1|A0|R/W|
A2,A1 and A0 are the external pins for getting different addreses.
R/W is the read, write bit.
In this library we have considered the use of the MCP23016 with A2, A1, A0 
connected to Vss (0 volts). If this is not the case, change the address at
#DEFINE MCP23016Address
(3) Configure direction (input or output) of each port bit by writing to the
registers IODIR0 and IODIR1. A port coud have some bits as inputs (1) and others
as outputs (0). ie: MCP23016_Reg_Write(IODIR0, 0b00001111) the first 4 pins
of the register will be used as inputs and the last 4 as outputs
(4) Write/read from/to the ports. ie: MCP23016_Reg_Write(GP0, 0xFF); write FF
to PortA while data=PCF8574_Reg_Read(GP0); reads the values at PortA


WARNINGS:
(1) This device starts with all pins configured as input
(2) If a port is configured in part as input and in part as output (some pins
as input and other as output), then if you write a byte, only the output pins
will show the corresponding bits of that value, but if you perform a read of
that port, you will get both the inputs and outputs. ie: if you have PortA
configured with the first 4 bits as inputs and the 4 last bits as outputs, if
you write FF (11111111) to the port, only the output bits will show data, so
at PortA you will have 1111xxxx, being xxxx whatever values of inputs are there.
If you now have at the input bits for example the value 0001 (first 4 pins) and
you perform a read of this port, you will get 11110001 so, the first 4 bits are
the input bits and the last 4 bits are the values you wrote to them. If you
whant to eliminate them, you just need to use a mask. for example
data=PCF8574_Reg_Read(GP0) & 0b00001111 will just give you the values of the
four first bits and data=PCF8574_Reg_Read(GP0) & PCF8574_Reg_Read(IODIR0) will
give you just the data at input pins (the mask here is the configuration
register of such port)

---
*/

#DEFINE MCP23016Address  0b01000000  //The I2C address of the device if A0,A1,A2 
                                     //are connected to Vss (0v)
                                     
//Registers described in datasheet                                     
#DEFINE  GP0     0x00               //PortA
#DEFINE  GP1     0x01               //PortB
#DEFINE  OLAT0   0x02               //Output latch of PortA
#DEFINE  OLAT1   0x03               //Output latch of PortB
#DEFINE  IPOL0   0x04               //Polarity of PortA pins
#DEFINE  IPOL1   0x05               //Polarity of PortB pins
#DEFINE  IODIR0  0x06               //Direction of PortA pins (1=In, 0=Out)
#DEFINE  IODIR1  0x07               //Direction of PortB pins (1=In, 0=Out)
#DEFINE  INTCAP0 0x08               //Interrupt Capture of PortA
#DEFINE  INTCAP1 0x09               //Interrupt Capture of PortB
#DEFINE  IOCON0  0x0A               //I/O expander control register
#DEFINE  IOCON1  0x0B               //Same as IOCON0

//Bits of registers
#DEFINE  MCP23016_PIN_A0 0x0001     
#DEFINE  MCP23016_PIN_A1 0x0002     
#DEFINE  MCP23016_PIN_A2 0x0004     
#DEFINE  MCP23016_PIN_A3 0x0008     
#DEFINE  MCP23016_PIN_A4 0x0010     
#DEFINE  MCP23016_PIN_A5 0x0020     
#DEFINE  MCP23016_PIN_A6 0x0040     
#DEFINE  MCP23016_PIN_A7 0x0080     
#DEFINE  MCP23016_PIN_B0 0x0100     
#DEFINE  MCP23016_PIN_B1 0x0200     
#DEFINE  MCP23016_PIN_B2 0x0400     
#DEFINE  MCP23016_PIN_B3 0x0800     
#DEFINE  MCP23016_PIN_B4 0x1000     
#DEFINE  MCP23016_PIN_B5 0x2000     
#DEFINE  MCP23016_PIN_B6 0x4000     
#DEFINE  MCP23016_PIN_B7 0x8000     

void MCP23016_Reg_Write(byte Reg, Data)
//Write data to a register in 8 bit mode
{
   i2c_start();                  //Starting Signal
   i2c_write(MCP23016Address);   //I2C Address (Write)
   i2c_write(Reg);               //Select register
   i2c_write(Data);              //Write data
   i2c_stop();
   delay_us(50);                //Requires this delay to work properly
}

void MCP23016_Reg_Write16(byte Reg, unsigned int16 Data)
//Write data to registers in 16 bit mode
{
   byte Data1, Data2;            //Data to write
   
   Data1= (Data & 0x00FF);
   Data2=((Data & 0xFF00) >> 8);
   i2c_start();                  //Starting Signal
   i2c_write(MCP23016Address);   //I2C Address (Write)
   i2c_write(Reg);               //Select register
   i2c_write(Data1);              //Write data
   i2c_write(Data2);              //Write data
   i2c_stop();
   delay_us(50);                //Requires this delay to work properly
}


byte PCF8574_Reg_Read(byte Reg)
//Read data from the register in 8 bit mode
{
   byte Data;            //Data read
   
   i2c_start();                  //Starting Signal
   i2c_write(MCP23016Address);    //I2C Address (Write)
   i2c_write(Reg);               //Select register
   i2c_stop();
   delay_us(50);                //Requires this delay to work properly
   i2c_start();                  //Starting Signal
   i2c_write(MCP23016Address|1);  //I2C Address (Read)
   Data = i2c_read(0);           //Read value with NAK
   return (Data);
}

int16 PCF8574_Reg_Read16(byte Reg)
//Read data from a couple of registers
{
   byte Data1, Data2;            //Data read
   unsigned int16 Data;          //the 16bit value of both registers
   
   i2c_start();                  //Starting Signal
   i2c_write(MCP23016Address);    //I2C Address (Write)
   i2c_write(Reg);               //Select register
   i2c_stop();
   delay_us(50);                //Requires this delay to work properly
   i2c_start();                  //Starting Signal
   i2c_write(MCP23016Address|1);  //I2C Address (Read)
   Data1 = i2c_read();           //Read first value
   Data2 = i2c_read(0);          //Read second value with NAK
   i2c_stop();
   delay_us(50);                //Requires this delay to work properly
   Data=Data2;
   Data=Data<<8;
   Data=Data|Data1;
   return (Data);
}

void MCP23016_output_high (int16 Pin)
//Set a pin or set of pins at PortA and PortB
{
   MCP23016_Reg_Write16(GP0, PCF8574_Reg_Read16(GP0)|Pin);
}

void MCP23016_output_low (unsigned int16 Pin)
//clear a pin or set of pins at PortA and PortB
{
   MCP23016_Reg_Write16(GP0, PCF8574_Reg_Read16(GP0)& ~Pin);
}

int1 MCP23016_input(int16 Pin)
//Get the value of a pin at PortA and PortB. If more than one pin is requested
//the function return true if any of the pins is active
{
   int16  data;
   data=PCF8574_Reg_Read16(GP0) & Pin;
   if (data>0) return (true);
   return (false);
}
