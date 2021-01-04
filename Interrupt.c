
/* p6_1.c using SW0 interrupt
 *
 * User Switch B1 is used to generate interrupt through PC13.
 * The user button is connected to PC13. It has a pull-up resitor 
 * so PC13 stays high when the button is not pressed.
 * When the button is pressed, PC13 becomes low.
 * The falling-edge of PC13 (when switch is pressed) triggers an
 * interrupt from External Interrupt Controller (EXTI).
 * In the interrupt handler, the user LD2 is blinked twice.
 * It serves as a crude way to debounce the switch.
 * The green LED (LD2) is connected to PA5.
 *
 * This program was tested with Keil uVision v5.24a with DFP v2.11.0.
 */

#include "stm32f4xx.h"

void delayMs(int n);

int main(void) {
    __disable_irq();                    /* global disable IRQs */

    RCC->AHB1ENR |= 4;	                /* enable GPIOC clock */
    RCC->AHB1ENR |= 1;                  /* enable GPIOA clock */
    RCC->APB2ENR |= 0x4000;             /* enable SYSCFG clock */

    /* configure PA5 for LED */
    GPIOA->MODER &= ~0x00000C00;        /* clear pin mode */
    GPIOA->MODER |=  0x00000400;        /* set pin to output mode */

    /* configure PC13 for push button interrupt */
    GPIOC->MODER &= ~0x0C000000;        /* clear pin mode to input mode */
    
    SYSCFG->EXTICR[3] &= ~0x00F0;       /* clear port selection for EXTI13 */
    SYSCFG->EXTICR[3] |= 0x0020;        /* select port C for EXTI13 */
    
    EXTI->IMR |= 0x2000;                /* unmask EXTI13 */
    EXTI->FTSR |= 0x2000;               /* select falling edge trigger */

//    NVIC->ISER[1] = 0x00000100;         /* enable IRQ40 (bit 8 of ISER[1]) */
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    
    __enable_irq();                     /* global enable IRQs */
    
    while(1) {
    }
}

void EXTI15_10_IRQHandler(void) {
        GPIOA->BSRR = 0x00000020;   /* turn on green LED */
        delayMs(250);
        GPIOA->BSRR = 0x00200000;   /* turn off green LED */
        delayMs(250);
        GPIOA->BSRR = 0x00000020;   /* turn on green LED */
        delayMs(250);        
        GPIOA->BSRR = 0x00200000;   /* turn off green LED */
        delayMs(250);

        EXTI->PR = 0x2000;          /* clear interrupt pending flag */
}

void delayMs(int n) {
    int i;


    /* Configure SysTick */
    SysTick->LOAD = 16000;  /* reload with number of clocks per millisecond */
    SysTick->VAL = 0;       /* clear current value register */
    SysTick->CTRL = 0x5;    /* Enable the timer */


    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNTFLAG is set */
            { }
    }
    SysTick->CTRL = 0;      /* Stop the timer (Enable = 0) */
}
