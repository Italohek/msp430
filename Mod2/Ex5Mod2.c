#include <msp430.h>

void configleds();
void delay_ms(int ms);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    configleds(); // Configura o comportamento do led vermelho

    while(1){
        P1OUT ^= BIT0;   // Inverte o estado do led vermelho
        delay_ms(500);   // Atraso de 500 ms
    }

    return 0;
}

void configleds() {
    P1DIR |= BIT0;   // Configura o led vermelho como saída
    P1OUT &= ~BIT0;  // Inicia com o led vermelho  apagado
}

void delay_ms(int ms) {
    volatile int i;
    for (i = 0; i < ms; i++) {
        __delay_cycles(1000);  // A cada loop levamos 1 ms, logo 500 loops são 500ms
    }
}
