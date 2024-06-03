#include <msp430.h> 
#define DBC 30000 // Valor definido para a quantidade de loops do for do debounce

void configleds();
void debounce(int valor);
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    configleds();
    while(1){
        if ((P2IN & BIT1) == 0) { // Se o botão esquerdo for pressionado
            P1OUT ^= BIT0; // O led vermelho é alternado
            debounce(DBC); // Temos debounce 
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

void debounce(int valor) {
    volatile int i;
    for (i = 0; i < valor; i++); // Tempo
}
