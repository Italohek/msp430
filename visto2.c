#include <msp430.h>

void config_ta1_1(void);
void instructions(int tecla);
void config_leds(void);
void config_tb0_1(void);
void config_servo(void);
void config_rising_ta1_1(void);
void config_falling_ta1_1(void);
void wait_first_falling_edge(void);
void wait_second_falling_edge(void);
void wait_rising_edge(void);
void write_vector(void);
void vector_into_words(void);
void debounce(int valor);

void up_pos(void);
void down_pos(void);

#define _1_x_1 0XBA45
#define _1_x_2 0XB946
#define _1_x_3 0XB847

#define _2_x_1 0XBB44
#define _2_x_2 0XBF40
#define _2_x_3 0XBC43

#define _3_x_1 0XF807
#define _3_x_2 0XEA15
#define _3_x_3 0XF609

#define _4_x_1 0XE916
#define _4_x_2 0XE619
#define _4_x_3 0XF20D

#define _5_x_2 0XE718

#define _6_x_1 0XF708
#define _6_x_2 0XE31C
#define _6_x_3 0XA55A

#define _7_x_2 0XAD52

#define POT100 10000 //0,02 * 1.048.576 --> 10 ms

#define partida_MAX 14500
#define partida_MIN 13500
#define bit1_MAX 2450
#define bit1_MIN 2250
#define bit0_MAX 1120
#define bit0_MIN 1340

#define T20ms 20971 //Período 20 ms (1.048.576 x 0,02)
#define T500us 524 //Período 0,5 ms (1.048.576 x 0,0005)
#define P10GRAUS 117 //Passo de 10 graus (2.097 / 18) 116.5 =~ 117

volatile int i = 0;
volatile int period=0;
volatile int vector[32];
volatile unsigned long code; //variavel
volatile unsigned long tecla;
volatile int pos=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Desabilita o watchdog timer
    config_leds(); // Configuração dos LEDs
    config_tb0_1(); // Configuração do Timer_B0 para PWM
    config_ta1_1(); // Configuração do Timer_A1 para captura de flancos
    config_servo(); // Configuração do Timer_A2 para controle de servo
    while(1){
        config_falling_ta1_1(); // Configuração de captura de flanco de descida
        wait_first_falling_edge(); // Espera pelo primeiro flanco de descida
        wait_second_falling_edge(); // Espera pelo segundo flanco de descida
        if((period>partida_MIN) && (period<partida_MAX)) { // Verifica período
            config_rising_ta1_1(); // Configuração de captura de flanco de subida
            wait_rising_edge(); // Espera pelo flanco de subida
            write_vector(); // Escreve valores de captura no vetor
        }
        vector_into_words(); // Converte vetor em palavras
        instructions(tecla); // Executa instruções com base na tecla pressionada
    }
}

// Função para escrever valores de captura no vetor
void write_vector(void){
    for(i=0;i<32;i++){
        while((TA1CCTL1 & CCIFG)==0); // Aguarda flag de captura
        TA1CTL |= TACLR; // Limpa o timer
        TA1CCTL1 &=~CCIFG; // Limpa flag de captura
        vector[i]=TA1CCR1; // Armazena valor de captura no vetor
    }
}

// Função para converter vetor em palavras
void vector_into_words(void){
    for (i=0;i<32;i++){
        code=code>>1; // Desloca código para a direita
        if((vector[i]>bit1_MIN) && (vector[i]<bit1_MAX)){
            code |= 0x80000000L; // Atualiza bit mais significativo do código
        }
    }
    tecla=code>>16; // Atualiza código da tecla
}

void config_ta1_1(void){
    //Configuracao de TA1.1 e P2.0
    TA1CTL = TASSEL_2 | MC_2; //SMCLK, Modo 2
    P2DIR &= ~BIT0; //P2.0 como entrada
    P2REN |= BIT0; //Habilitar resistor
    P2OUT |= BIT0; //Para pullup
    P2SEL |= BIT0; //Dedicado para captura (TA1.1)
    TA1CCTL1 &= ~CCIFG; //Apagar CCIFG
}

// Configuração do Timer_A1 para captura de flanco de subida
void config_rising_ta1_1(void){
    TA1CCTL1 = CM_1 | CCIS_0 | SCS | CAP; // Captura flanco de subida
    TA1CCTL1 &= ~CCIFG; // Limpa flag de captura
}

// Configuração do Timer_A1 para captura de flanco de descida
void config_falling_ta1_1(void){
    TA1CCTL1 = CM_2 | CCIS_0 | SCS | CAP; // Captura flanco de descida
    TA1CCTL1 &= ~CCIFG; // Limpa flag de captura
}

