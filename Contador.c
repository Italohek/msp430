#include <msp430.h>
#define contador 3276

void configleds();

int main(void) {
    unsigned int dseg, seg, min, hora; // Definição dos valores
    WDTCTL = WDTPW | WDTHOLD; // Stop dogtimer
    configleds();

    TA0CTL = TASSEL_1 + MC_1; // Define a configuração do contador como ACLK e up
    TA0CCR0 = contador; // Valor de interrupção do clock

    dseg = seg = min = hora = 0;
    while(1) {
        while ((TA0CCTL0&CCIFG) == 0); // Enquanto a tag de overflow for 0, continua no loop
        TA0CCTL0 &= ~CCIFG; // Redefine a tag de overflow pra 0
        P4OUT ^= BIT7; // Led verde é alternado
        if (++dseg == 10) {
            dseg = 0;
            P1OUT ^= BIT0; // Led vermelho é alternado
            if (++seg == 60) {
                seg = 0;
                if (++min == 60) {
                    min = 0;
                    if (++hora == 24) {
                        hora = 0;
                    }
                }
            }
        }
    }
    return 0;
}

void configleds(void) {
    P1DIR |= BIT0; // Led vermelho é a saída
    P1OUT &= ~BIT0;  // Led vermelho apagado
    P4DIR |= BIT7; // Led verde é saída
    P4OUT &= ~BIT7; // Led verde tá apagado
}
