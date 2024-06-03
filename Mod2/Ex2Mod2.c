#include <msp430.h> 

void configleds();
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    configleds();
    while(1){ 
        if ((P2IN & BIT1) == 0) { //Se a chave for pressionada
            P1OUT ^= BIT0; //O led vermelho é alternado
        }
    }
    return 0;
}

void configleds() {
    P1DIR |= BIT0; //Led vermelho é a saída
    P1OUT &= ~BIT0; //Led vermelho começa apagado

    P2DIR &= ~BIT1; //Botão esquerdo é entrada
    P2REN |= BIT1; //Habiltia o resistor do botão 1
    P2OUT |= BIT1; //Habilita pullup, ou seja, estado alto quando não pressionado
}
