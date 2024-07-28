#include <msp430.h>

void configure_ta1_capture(void);
void execute_instructions(int button);
void configure_leds(void);
void configure_tb0_pwm(void);
void configure_servo(void);
void set_ta1_rising_edge_capture(void);
void set_ta1_falling_edge_capture(void);
void await_first_falling_edge(void);
void await_second_falling_edge(void);
void await_rising_edge(void);
void record_capture_values(void);
void decode_vector_to_word(void);
void debounce_delay(int value);

void increase_servo_position(void);
void decrease_servo_position(void);

#define BUTTON_1_1 0XBA45
#define BUTTON_1_2 0XB946
#define BUTTON_1_3 0XB847

#define BUTTON_2_1 0XBB44
#define BUTTON_2_2 0XBF40
#define BUTTON_2_3 0XBC43

#define BUTTON_3_1 0XF807
#define BUTTON_3_2 0XEA15
#define BUTTON_3_3 0XF609

#define BUTTON_4_1 0XE916
#define BUTTON_4_2 0XE619
#define BUTTON_4_3 0XF20D

#define BUTTON_5_2 0XE718

#define BUTTON_6_1 0XF708
#define BUTTON_6_2 0XE31C
#define BUTTON_6_3 0XA55A

#define BUTTON_7_2 0XAD52

#define PERIOD_10MS 10000

#define START_MAX 14500
#define START_MIN 13500
#define BIT_1_MAX 2450
#define BIT_1_MIN 2250
#define BIT_0_MAX 1120
#define BIT_0_MIN 1340

#define PERIOD_20MS 20971
#define PERIOD_500US 524
#define STEP_10_DEGREES 117

volatile int index = 0;
volatile int pulse_period = 0;
volatile int capture_vector[32];
volatile unsigned long decoded_code;
volatile unsigned long button_code;
volatile int servo_position = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Disable watchdog timer
    configure_leds();
    configure_tb0_pwm();
    configure_ta1_capture();
    configure_servo();
    
    while (1) {
        set_ta1_falling_edge_capture();
        await_first_falling_edge();
        await_second_falling_edge();
        
        if ((pulse_period > START_MIN) && (pulse_period < START_MAX)) {
            set_ta1_rising_edge_capture();
            await_rising_edge();
            record_capture_values();
        }
        
        decode_vector_to_word();
        execute_instructions(button_code);
    }
}

void record_capture_values(void) {
    for (index = 0; index < 32; index++) {
        while (!(TA1CCTL1 & CCIFG));
        TA1CTL |= TACLR;
        TA1CCTL1 &= ~CCIFG;
        capture_vector[index] = TA1CCR1;
    }
}

void decode_vector_to_word(void) {
    for (index = 0; index < 32; index++) {
        decoded_code >>= 1;
        if ((capture_vector[index] > BIT_1_MIN) && (capture_vector[index] < BIT_1_MAX)) {
            decoded_code |= 0x80000000L;
        }
    }
    button_code = decoded_code >> 16;
}

void configure_ta1_capture(void) {
    TA1CTL = TASSEL_2 | MC_2; // SMCLK, Continuous mode
    P2DIR &= ~BIT0; // P2.0 as input
    P2REN |= BIT0; // Enable pull-up resistor
    P2OUT |= BIT0;
    P2SEL |= BIT0; // P2.0 as capture input for TA1.1
    TA1CCTL1 &= ~CCIFG;
}

void set_ta1_rising_edge_capture(void) {
    TA1CCTL1 = CM_1 | CCIS_0 | SCS | CAP;
    TA1CCTL1 &= ~CCIFG;
}

void set_ta1_falling_edge_capture(void) {
    TA1CCTL1 = CM_2 | CCIS_0 | SCS | CAP;
    TA1CCTL1 &= ~CCIFG;
}

void await_first_falling_edge(void) {
    while (!(TA1CCTL1 & CCIFG));
    TA1CTL |= TACLR;
    TA1CCTL1 &= ~CCIFG;
}

void await_second_falling_edge(void) {
    while (!(TA1CCTL1 & CCIFG));
    TA1CTL |= TACLR;
    TA1CCTL1 &= ~CCIFG;
    pulse_period = TA1CCR1;
}

void await_rising_edge(void) {
    while (!(TA1CCTL1 & CCIFG));
    TA1CTL |= TACLR;
    TA1CCTL1 &= ~CCIFG;
}

void configure_leds(void) {
    P1OUT &= ~BIT0;
    P1DIR |= BIT0;
    P4OUT &= ~BIT7;
    P4DIR |= BIT7;
    P4SEL |= BIT7;
    PMAPKEYID = 0X02D52;
    P4MAP7 = PM_TB0CCR1A;
}

void increase_servo_position(void) {
    servo_position++;
    if (servo_position > 18) servo_position = 18;
    TA2CCR2 = PERIOD_500US + servo_position * STEP_10_DEGREES;
}

void decrease_servo_position(void) {
    servo_position--;
    if (servo_position < 0) servo_position = 0;
    TA2CCR2 = PERIOD_500US + servo_position * STEP_10_DEGREES;
}

void execute_instructions(int button) {
    switch(button) {
        case BUTTON_1_1:
            P1OUT |= BIT0;
            break;

        case BUTTON_1_2:
            P1OUT &= ~BIT0;
            break;

        case BUTTON_1_3:
            P1OUT ^= BIT0;
            break;

        case BUTTON_2_1:
            TB0CCR1 = PERIOD_10MS;
            break;

        case BUTTON_2_2:
            TB0CCR1 = 0;
            break;

        case BUTTON_2_3:
            TB0CCR1 = PERIOD_10MS - TB0CCR1;
            debounce_delay(10000);
            break;

        case BUTTON_3_1:
            TB0CCR1 = PERIOD_10MS * 0.1;
            break;

        case BUTTON_3_2:
            TB0CCR1 = PERIOD_10MS * 0.2;
            break;

        case BUTTON_3_3:
            TB0CCR1 = PERIOD_10MS * 0.3;
            break;

        case BUTTON_4_1:
            TB0CCR1 = PERIOD_10MS * 0.4;
            break;

        case BUTTON_4_2:
            TB0CCR1 = PERIOD_10MS * 0.5;
            break;

        case BUTTON_4_3:
            TB0CCR1 = PERIOD_10MS * 0.6;
            break;

        case BUTTON_5_2:
            TB0CCR1 = PERIOD_10MS * 0.7;
            break;

        case BUTTON_6_1:
            decrease_servo_position();
            break;

        case BUTTON_6_2:
            TB0CCR1 = PERIOD_10MS * 0.8;
            break;

        case BUTTON_6_3:
            increase_servo_position();
            break;

        case BUTTON_7_2:
            TB0CCR1 = PERIOD_10MS * 0.9;
            break;
    }
}

void configure_tb0_pwm(void) {
    TB0CTL = TBSSEL_2 | MC_1; // TB0 with SMCLK and Up mode
    TB0CCR0 = PERIOD_10MS;
    TB0CCTL1 = OUTMOD_6;
    TB0CCR1 = PERIOD_10MS;
}

void configure_servo(void) {
    TA2CTL = TASSEL_2 | ID_0 | MC_1 | TACLR;
    TA2EX0 = TAIDEX_0;
    TA2CCR0 = PERIOD_20MS;
    TA2CCTL2 = OUTMOD_6;
    TA2CCR2 = PERIOD_500US;
    P2DIR |= BIT5;
    P2SEL |= BIT5;
}

void debounce_delay(int value) {
    for (index = 0; index < value; index++);
}
