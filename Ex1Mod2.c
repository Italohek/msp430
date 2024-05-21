#include <msp430.h> 

void configleds();
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	configleds();
	while(1){
	    if ((P2IN & BIT1) == 0) { //Se a chave estiver pressionada
	        P4OUT |= BIT7; //O led verde é aceso
	    }
	    else {
	        P4OUT &= ~BIT7; //O led verde é apagado
	    }
	}
	return 0;
}

void configleds() {
    P4DIR |= BIT7; //Led verde é saída
    P4OUT &= ~BIT7; //Led verde começa apagado

    P2DIR &= ~BIT1; //Botão esqueda é entrada
    P2REN |= BIT1; //Habiltia o resistor do botão 1
    P2OUT |= BIT1; //Habilita pullup, ou seja, estado alto quando não pressionado
}
