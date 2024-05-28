#include <msp430.h> 
#include "slave.h"


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	return 0;
}

void USCI_B1_config(void) {
    UCB1CTL1 = UCSWRST;             // Resetar USCI_B1
    UCB1CTL0 = UCMODE_3 | UCSYNC;   // Modo I2C síncrono
    UCB1I2CA = 52;                  // Endereço do escravo que em hexa é 0x34
    UCB1CTL1 = 0;                   // UCSWRST = 0
    UCB1IE = UCTXIE | UCRXIE;       // Hablita interrupção TX e RX
    P4SEL |= BIT2 | BIT1;           // Função alternativa
    P4REN |= BIT2 | BIT1;           // Habilita o resistor
    P4OUT |= BIT2 | BIT1;           // Habilita o pullup
    for (b1 = 0; b1<MAX_USCI_B1; b1++){ // Zera o vetor de recepção
        b1_vet[b1] = 0;
    }
    b1 = 0;                         // Zera o indexador
    b1_cont = 0;                    // Zera o contador
    __enable_interrupt();           // Habilita interrupções
}

// ISR da USCI_B1
#pragma vector = USCI_B1_VECTOR     // Vetor 45
__interrupt void ist_USCI_B1(void) {
    int n;
    n = __even_in_range(UCB1IV, 0xC);
    switch(n) {
    case 0x0: break;
    case 0x2: break;
    case 0x4: break;
    case 0x6: break;
    case 0x8: break;
    case 0xA: USCI_B1_rx(); break;  // Rotina RX
    case 0xC: USCI_B1_tx(); break;  // Rotina TX
    }
}

// Rotina para receber um byte e guardar no vetor
void USCI_B1_rx(void) {
    b1_vet[b1++] = UCB1RXBUF;       // Guarda o dado recebido
    if (b1 > MAX_USCI_B1) {
        b1 = 0;
    }
}

// Rotina para transmitir o contador e incrementá-lo
void USCI_B1_tx(void) {
    UCB1TXBUF = b1_cont++;
}
