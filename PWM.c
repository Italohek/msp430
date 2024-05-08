#include <msp430.h>

// Protótipos das funções
void config_leds(void);
void config_p12(void);
void config_timer(void);

// Variável volátil para armazenar a fase do timer
volatile int fase = 0;

int main(void){
    // Desativa o watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Configuração dos LEDs
    config_leds();

    // Configuração do P1.2 como saída
    config_p12();

    // Configuração do Timer
    config_timer();

    while(1); // Loop infinito

    return 0;
}

// Configuração do LED como saída
void config_leds(void){
    P1DIR |= BIT0; // Define P1.0 como saída
    P1OUT &= ~BIT0; // Apaga o LED P1.0
}

// Configuração do P1.2 para usar a saída do Timer A
void config_p12(void){
    P1DIR |= BIT2; // Define P1.2 como saída
    P1SEL |= BIT2; // Usa o timer para o P1.2
}

// Configuração do Timer A
void config_timer(void) {
    TA0CTL = TASSEL_1 | MC_1; // Configura Timer A com ACLK (TASSEL_1) e modo de contagem MC_1
    TA0CCR0 = 256; // Define o valor máximo do contador do Timer A
    TA0CCR1 = (TA0CCR0 * 0.5) - 1; // PWM com 50% de ciclo
    TA0CCTL1 = OUTMOD_6; // Configura o modo de saída do CCR1 para gerar PWM
}
