#include <msp430.h> 

void GPIO_config();
void ADC_config();
void TimerB_config();
void TimerA_config();
void PWM(float valor, char canal);

volatile char MX, MY;
volatile char canal;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	GPIO_config();
    TimerA_config();
    TimerB_config();
	ADC_config();
    PWM(255/2., 0);
    PWM(255/2., 1);
	__enable_interrupt();
	while(1){
	    PWM(MX/255.0, 1);
	    PWM(MY/255.0, 0);
	}
	return 0;
}

void ADC_config(){
    volatile unsigned char *pt;
    unsigned char i;

    ADC12CTL0 = ~ADC12ENC;
    ADC12CTL0 = ADC12ON;
    ADC12CTL1 = ADC12CONSEQ_3 | ADC12SHS_1 | ADC12CSTARTADD_0 | ADC12SSEL_3;
    ADC12CTL2 = 0;

    pt = &ADC12MCTL0;
    for (i = 0; i < 8; i++){
        pt[i] = ADC12SREF_0 | ADC12INCH_1;
    }
    for (i = 8; i < 16; i++){
        pt[i] = ADC12SREF_0 | ADC12INCH_2;
    }
    pt[15] |= ADC12EOS;


    P6SEL |= BIT1 | BIT2;
    ADC12CTL0 |= ADC12ENC;
    ADC12IE |= ADC12IE15;
}

void TimerA_config(){

    TA0CTL = TASSEL_1 | MC_1;
    TA0CCTL1 = OUTMOD_6;
    TA0CCR0 = 32767/400;
    TA0CCR1 = TA0CCR0/2;

}

void GPIO_config(){

    P4DIR |= BIT7;
    P4OUT &= ~BIT7;
    P4SEL |= BIT7;

    P4DIR |= BIT3;
    P4OUT &= ~BIT3;
    P4SEL |= BIT3;

    PMAPKEYID = 0x02D52;
    P4MAP3 = PM_TB0CCR1A;
    P4MAP7 = PM_TB0CCR2A;
    PMAPKEYID = 0;
    P3DIR &= ~BIT4;
    P3REN |= BIT4;
    P3OUT |= BIT4;

}

void TimerB_config(){

    TB0CTL = TBSSEL_1 | MC_1;
    TB0CCTL1 = OUTMOD_6;
    TB0CCTL2 = OUTMOD_6;
    TB0CCR0 = 32767/400;
    TB0CCR1 = TB0CCR0/2;
    TB0CCR2 = TB0CCR0/2;

}


#pragma vector = ADC12_VECTOR
__interrupt void adc_isr(void){
    volatile unsigned int *pt;
    volatile unsigned int i,soma;
    pt = &ADC12MEM0;
    soma = 0;
    for (i=0; i<8; i++) {
        soma +=pt[i];
    }
    MX = soma >> 3;
    soma = 0;
    for (i=8; i<16; i++) {
        soma += pt[i];
    }
    MY = soma >> 3;
}

void PWM(float valor, char canal){
    if (canal == 0) {//Y
        TB0CCR1 = TB0CCR0*valor;
    }
    else{//X
        TB0CCR2 = TB0CCR0*valor;
    }
}












