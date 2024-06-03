// Endereçar escravo e transmitir 0x55
// Transmitir um único dado
// Mestre -> USCI_B0: P3.0 = SDA e P3.1 = SCL
// Escravo -> USCI_B1: P4.1 = SDA e P4.2 = SCL, endereço 42
#include <msp430.h>
#include "slave.h"
#define TRUE 1
#define FALSE 0
# define BR10K 105 //com SMCLK

// Funções usadas

int teste_nack(void);
void config_USCI_B0(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    USCI_B1_config(); //<--- USCI_B1 como escrava
    config_USCI_B0(); //USCI_B1 como mestre
    UCB0I2CSA = 0x37; //Endereço do escravo
    volatile int i = 0;
    volatile char a;
    for(i = 0; i < 128; i++) {
        UCB0CTL1 |= UCTR | UCTXSTT; //Gerar START
        while ( (UCB0IFG & UCTXIFG) == 0); //Esperar TXIFG=1 (START)
        UCB0TXBUF=0x55; //Primeiro número no TXBUF
        while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT); //Esperar TXSTT=0 (confirmação)
        //teste_nack();
        UCB0CTL1 |= UCTXSTP; //Gerar STOP
        while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP); //Esperar STOP
        if (!teste_nack()) {
            a = i;
            break;
        }
    }
    while(1);
}

//Verificar se aconteceu NACK
int teste_nack(void){
    if ((UCB0IFG & UCNACKIFG) == 0){
        return 0;
    }
    return 1;
}

void USCI_B1_config(void){
    UCB1CTL1 = UCSWRST; //Resetar USCI_B1
    UCB1CTL0 = UCMODE_3 | //Modo I2C
    UCSYNC; //Síncrono
    UCB1I2COA = 0x37; //Endereço do Escravo
    UCB1CTL1 = 0; //UCSWRST=0
    UCB1IE = UCTXIE | UCRXIE; //Hab interrp TX e RX
    P4SEL |= BIT2 | BIT1; //Função alternativa
    P4REN |= BIT2 | BIT1; //Hab resistor
    P4OUT |= BIT2 | BIT1; //Pullup
    }

// Configurar USCI_B0 como mestre

// P3.0 = SDA e P3.1 = SCL
void config_USCI_B0(void){
    UCB0CTL1 = UCSWRST; //Resetar USCI_B1
    UCB0CTL0 = UCMST | //Modo Mestre
    UCMODE_3 | //I2C
    UCSYNC; //Síncrono
    UCB0BRW = BR10K; //10 kbps
    UCB0CTL1 = UCSSEL_3; //SMCLK e UCSWRST=0
    P3SEL |= BIT1 | BIT0; //Funções alternativas
    P3REN |= BIT1 | BIT0;
    P3OUT |= BIT1 | BIT0;
}


