LIST p=18f4520		
#include<p18f4520.inc>
#include<lab4mac.asm>

ORG	0x00 ; setting initial address
    
INITIAL:    NUMS10	     ;// 載入16組數字至120~12F address
	    LFSR 0,0X120    ; 設定FSR0為0x120當作i
	    LFSR 1,0X121    ; 設定FSR1為0x120當作j  

MAIN:	    MOVFF FSR0L, FSR1L
	    MOVFF FSR0H, FSR1H
	    RCALL SBRT
	   
	    INCF FSR0L
	    BTFSS FSR0L, 4
	    GOTO MAIN
	    GOTO CHECK ; 排序完後跳至check檢查排序是否正確
	    
SBRT:   ; // 可任意的增加label數量     
	    MOVF INDF0, 0
	    CPFSGT INDF1, 0
	    GOTO SWP 
LP:	    INCF FSR1L
	    BTFSS FSR1L, 4
	    GOTO SBRT
	    RETURN

SWP:       SWAP INDF0, INDF1
	    GOTO LP
	    
CHECK:	    RTLST ; 檢查是否排序正確
	    NOP
	    END