sbit LCD_RS at RB2_bit ;
sbit LCD_EN at RB3_bit ;
sbit LCD_D4 at RB4_bit ;
sbit LCD_D5 at RB5_bit ;
sbit LCD_D6 at RB6_bit ;
sbit LCD_D7 at RB7_bit ;
sbit LCD_RS_Direction at TRISB2_bit ;
sbit LCD_EN_Direction at TRISB3_bit ;
sbit LCD_D4_Direction at TRISB4_bit ;
sbit LCD_D5_Direction at TRISB5_bit ;
sbit LCD_D6_Direction at TRISB6_bit ;
sbit LCD_D7_Direction at TRISB7_bit ;
sbit Va at RA0_bit ;
char message1 [] = " Capacitance ";
char message2 [] = " Meter ";
unsigned int T_Value , Num ;
float x , cap , y ;
unsigned short i , j , TimerValue , OverRange = 0;
char Capacitance [] = "00.000 " , txt [20];
void interrupt () {
  if( PIR1 . TMR2IF ) {
  TMR2 = TimerValue ;
  Num ++;
  if( Num > 9999) OverRange = 1; // Range is 99.99 uF
  PIR1 . TMR2IF =0; // Clear TMR0 interrupt flag
  }
}
void Display_Cap ( unsigned int n ) {
  Capacitance [0] = n /10000 + 48;
  Capacitance [1] = ( n /1000) %10 + 48;
  Capacitance [3] = ( n /100) %10 + 48;
  Capacitance [4] = ( n /10) %10 + 48;
  Capacitance [5] = ( T_Value *10) /153 + 48;
  Lcd_Cmd ( _Lcd_Clear ) ;
  Lcd_Out (1 , 1 , " C ( uF ) :") ;
  x = n +( T_Value /20) ;
  cap =(0.002357* x -0.00205) ;
  FloatToStr ( cap , txt ) ;
  if (! n )
    Lcd_Out (2 , 1 , Capacitance ) ;
  else
    Lcd_Out (2 ,1 , txt ) ;
}
void reset () {
  TRISA = 0 b00000100 ;
  CMCON = 7;
  RA1_bit = 0;
  Delay_ms (2000) ;
  TRISA = 0 b00000110 ;
  CMCON = 5;
}
void main () {
  char cap_size ;
  TRISB = 0 b00000001 ;
  PORTB = 0;
  TRISA = 0 b00000110 ;
  OPTION_REG . T0CS = 0;
  INTCON . GIE = 1; // Enable global interrupt
  INTCON . PEIE = 1; // Enable peripheral interrupt
  // Configure Timer2 module
  PIE1 . TMR2IE = 1; // Enable Timer2 interrupt
  T2CON = 0; // Prescaler 1:1 , and Timer2 is off initially
  PIR1 . TMR2IF =0; // Clear int bit
  CMCON = 5; // Independent comparator between RA1 ( -) and RA2 (+)
  Lcd_Init () ;
  Lcd_Cmd ( _Lcd_Clear ) ;
  Lcd_Cmd ( _LCD_CURSOR_OFF ) ;
  Lcd_Out (1 , 1 , message1 ) ;
  Lcd_Out (2 , 1 , message2 ) ;
  delay_ms (2000) ;
  Lcd_Cmd ( _Lcd_Clear ) ;
  Lcd_Out (1 , 1 , " C = ") ;
  Va = 0;
  TimerValue = 112; // 104 + 4 clock delay on branching to ISR and others
  while (1) {
    Num = 0;
    OverRange =0;
    Lcd_Cmd ( _Lcd_Clear ) ;
    Lcd_Out (1 , 1 , " Testing .") ;
    Lcd_Out (2 , 1 , "...") ;
    TMR2 = TimerValue ; // Initial value of Timer2 for 30 us delay
    Va = 1; // apply voltage
    T2CON . TMR2ON = 1; // start timer
    while ( CMCON . C2OUT ) {
      if( OverRange ) ;
    }
    T2CON . TMR2ON = 0; // stop timer
    T_Value = TMR2 - TimerValue ;
    Va = 0;
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (! OverRange ) {
      Display_Cap ( Num *10) ;
    }
    else {
      OverRange = 0;
      Lcd_Cmd ( _Lcd_Clear ) ;
      Lcd_Out (1 , 1 , " Out of Range !") ;
    }
    reset () ;
  }
}
end
