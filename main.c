#define ZERO 0b00111111
#define ONE 0b00000110
#define TWO 0b01011011
#define THREE 0b01001111
#define FOUR 0b01100110
#define FIVE 0b01101101
#define SIX 0b01111101
#define SEVEN 0b00000111
#define EIGHT 0b01111111
#define NINE  0b01101111

int ms = 0, m = 0, s = 0, digit = 0, adc_l = 0, adc_h = 0, a = 0, value=0;
int rpm=0;
long cycle=0, trot, icr_l, icr_h;

void Init_adc() {
     ADMUX = 0b01000000;
     ADCSRA = 0b10000111;
}

int readADC(char ch) {
    ADMUX &= 0b11100000;
    ADMUX |= ch;
    ADCSRA |= (1 << 6);
           while (ADCSRA & (1 << 6));
    adc_l = ADCL;
    adc_h = ADCH;
    return ((adc_h << 8) | adc_l);
}

void display(int a, int b) {
     PORTA = 0b11110000;
     PORTC = 0b00000000;
     switch (a) {
            case 1:
                 PORTA = 0b00000001;
                 break;
            case 2:
                 PORTA = 0b00000010;
                 break;
            case 3:
                 PORTA = 0b00000100;
                 break;
            case 4:
                 PORTA = 0b00001000;
                 break;
     }
     switch(b) {
               case 0:
                    PORTC = ZERO;
                    break;
               case 1:
                    PORTC = ONE;
                    break;
               case 2:
                    PORTC = TWO;
                    break;
               case 3:
                    PORTC = THREE;
                    break;
               case 4:
                    PORTC = FOUR;
                    break;
               case 5:
                    PORTC = FIVE;
                    break;
               case 6:
                    PORTC = SIX;
                    break;
               case 7:
                    PORTC = SEVEN;
                    break;
               case 8:
                    PORTC = EIGHT;
                    break;
               case 9:
                    PORTC = NINE;
                    break;
     }
}

void init_timer() {
     SREG = 1<<7;
     TCCR0 = 0b00001011;
     TCNT0 = 0;
     OCR0 = 125;
     TIMSK |= 0b00000010;
}

void Timer1_OC_ISR() iv IVT_ADDR_TIMER0_COMP ics ICS_AUTO {

     digit++;
     switch(digit) {
          case 1: display(1, rpm%10); break;
          case 2: display(2, (rpm/10)%10); break;
          case 3: display(3, (rpm/100)%10); break;
          case 4: display(4, rpm/1000); digit = 0; break;
     }
     if (ms==99){
          value = (readADC(5)/4);
          //trot = (icr_l +65536 * cycle) * 300;
          OCR2 = value;
          ms=0;
     }else ms++;
}

void init_PWM2(){
     DDRD |= 1<<7;//PD7
     TCCR2 = 0b01101001; 
}

void Init_Input_Capture(){
     DDRD &=~(1<<6); 
     TCCR1A = 0b00000000;
     TCCR1B = 0b01000010;
     TIMSK |= 1<<5; 
     TIMSK |= 1<<2; 
}

void IC_ISR() iv IVT_ADDR_TIMER1_CAPT ics ICS_AUTO {
     TCNT1H=0;
     TCNT1L=0;
     icr_l = ICR1L;
     icr_h = ICR1H;
     trot = (((icr_h << 8) | icr_l)+65536*cycle)*300;
     rpm = 60000000/trot;
     cycle=0;
}


void COMP_ISR() iv IVT_ADDR_TIMER1_OVF ics ICS_AUTO 
{
     cycle++; 
}


void main() {
     DDRA = 0b00001111;
     DDRC = 0b11111111;
     init_timer();
     Init_adc();
     init_PWM2();
     Init_Input_Capture();
     init_PWM2();
     while(1);
}
