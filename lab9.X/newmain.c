#define TMR2PRESCALE 16

#include <xc.h>
// BEGIN CONFIG
#pragma config OSC = INTIO67   // Oscillator Selection bits (HS oscillator)
#pragma config WDT = OFF  // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRT = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON  // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF   // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF   // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)

//END CONFIG

PWM1_Init(long setDuty)
{
  PR2 = setDuty;
}

PWM1_Duty(unsigned int duty)
{
//set duty to CCPR1L , CCP1X(CCP1CON:5) and CCP1Y(CCP1CON:4)
    CCP1CONbits.CCP1X = (duty >> 1) & 1;
    CCP1CONbits.CCP1Y = duty & 1;
    CCPR1L = duty >> 2;
}

PWM1_Start()
{

  //set CCP1CON
  //set timer2 on
  //set rc2 output
  if (TMR2PRESCALE == 1)
  {
	
  }
  else if(TMR2PRESCALE == 4)
  {  
 
  }
  else if (TMR2PRESCALE == 16)
  {
      CCP1CON = 0b00001100;
      T2CON = 0x07;
      OSCCONbits.IRCF2 = 0;
      OSCCONbits.IRCF1 = 1;
      OSCCONbits.IRCF0 = 1;
      TRISCbits.RC2 = 0;
  }
 
}

void main()
{
  int i=16;
  int turn = 0;
  TRISDbits.RD0 = 1;
  PWM1_Init(155);
  PWM1_Start();
  
  do
  {
    if(RD0 == 0 && i<75)
      i=i+1;
    PWM1_Duty(i);

    
    int delay1 = 100;
    int delay2 = 10;
    while( delay1 != 0 )
    {
        delay2 = 10;
        while(delay2 != 0 )
        {
            delay2--;
        }
        delay1--;
    }
  }while(1);
}