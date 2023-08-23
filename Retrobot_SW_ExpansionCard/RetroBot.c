/*
PROGRAM:    RetroBot
DEVELOPER:  LIEBANA Design
DATE:       July 2011
Purpose:    This program is loaded in the ZX81 Expansion Card
            and control the interface.

Pinout:
RA0   IN    Not connected
RA1   OUT   ZX81 Wait signal
RA2   IN    ZX81 Direction of data 
RA3   OUT   Relay control
RA4   IN    Not connected
RA5   IN    PIR signal
RB0   IN    ZX81 ready signal      
RB1   I/O   Data Line 1 
RB2   I/O   Data Line 2
RB3   I/O   Data Line 3
RB4   I/O   Data Line 4
RB5   I/O   Data Line 5
RB6   I/O   Data Line 6
RB7   I/O   Data Line 7
RC0   IN    Not connected   
RC1   OUT   PWM2 for motors   
RC2   OUT   PWM1 for motors 
RC3         I2C. SCL
RC4         I2C. SDA
RC5   I/O   Data Line 0
RC6   OUT   Led
RC7   IN    Not connected

I/O Expansion Port Pinout:

A0    OUT   S1M1
A1    OUT   S2M1
A2    OUT   S1M2
A3    OUT   S2M2
A4    OUT   S1M3
A5    OUT   S2M3
A6    OUT   S1M4
A7    OUT   S2M4 
B0    OUT   S1M5
B1    OUT   S2M5
B2    OUT   S1M6
B3    OUT   S2M6
B4    OUT   S1M7
B5    OUT   S2M7
B6    OUT   S1M8
B7    OUT   S2M8


*/
// LIBRARIES
#include "RetroBot.h"
#include "LM75.h"             //Library for LM75 I2C Temperature Sensor 
#include "MCP23016.h"         //MCP23016 Chip (16 bit IO expander via I2C)
#include "SRF02.h"            //SRF02 Device (Sonar via I2C)

//I2C address
#DEFINE  LM75Address      0x9E  //Temperature sensor
#DEFINE  SRF02Address     0xE0  //Sonar sensor

//Pins assignment
#DEFINE  ZX81_WAIT       PIN_A1   //Active low. stop ZX81 clock during a transfer
#DEFINE  ZX81_DIR        PIN_A2   //Data dir. 1:ZX81->ExpCard, 0:ExpCard->ZX81
#DEFINE  Relay           PIN_A3   //Relay control
#DEFINE  PIR             PIN_A5   //PIR signal
#DEFINE  ZX81_READY      PIN_B0   //Indicate ZX81 is ready for a read or write
#DEFINE  DataLine0       PIN_C5   //Data line 0
#DEFINE  LED             PIN_C6   //Test led

//Motors control
#DEFINE  FORWARD         255      //Forward for motors
#DEFINE  BACKWARD          0      //Backward for motors
#DEFINE  STOP            128      //Stop for motors
#DEFINE  ACTIVATE          0      //ACTIVATE for motors
#DEFINE  OPEN            255      //OPEN for motors
#DEFINE  CLOSE             0      //CLOSE for motors
#DEFINE  UP              255      //Move up for motors
#DEFINE  DOWN              0      //Move down for motors

#DEFINE  MaxDuty 800     //Maximum value of Duty Cycle (100%)
#DEFINE  MinDuty 200     //Duty were the motor stops for sure

//Motor assignment
#DEFINE  Maraca            1      //Motor assigned to Maraca
#DEFINE  Hand_R            2      //Motor assigned to Hand of right arm
#DEFINE  Arm_R             3      //Motor assigned to right arm
#DEFINE  Light_R           4      //Motor assigned to light of right arm
#DEFINE  Wheel_R           5      //Motor assigned to right wheel
#DEFINE  Wheel_L           6      //Motor assigned to left wheel
#DEFINE  Shoulder_R        7      //Motor assigned to shoulder of right arm
#DEFINE  Shoulder_L        8      //Motor assigned to shoulder of left arm


//Counters
#DEFINE  MaxCounters    2     //Number of multipurpose counters
#DEFINE  AutoNavFwd     0     //counter for allowing forward movement
#DEFINE  Dance          1     //counter for dance times

#DEFINE  MaxAutoNavFwd   381     //x13.1= 5s aprox
#DEFINE  MaxDance        763     //x13.1= 10s aprox


byte  Temperature=0;             //Temperature of the card
byte  Distance=0;                //Value in cm of distance to obstacle 8bit
int1  PIRStatus=FALSE;           //Status of PIR sensor

int1  AutoNavMode=FALSE;      //Indicate if auto navigation mode is active
int16 Counter[MaxCounters];         //Multipurpose counters incremented by interrupt

int16  i;                            //Used for local FOR loops



