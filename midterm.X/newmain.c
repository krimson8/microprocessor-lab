#define TMR2PRESCALE 16

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
// BEGIN CONFIG
#pragma config OSC = INTIO67   // Oscillator Selection bits (HS oscillator)
#pragma config WDT = OFF  // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRT = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON  // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF   // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF   // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)

//END CONFIG
// notes on how to start the serial terminal 
// sudo apt-get isntall putty
// dmesg | grep tty
// sudo putty /dev/ttyUSB0 -serial -sercfg 9600,8,n,1,N

int i=1;
int lolflag = 38;
unsigned int MyadcValue;
unsigned int total = 0;
int timersec = 0;
char global = '\0';
int au = 0;

void Myusartwrite(char b)
{
    TXREG = b;
    
    return ;
}

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
    TRISCbits.TRISC6 = 1;            // Setting by data sheet 
    TRISCbits.TRISC7 = 1;            //  
    TRISDbits.RD0 = 0;
    TRISDbits.RD1 = 0;
//    
//    /*
//           Serial Setting      
//     * 1.   Setting Baud rate
//     * 2.   choose sync/async mode 
//     * 3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
//     * 3.5  enable Tx, Rx Interrupt(optional)
//     * 4.   Enable Tx & RX
//     */      
//    //  Setting baud rate
    BAUDCONbits.BRG16 = 1 ;          //Read Baud rate table
    TXSTAbits.BRGH = 1;
    SPBRG = 25;      
//    
//    //   Serial enable
    TXSTAbits.SYNC = 0;             //choose the async moode
    RCSTAbits.SPEN = 1;              //open serial port
    
    //setting TX/RX
    PIR1bits.TXIF = 1;
    PIR1bits.RCIF = 0;
    TXSTAbits.TXEN = 1;             //Enable Tx
    RCSTAbits.CREN = 1;             //Enable Rx
    //setting TX/RX interrupt
    // PIE1bits.TXIE = 1;              //Tx interrupt
    // IPR1bits.TXIP = 0;              //Setting Tx as high/low priority interrupt
     PIE1bits.RCIE = 1;              //Rx interrupt
     IPR1bits.RCIP = 0;              //Setting Rc as high/low priority interrupt
//    
    // setup adc
    ADCON1bits.VCFG1 = 0 ;  //setting vref-
    ADCON1bits.VCFG0 = 0 ;  //setting vref+
    ADCON1bits.PCFG  = 1;   //Setting A/D Port Configuration Control
    ADCON0bits.CHS = 0 ;    //setting input channel
    TRISAbits.RA0 = 1 ;     //setting port as input 
    ADCON2bits.ADFM = 1 ;   //setting ??? justified

    ADCON2bits.ACQT = 1;
    ADCON2bits.ADCS = 0;
    ADCON0bits.ADON = 1;    //turn on ad module 

    RCONbits.IPEN=0x01;
    INTCONbits.GIEH = 1;    //enable high priority interrupt
    INTCONbits.GIEL = 1;     //enable low priority interrupt
    
    // setup timer and pwm
    T1CON = 0x95;
    CCP1CON = 0b00001100;
    T2CON = 0x07;
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 0;
    OSCCONbits.IRCF0 = 0;
    TRISCbits.RC2 = 0;

    PIE1bits.TMR1IE=1; // TMR1 Overflow Interrupt Enable bit
    IPR1bits.TMR1IP=1; //TMR1 Overflow Interrupt Priority bit
    PIR1bits.TMR1IF=0; //TMR1 Overflow Interrupt Flag bit

    TMR1 = 34285;
}

void MyadcRead()
{
    while(ADCON0bits.GO == 1);
    ADCON0bits.GO = 1;
    MyadcValue = ADRESL;
    MyadcValue |= (ADRESH & 3) << 8;

    return ;
}

char checker(int a)
{
    char aChar;
    switch (a)
    {
        case 0:
            aChar = '0';
            break;
        case 1:
            aChar = '1';
            break;
        case 2:
            aChar = '2';
            break;
        case 3:
            aChar = '3';
            break;
        case 4:
            aChar = '4';
            break;
        case 5:
            aChar = '5';
            break;
        case 6:
            aChar = '6';
            break;
        case 7:
            aChar = '7';
            break;
        case 8:
            aChar = '8';
            break;
        case 9:
            aChar = '9';
            break;
        default:
            aChar = '?';
            break;
    }
    return aChar;
}

void main()
{
    PWM1_Init(155);
    PWM1_Start();

    while(1) {
        MyadcRead();
    }
}

void interrupt wtf(void)     // High priority interrupt
{
    if(TMR1IE && TMR1IF){
        TMR1 = 34285;
        if( i > 600 ) lolflag = -38;
        else if( i < 1) {
            i = 1;
            lolflag = 38;
        }
        PWM1_Duty(i);

        i += lolflag;
        TMR1IF = 0;
        total = total + MyadcValue;
        timersec++;

        if(timersec == 4) {
            if(au == 1) {
                total = total/4;
                while( total > 0 ) {
                    int a = total%10;
                    char b = checker(a);
                    Myusartwrite(b);
                    total /= 10;
                }
                Myusartwrite(' ');
            }
            

            timersec = 0;
            total = 0;
        }
    }
    if(RCIF) {
        global = RCREG;
        if(global == 'p') {
            Myusartwrite('p');
            while( MyadcValue > 0 ) {
                int a = MyadcValue%10;
                char b = checker(a);
                Myusartwrite(b);
                MyadcValue /= 10;
            }
            Myusartwrite(' ');
        }
        if(global == 'a') {
            if(au == 1) au = 0;
            else au = 1;
        }
    }

    
}