// Espera pelo primeiro flanco de descida
void wait_first_falling_edge(void){
    while((TA1CCTL1&CCIFG)==0); // Aguarda primeiro flanco de descida
    TA1CTL|=TACLR; // Limpa o timer
    TA1CCTL1&=~CCIFG; // Limpa flag de captura
}

// Espera pelo segundo flanco de descida
void wait_second_falling_edge(void){
    while((TA1CCTL1&CCIFG)==0); // Aguarda segundo flanco de descida
    TA1CTL|=TACLR; // Limpa o timer
    TA1CCTL1&=~CCIFG; // Limpa flag de captura
    period = TA1CCR1; // Atualiza período
}

// Espera pelo flanco de subida
void wait_rising_edge(void){
    while((TA1CCTL1&CCIFG)==0); // Aguarda flanco de subida
    TA1CTL|=TACLR; // Limpa o timer
    TA1CCTL1&=~CCIFG; // Limpa flag de captura
}

// Configuração dos LEDs
void config_leds(void){
    P1OUT &= ~BIT0; // LED vermelho desligado
    P1DIR |= BIT0; // LED vermelho como saída
    P4OUT &= ~BIT7; // LED verde desligado
    P4DIR |= BIT7; // LED verde como saída
    P4SEL |= BIT7; // P4.7 mapeado para saída de Timer_B0
    PMAPKEYID = 0X02D52; //Escrever chave
    P4MAP7 = PM_TB0CCR1A; //TB0.1 mapeado para P4.7
}
    

// Incrementa posição do servo
void up_pos(void) {
    pos++;
    if (pos > 18) pos=18;
    TA2CCR2=T500us+pos*P10GRAUS; // Atualiza posição do servo
}

// Decrementa posição do servo
void down_pos(void) {
    pos--;
    if (pos < 0) pos=0;
    TA2CCR2=T500us+pos*P10GRAUS; // Atualiza posição do servo
}

void instructions(int tecla){
    switch(tecla){
        case _1_x_1:
            P1OUT |= BIT0;
            break;

        case _1_x_2:
            P1OUT &= ~BIT0;
            break;

        case _1_x_3:
            P1OUT ^= BIT0;
            break;

        case _2_x_1:
            TB0CCR1 = POT100;
            break;

        case _2_x_2:
            TB0CCR1 = 0;
            break;

        case _2_x_3:
            TB0CCR1 = POT100 - TB0CCR1;
            debounce(10000);
            break;

        case _3_x_1:
            down_pos();
            break;

        case _3_x_2:
            TB0CCR1 = 0;
            up_pos();
            break;

         case _3_x_3:
             TB0CCR1 = POT100 * 0.1;
             break;

         case _4_x_1:
             TB0CCR1 = POT100 * 0.2;
             pos = 0;
             TA2CCR2=T500us;
             break;

         case _4_x_2:
             TB0CCR1 = POT100 * 0.3;
             pos = 9;
             TA2CCR2=T500us+(pos*P10GRAUS);
             break;

         case _4_x_3:
             TB0CCR1 = POT100 * 0.4;
             pos = 18;
             TA2CCR2=T500us+(pos*P10GRAUS);
             break;

         case _5_x_2:
             TB0CCR1 = POT100 * 0.5;
             break;

         case _6_x_1:
             TB0CCR1 = POT100 * 0.6;
             break;

         case _6_x_2:
             TB0CCR1 = POT100 * 0.7;
             break;

         case _6_x_3:
             TB0CCR1 = POT100 * 0.8;
             break;

         case _7_x_2:
             TB0CCR1 = POT100 * 0.9;
             break;
    }
}

void config_tb0_1(void){
    TB0CTL = TBSSEL_2 | MC_1; //TB0 com SMCLK e Modo Up
    TB0CCR0 = POT100; //Período PWM
    TB0CCTL1 = OUTMOD_6; //Saída Modo 6
    TB0CCR1 = POT100; //Iniciar com 100%
}

void config_servo(void){
    TA2CTL = TASSEL_2|ID_0|MC_1|TACLR;
    TA2EX0 = TAIDEX_0;
    TA2CCR0 = T20ms; //Período de 20 ms
    TA2CCTL2 = OUTMOD_6;
    TA2CCR2 = T500us; //Zero graus
    //Configurar P2.5 para PWM
    P2DIR |= BIT5;
    P2SEL |= BIT5; //P2.5 = PWM
}

void debounce(int valor) {
    for(i = 0; i < valor; i++);
}