#INT_TIMER3
//Timer3 Interrupt. Used for several multipurpose counters Counter[]
//overflows every 13.1ms incrementing all counters by 1
void Timer3_isr() 
{
   byte NumCounter;

   for(NumCounter=0;NumCounter<MaxCounters;NumCounter++)
      Counter[NumCounter]++;
}


void InitGeneralPurposeCounters() 
//Put all general purpose counters to 0
{
   byte NumCounter;

   for(NumCounter=0;NumCounter<MaxCounters;NumCounter++)
      Counter[NumCounter]=0;
}   

void Blink (byte blinks)
//Blinks card led
{
   byte i;
   for(i=1;i<=blinks;i++) {
      output_high(LED);
      delay_ms(100);
      output_low(LED);
      delay_ms(100);              
   }
}


void SetMotor (byte MotorNum, Direction)
//Control the direction of the 8 motors
//Direction 128:Stop, >128:Forward, <128:backwards
{
   switch (MotorNum) {
      case 1:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_A0);
            MCP23016_output_low  (MCP23016_PIN_A1);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_A0);
            MCP23016_output_high (MCP23016_PIN_A1);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_A0|MCP23016_PIN_A1);         
         }
         break;
      case 2:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_A2);
            MCP23016_output_low  (MCP23016_PIN_A3);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_A2);
            MCP23016_output_high (MCP23016_PIN_A3);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_A2|MCP23016_PIN_A3);         
         }
         break;
      case 3:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_A4);
            MCP23016_output_low  (MCP23016_PIN_A5);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_A4);
            MCP23016_output_high (MCP23016_PIN_A5);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_A4|MCP23016_PIN_A5);         
         }
         break;
      case 4:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_A6);
            MCP23016_output_low  (MCP23016_PIN_A7);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_A6);
            MCP23016_output_high (MCP23016_PIN_A7);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_A6|MCP23016_PIN_A7);         
         }
         break;
      case 5:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_B0);
            MCP23016_output_low  (MCP23016_PIN_B1);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_B0);
            MCP23016_output_high (MCP23016_PIN_B1);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_B0|MCP23016_PIN_B1);         
         }
         break;
      case 6:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_B2);
            MCP23016_output_low  (MCP23016_PIN_B3);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_B2);
            MCP23016_output_high (MCP23016_PIN_B3);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_B2|MCP23016_PIN_B3);         
         }
         break;
      case 7:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_B4);
            MCP23016_output_low  (MCP23016_PIN_B5);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_B4);
            MCP23016_output_high (MCP23016_PIN_B5);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_B4|MCP23016_PIN_B5);         
         }
         break;
      case 8:
         if(Direction<128){
            MCP23016_output_high (MCP23016_PIN_B6);
            MCP23016_output_low  (MCP23016_PIN_B7);
         }
         if(Direction>128){
            MCP23016_output_low  (MCP23016_PIN_B6);
            MCP23016_output_high (MCP23016_PIN_B7);
         }
         if(Direction==128){
            MCP23016_output_low (MCP23016_PIN_B6|MCP23016_PIN_B7);         
         }
         break;
      default:
         break; 
   }
}

void Dancer ()
// Activate audio cassette and dance with maraca
{

   Output_high(RELAY);            //Relay (Casete) signal on 
   SetMotor (Maraca, ACTIVATE);    //Activate maraca
   for (i=1;i<=8;++i) {
      SetMotor (Wheel_R, FORWARD);   
      SetMotor (Wheel_L, BACKWARD);   
      delay_ms (1000);  //Time of turn. Then stop
      SetMotor (Wheel_R, STOP);   
      SetMotor (Wheel_L, STOP);   
      delay_ms (1000);  //Time of turn. Then stop
      SetMotor (Wheel_R, BACKWARD);   
      SetMotor (Wheel_L, FORWARD);   
      delay_ms (1000);  //Time of turn. Then stop
      SetMotor (Wheel_R, STOP);   
      SetMotor (Wheel_L, STOP);   
   }
   Output_low(RELAY);            //Relay (Casete) signal off 
   SetMotor (Maraca, STOP);    //Activate maraca
   //Arm
   SetMotor (Arm_R, FORWARD);    //Extend Arm_R
   delay_ms(1000);
   SetMotor (Arm_R, STOP);    //Stop Arm_R
   delay_ms(1000);
   SetMotor (Arm_R, BACKWARD);    //Compress Arm_R
   delay_ms(1000);
   SetMotor (Arm_R, STOP);    //Stop Arm_R
}



///////////////////////////////////////////////////////////////////////////////
//////                                                                   //////
//////                          MAIN BODY                                //////
//////                                                                   //////
///////////////////////////////////////////////////////////////////////////////

