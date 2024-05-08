#include <msp430.h>

// Defini��o dos intervalos para 0,7 e 0,3 * 32768
#define INTERV_70 22938 // Intervalo = 0,7 * 32768
#define INTERV_30 9831  // Intervalo = 0,3 * 32768


void leds(void); // Configura os leds
void con1(void); // Controla o led 1
void con2(void); // Controla o led 2

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Desativa o watchdog timer
    leds(); // Configura os LEDs
    TA0CTL = TASSEL_1 | MC_2; // Configura o Timer A com ACLK e modo de contagem cont�nua (Continuous mode)
    TA0CCTL1 = CCIE; // Habilita a interrup��o de compara��o/captura do CCR1
    TA0CCTL2 = CCIE; // Habilita a interrup��o de compara��o/captura do CCR2
    TA0CCR1 = INTERV_70; // Define o valor de compara��o do CCR1 para o intervalo de 70%
    TA0CCR2 = INTERV_30; // Define o valor de compara��o do CCR2 para o intervalo de 30%
    __enable_interrupt(); // Habilita as interrup��es globais
    while(1); // Loop infinito
        return 0;
}

// Rotina de interrup��o do Timer A
#pragma vector = TIMER0_A1_VECTOR
__interrupt void isr_ta0(void){
    int n;
    n = __even_in_range(TA0IV,0xE); // L� o vetor e armazena o valor em 'n'
    switch(n){
        case 0x2: con1(); break; // Intera��o com o CCR1
        case 0x4: con2(); break; // Intera��o com o CCR2
        default: break;
    }
}

// Fun��o para controlar o LED conectado ao P1.0
void con1(void){
    P1OUT ^= BIT0; // Inverte o estado do LED
    TA0CCR1 += INTERV_70; // Incrementa o valor de compara��o do CCR1
}

// Fun��o para controlar o LED conectado ao P4.7
void con2(void){
    P4OUT ^= BIT7; // Inverte o estado do LED
    TA0CCR2 += INTERV_30; // Incrementa o valor de compara��o do CCR2
}

// Configura��o dos LEDs
void leds(void){
    P1DIR |= BIT0; // Define P1.0 como sa�da
    P1OUT &= ~BIT0; // Apaga o LED (P1.0)
    P4DIR |= BIT7; // Define P4.7 como sa�da
    P4OUT &= ~BIT7; // Apaga o LED (P4.7)
}
