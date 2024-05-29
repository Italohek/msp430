// ER 9.3
// Endereçar escravo e transmitir 0x55
// Transmitir um único dado
// Mestre -> USCI_B0: P3.0 = SDA e P3.1 = SCL
// Escravo -> USCI_B1: P4.1 = SDA e P4.2 = SCL, endereço 42
#include <msp430.h>
#include "slave.h" //<--- ER 9.1
#define TRUE 1
#define FALSE 0
# define BR10K 105 //com SMCLK

// Funções usadas

void teste_nack(void);
void config_USCI_B0(void);
void config_leds(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    config_USCI_B1(); //<--- ER 9.1, USCI_B1 como escrava
    config_USCI_B0(); //USCI_B1 como mestre
    config_leds(); //Configurar Leds
    UCB0I2CSA = 42; //Endereço do escravo
    UCB0CTL1 |= UCTR | UCTXSTT; //Gerar START
    while ( (UCB0IFG & UCTXIFG) == 0); //Esperar TXIFG=1 (START)
    UCB0TXBUF=0x55; //Primeiro número no TXBUF
    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT); //Esperar TXSTT=0 (confirmação)
    teste_nack();
    UCB0CTL1 |= UCTXSTP; //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP); //Esperar STOP
    P4OUT |= BIT7; //Verde indica sucesso
    while(TRUE); //Prender execução
    return 0;
}

//Verificar se aconteceu NACK
void teste_nack(void){
    if ((UCB0IFG & UCNACKIFG) == UCNACKIFG){
        P1OUT |= BIT0; //Vermelho=falha
        UCB0CTL1 |= UCTXSTP; //Gerar STOP
        while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP); //Esperar STOP
        while(1); //Travar
    }
}

// Configurar USCI_B0 como mestre
// P3.0 = SDA e P3.1 = SCL
void config_USCI_B0(void){
    UCB0CTL1 = UCSWRST; //Ressetar USCI_B1
    UCB0CTL0 = UCMST | //Modo Mestre
    UCMODE_3 | //I2C
    UCSYNC; //Síncrono
    UCB0BRW = BR10K; //10 kbps
    UCB0CTL1 = UCSSEL_3; //SMCLK e UCSWRST=0
    P3SEL |= BIT1 | BIT0; //Funções alternativas
    P3REN |= BIT1 | BIT0;
    P3OUT |= BIT1 | BIT0;
}

// Configurar Leds
void config_leds(void){
    P1DIR |= BIT0; P1OUT &= ~BIT0;
    P4DIR |= BIT7; P4OUT &= ~BIT7;
}