void main()
{

   //Ports configuration
   set_tris_a (0b11110101);        //configure port A (See comments at header)
   set_tris_b (0b11111111);        //configure port B (See comments at header)
   set_tris_c (0b10100001);        //configure port C (See comments at header)
   
   Output_low(RELAY);            //Relay signal off 
   Output_low(LED);              //Led off
   Output_high(ZX81_WAIT);       //Non wait (Active low)


   //TIMERS CONFIGURATION
   setup_timer_3(T3_INTERNAL|T3_DIV_BY_1);   //Timer for multipurpose counters
   set_timer3(0);                            //ensure overflow every 13.1ms

   //PWM generation
   setup_timer_2(T2_DIV_BY_1,199,1);          // PWM 1 & 2 aprox 25KHz
   setup_ccp1(CCP_PWM);                      
   setup_ccp2(CCP_PWM);
   set_pwm1_duty(800);         
   set_pwm2_duty(500);         

   //General Purpose counters
   InitGeneralPurposeCounters();

   //ENABLE INTERRUPTS
   enable_interrupts(INT_TIMER3);   //Timer3 overflow
   enable_interrupts(GLOBAL);   


   //OTHER INITIAL SETUPS --------------------------------

   Blink(10);

   //I/O Espander port config . Do not move.
   MCP23016_Reg_Write(IODIR0, 0b00000000);
   MCP23016_Reg_Write(IODIR1, 0b00000000);
   

//   set_pwm1_duty(800);           //Duty cycle of Group 1 of motors 

AutoNavMode=TRUE;
//Output_high(RELAY);

   while (TRUE) { //MAIN LOOP

//      Temperature=LM75_TempRead(LM75Address);   //Get temperature
//      PIRStatus=Input(PIR);                     //Get PIR status
      Distance=SRF02_Distance_8(SRF02Address);         //Get Sonar range

/*delay_ms(1000);

If (Distance!=0) Blink(2);
*/
//If (PIRStatus) Blink(2);

   //Arm
/*   SetMotor (Shoulder_L, UP);    //Move up Shoulder_L
   delay_ms(6000);
   SetMotor (Shoulder_L, STOP);    //Stop Shoulder_L
   delay_ms(1000);
   SetMotor (Shoulder_L, DOWN);    //Move down Shoulder_L
   delay_ms(3000);
   SetMotor (Shoulder_L, STOP);    //Stop Shoulder_L
   delay_ms(2000);

*/


/*

            SetMotor (Wheel_R, FORWARD);   
            SetMotor (Wheel_L, FORWARD);   
            delay_ms (1000);  //Time of turn. Then stop
            SetMotor (Wheel_R, STOP);   
            SetMotor (Wheel_L, STOP);   


   //Arm
   SetMotor (Arm_R, FORWARD);    //Extend Arm_R
   delay_ms(1000);
   SetMotor (Arm_R, STOP);    //Stop Arm_R
   delay_ms(1000);
   SetMotor (Arm_R, BACKWARD);    //Compress Arm_R
   delay_ms(1000);
   SetMotor (Arm_R, STOP);    //Stop Arm_R
   delay_ms(1000);

   //Hand
   SetMotor (Hand_R, OPEN);    //Open hand
   delay_ms(1000);
   SetMotor (Hand_R, STOP);    //Stop hand
   delay_ms(1000);
   SetMotor (Light_R, ACTIVATE);    //Activate Light right arm
   delay_ms(1000);
   SetMotor (Light_R, STOP);    //STOP Light right arm
   SetMotor (Hand_R, CLOSE);    //Close hand
   delay_ms(1000);
   SetMotor (Hand_R, STOP);    //Stop hand
   delay_ms(1000);

   //Maraca
   SetMotor (Maraca, ACTIVATE);    //Activate maraca
   delay_ms(2000);
   SetMotor (Maraca, STOP);    //Stop maraca
   delay_ms(4000);
*/
      //Activación audio
/*      SetRelay(TRUE);
      delay_ms(2000);
      SetRelay(FALSE);
      delay_ms(4000);
*/     

//Counter[Dance]=0;

      if (Counter[Dance]>MaxDance){
         Dancer ();
         Counter[Dance]=0;
      }

      if(AutoNavMode) {
         if(((Distance<50)&&(Distance>0))||(Counter[AutoNavFwd]>MaxAutoNavFwd)) { 
         //Obstacle in front or too much time going ahead. Turn a bit
            SetMotor (Wheel_R, STOP);   
            SetMotor (Wheel_L, STOP);   
            delay_ms (100);
            SetMotor (Wheel_R, FORWARD);   
            SetMotor (Wheel_L, BACKWARD);   
            delay_ms (1000);  //Time of turn. Then stop
            SetMotor (Wheel_R, STOP);   
            SetMotor (Wheel_L, STOP);   
            Counter[AutoNavFwd]=0;
         }
         else {                             //No obstacle. Go forward
            SetMotor (Wheel_R, FORWARD);   
            SetMotor (Wheel_L, FORWARD);   
         }
      }

   } //End While MAIN LOOP

}  //End Program
