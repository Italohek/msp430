// LCD_Base
// Rotinas básicas para usar o LCD

// P3.0 ==> SDA
// P3.1 ==> SCL
#include <stdio.h>
#include <msp430.h> 

void lcd_inic();
void lcdWriteNibble(char dado, char RS);
void B0_config();
char i2cSend(char address, char data);
void WriteFloat(float valor, int casas);


int main(void){
    volatile float teste = 0xffffff;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    B0_config();

    lcd_inic();     //Inicializar LCD
    i2cSend(0x27, 8);   //Acender Back Light

    volatile char str[]="Hello ITALO!!";
    volatile int i = 0;
    volatile float value = 3.147;

    while(str[i] != 0){
        lcdWriteNibble(str[i],1);
        i++;
    }
    lcdWriteNibble(0xc0, 0);
    i = 0;
    WriteFloat(value, 3);

    while(1);
    return 0;
}

// Incializar LCD modo 4 bits
void lcd_inic(void){
    UCB0I2CSA = 0x27;
    UCB0CTL1 |= UCTR    | UCTXSTT;
    while ((UCB0IFG & UCTXIFG) == 0);
    UCB0TXBUF = 0;
    while ((UCB0CTL1 & UCTXSTT) == UCTXSTT);
    if ((UCB0IFG & UCNACKIFG) == UCNACKIFG)
        while(1);

    lcdWriteNibble(0, 0);
    //__delay_cycles(20000);
    lcdWriteNibble(3, 0);     //3
    //__delay_cycles(30000);
    lcdWriteNibble(3, 0);     //3
    //__delay_cycles(10000);
    lcdWriteNibble(3, 0);     //3
    //__delay_cycles(10000);
    lcdWriteNibble(2, 0);     //2

    // Entrou em modo 4 bits
    lcdWriteNibble(0x28, 0);
    lcdWriteNibble(8, 0);
    lcdWriteNibble(01, 0);
    lcdWriteNibble(06, 0);
    lcdWriteNibble(0x0F, 0);
}

// Auxiliar inicialização do LCD (RS=RW=0)
// *** Só serve para a inicialização ***
void lcdWriteNibble(char dado, char RS){
    char highNibble = (dado & 0xF0) | (RS);
    char lowNibble = ((dado << 4) & 0xF0) | (RS ? 0x01 : 0x00);

    i2cSend(0x27, highNibble| BIT3);
   // __delay_cycles(100);
    i2cSend(0x27, highNibble| BIT3 | BIT2);
    //__delay_cycles(1000);
    i2cSend(0x27, highNibble | BIT3);
    //__delay_cycles(1000);
    i2cSend(0x27, lowNibble| BIT3);
    //__delay_cycles(100);
    i2cSend(0x27, lowNibble| BIT3 | BIT2);
    //__delay_cycles(1000);
    i2cSend(0x27, lowNibble | BIT3);
}


// Escrever dado na porta
char i2cSend(char address, char data) {
    UCB0I2CSA = address;
    UCB0CTL1 |= UCTR | UCTXSTT;
    while ((UCB0IFG & UCTXIFG) == 0);
    UCB0TXBUF = data;
    while ((UCB0CTL1 & UCTXSTT) != 0);
    UCB0CTL1 |= UCTXSTP;
    while ((UCB0CTL1 & UCTXSTP) != 0);
    if ((UCB0IFG & UCNACKIFG) != 0)
        return 1; // Erro
    return 0; // Sucesso
}


void B0_config(){   //mestre
    UCB0CTL1 = UCSWRST;
    UCB0CTL0 = UCSYNC | UCMODE_3 | UCMST;
    UCB0BRW = 11;
    UCB0CTL1 = UCSSEL_3;
    P3SEL |= BIT0 | BIT1;
    P3REN |= BIT1 | BIT0;
    P3OUT |= BIT1 | BIT0;
}

void WriteFloat(float valor, int casas){
    volatile int temp;
    volatile int i = 0;
    temp = valor;
    valor -= temp;
    valor*=10;
    lcdWriteNibble(temp+'0', 1);
    lcdWriteNibble(',', 1);
    while (casas != 0){
        temp = valor;
        lcdWriteNibble(temp+'0', 1);
        valor-=temp;
        valor *=10;
        casas--;
    }
}
