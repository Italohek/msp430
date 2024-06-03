#include <msp430.h>

void configLED();
void configTimer();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    configLED();
    configTimer();

    __bis_SR_register(GIE);     // Habilita as interrupções gerais

    while (1) {
        // Programa continua e o led é invertido na rotina de interrupção
    }

    return 0;
}

void configLED() {
    P4DIR |= BIT7;   // Configura o led verde como saída 
    P4OUT &= ~BIT7;  // Inicia com o LED verde apagado
}

void configTimer() {
    TA0CTL = TASSEL_2 + MC_1 + TACLR; // configura o Timer A0 para funcionar no modo Up Mode com a fonte de clock SMCLK
    TA0CCR0 = 49999; // Valor de contagem
    TA0CCTL0 = CCIE; // Habilita a interrupção de comparação para TACCR0
}

// Rotina de interrupção do Timer0_A0
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    P4OUT ^= BIT7;   // Inverte o estado do LED
}
