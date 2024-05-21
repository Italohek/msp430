#include <msp430.h> 
#define DBC     30000 //Valor utilizado como argumento no debounce

void ledsconfig(void); //Função para configurar o led
void debounce(int valor); //Função para diminuir o bounce

void main(void){
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    ledsconfig(); //Configurar led

    while(1){
            if ( (P2IN & BIT1) == 0 || (P1IN&BIT1) == 0 ) { //Se a chave esquerda ou direita forem pressionadas
                P1OUT ^= BIT0; //Inverte o estado do led P1
                debounce(DBC); //Evitamos o bounce através da função
            }
    }
}

// Configurar GPIO
void ledsconfig(void){
    P1DIR |= BIT0; //Led1 = P1.0 = saída
    P1OUT &= ~BIT0; //Led1 apagado

    P2DIR &= ~BIT1; //S1 = P2.1 = entrada
    P2REN |= BIT1; //Habilitar resistor
    P2OUT |= BIT1; //Habilitar pullup

    P1DIR &= ~BIT1; //S2 = P2.2 = entrada
    P1REN |= BIT1; //Habilitar resistor
    P1OUT |= BIT1; //Habilitar pullup
}

// Debounce
void debounce(int valor){
    volatile int i; //Volatile vai evitar optimização do compilador
    for (i = 0; i < valor; i++); //Apenas gasta tempo
}
