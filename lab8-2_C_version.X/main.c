#include <xc.h>
// CONFIG1H
#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
// CONFIG3H
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
// CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)

int count=0;
void main(void) {
    RCONbits.IPEN=0x01;
    T1CON = 0x95;
    T2CON = 0x7F;
    
    OSCCONbits.IRCF2 = 0;
    OSCCONbits.IRCF1 = 1;
    
   
    PIE1bits.TMR1IE=1; // TMR1 Overflow Interrupt Enable bit
    IPR1bits.TMR1IP=1; //TMR1 Overflow Interrupt Priority bit
    PIR1bits.TMR1IF=0; //TMR1 Overflow Interrupt Flag bit
    
    PIE1bits.TMR2IE=1; // TMR2 Overflow Interrupt Enable bit
    IPR1bits.TMR2IP=1; //TMR2 Overflow Interrupt Priority bit
    PIR1bits.TMR2IF=0; //TMR2 Overflow Interrupt Flag bit
    
    TMR1 = 49910;
    
    TMR2 = 0;
    PR2 = 244;
    INTCONbits.GIE=1;
    
    TRISD = 0x00;
    LATD = 0x03;
    
    while(1);
}

void interrupt tc_int(void)     // High priority interrupt
{
    if(TMR1IE && TMR1IF){
        TMR1 = 49910;
        if(LATDbits.LD0 == 0)
            LATDbits.LD0 = 1;
        else 
            LATDbits.LD0 = 0;

        TMR1IF=0;
    }
    
    if(TMR2IE && TMR2IF){
        TMR2 = 0;
        TMR1 = 49910;
        if(LATDbits.LD1 == 0)
            LATDbits.LD1 = 1;
        else 
            LATDbits.LD1 = 0;
        
        TMR2IF=0;
    } 
    
}