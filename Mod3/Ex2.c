#include <msp430.h> 

#define MAX_ADR 50
#define BR10K 105 //com SMCLK

void USCI_B0_config(void);
void USCI_B1_config(void);
char i2c_test(char adr, char modo);
void leds_s1_config(void);
void rebote(int x);

int main(void){
     char i; //Indexador dos vetores
     char adr; //Endereço de escravo
     volatile unsigned valor = 0;

     WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
     leds_s1_config(); //Leds e S1
     USCI_B0_config(); //Configurar USCI_B0
     USCI_B1_config();

     while(1) {
     while ( (P2IN&BIT1) == BIT1); //Esperar S1
         P4OUT &= ~BIT7; //Apagar verde
         P1OUT |= BIT0; //Acender vermelho
         rebote(30000);
         // Procurar pelos escravos transmissores
         i=1;
         for (adr=0; adr<127; adr++){
             if (i2c_test(adr, 0) == 1) {
                 valor = adr; }
             if (i==MAX_ADR) break;
         }

         P1OUT &= ~BIT0; //Apagar vermelho
         P4OUT |= BIT7; //Acender verde
         while ( (P2IN&BIT1) == 0); //Esperar soltar S1
         rebote(1000);
     }
     return 0;
}


char i2c_test(char adr, char modo){
     UCB0I2CSA = adr; //Endereço a ser testado

     if (modo== 0){ //RD = Escravo Transmissor
         UCB0CTL1 &= ~UCTR; //Mestre RX <-- escravo TX
         UCB0CTL1 |= UCTXNACK; //NACK ao receber um dado
         UCB0CTL1 |= UCTXSTT; //Gerar START
         while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT); //Esperar START
     }
     UCB0CTL1 |= UCTXSTP; //Gerar STOP
     while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP); //Esperar STOP

     //Teste do ACK
     if ((UCB0IFG & UCNACKIFG) == 0) return 1; //Chegou ACK
     else return 0; //Chegou NACK
}

// Configurar USCI_B0 como mestre
// P3.0 = SDA e P3.1 = SCL
void USCI_B0_config(void){ // Mestre
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

// Configurar I/O
void leds_s1_config(void){
     P1DIR |= BIT0; P1OUT &= ~BIT0; //Led Vermelho
     P4DIR |= BIT7; P4OUT &= ~BIT7; //Led Verde
     P2DIR &= ~BIT1; //P2.1 = entrada
     P2REN |= BIT1; //Habilitar resistor
     P2OUT |= BIT1; //Selecionar Pullup
}

// Delay para evitar rebotes
void rebote(int x){
    volatile int i;
    for (i=0; i<x; i++);
}

void USCI_B1_config(void){ // Escravo
     UCB1CTL1 = UCSWRST; //Resetar USCI_B1
     UCB1CTL0 = UCMODE_3 | //Modo I2C
     UCSYNC; //Síncrono
     UCB1I2COA = 32; //Endereço do Escravo
     UCB1CTL1 = 0; //UCSWRST=0
     UCB1IE = UCTXIE | UCRXIE; //Hab interrp TX e RX
     P4SEL |= BIT2 | BIT1; //Função alternativa
     P4REN |= BIT2 | BIT1; //Hab resistor
     P4OUT |= BIT2 | BIT1; //Pullup
}
