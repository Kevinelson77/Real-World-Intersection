// Kevin Nelson
// Final Project 
// Microcontrollers - Fall 2020
// Real World Intersection - No Interrupts

#include "stm32f4xx.h"

#define RS 0x20     		// PA5 mask for reg select
#define RW 0x40     		// PA6 mask for read/write
#define EN 0x80     		// PA7 mask for enable

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(char datawrite);
void LCD_init(void);
void PORTS_init(void);
void urban_traffic_light(void);
void rural_traffic_light(void);
void blinking_yellow(void);
void crosswalk_countdown(void);
int  scroll_and_update_cursor(int);
char keypad_getkey(void);

int main(void) {

	int key;
	int scroll_cursor = 0;
	unsigned char mode_question[46] = "Select mode: 1.Urban 2.Rural 3.Blinking Yellow";
	unsigned char mode_1[11] = "Mode: Urban";
	unsigned char mode_2[11] = "Mode: Rural";
	unsigned char mode_3[20] = "Mode:Blinking Yellow";
	
	RCC->AHB1ENR = 7;          	// enable GPIOA/B/C clock
	RCC->APB2ENR = 0x4000;         	// enable SYSCFG clock
	LCD_init(); 			// initialize LCD controller
	
	for(int k=0; k<28; k++){
		LCD_data(mode_question[k]);		// Prints Mode Question
	}
	LCD_command(0xC0);
	for(int k=29; k<46; k++){
		LCD_data(mode_question[k]);
	}
	key = 0;
	int updated_key = 0;
	int should_scroll = 1;				// variable to see if the LCD should scroll
	int should_print_mode = 1; 
	while(1) {
		updated_key = keypad_getkey();
		if ((updated_key == 1 || updated_key == 2 || updated_key == 3) && updated_key != key) {
			should_print_mode = 1;
			key = updated_key;
		}
		if (should_scroll) { 
		  scroll_cursor = scroll_and_update_cursor(scroll_cursor);
		}
		if (key != 1 && key != 2 && key!=3) {
			continue;		
		}
		
		LCD_command(0x01);
		should_scroll = 0;
		if (key == 1)
			{						// if Urban Mode is selected
			if (should_print_mode) {			// Prints when 1(1 at the start)
			  for(int k=0; k<11; k++) {
			    LCD_data(mode_1[k]);			// Prints Urban mode
		    }
			  should_print_mode = 0;			// stops mode_1 from printing
		  }
			urban_traffic_light();				// Starts Urban Traffic Light
		} 
		else if (key == 2) {					// if Rural Mode is selected
			if (should_print_mode) {			// Prints when 1(1 at the start)
			  for(int k=0; k<11; k++) {
				  LCD_data(mode_2[k]);			// Prints Rural mode
	      }
			  should_print_mode = 0;			// stops mode_1 from printing
			}
			rural_traffic_light();				// Starts Rural Traffic Light
		}
		else if (key == 3) {					// if Rural Mode is selected
			if (should_print_mode) {			// Prints when 1(1 at the start)
				for(int k=0; k<14; k++) {
					LCD_data(mode_3[k]);		// Prints Blinking mode
				}
				LCD_command(0xC0);
				for(int k=14; k<20; k++) {
					LCD_data(mode_3[k]);
				}
			  should_print_mode = 0;			// stops mode_1 from printing
			}
			blinking_yellow();				// Starts Blinking Yellow Light
		}
		if((GPIOC->IDR & 0x0400) != 0x0400){			// If crosswalk push button is pushed
			crosswalk_countdown();				// Start displaying countdown
		}
	}
}

// scroll and update cursor
int scroll_and_update_cursor(int scroll_cursor) {
		delayMs(400);
		LCD_command(0x18);
		scroll_cursor += 1;
		if(scroll_cursor == 12) {
			delayMs(3000);
			LCD_command(0x02);
			scroll_cursor = 0;
		}
		return scroll_cursor;
}

// Urban Traffic Light
void urban_traffic_light(void) {
	 	GPIOA->MODER = 0xF00055FF; 		// clear PA8-13
		GPIOA->MODER = 0x55555555; 		// set pins to output mode (PA8-PA13)
		GPIOC->PUPDR = 0x00150055;	   	// pull up PC10
		GPIOA->ODR = 0x00002100; 		// turn on NSG(PA8), EWR(PA13)
			delayMs(5000);
		if((GPIOC->IDR & 0x0400) != 0x0400){
			GPIOA->ODR = 0x00002200; 	// turn on NSY(PA9), EWR(PA13)
			delayMs(2000);
			crosswalk_countdown();
			return;
		}
		GPIOA->ODR = 0x00002200; 		// turn on NSY(PA9), EWR(PA13)
			delayMs(2000);
		if((GPIOC->IDR & 0x0400) != 0x0400){
			crosswalk_countdown();
		}
		GPIOA->ODR = 0x0000C00; 		// turn on NSR(PA10), EWG(PA11)
			delayMs(5000);
		if((GPIOC->IDR & 0x0400) != 0x0400){
			GPIOA->ODR = 0x00001400; 	// turn on NSR(PA10), EWY(PA12)
			delayMs(2000);
			crosswalk_countdown();
			return;
		}
		GPIOA->ODR = 0x00001400; 		// turn on NSR(PA10), EWY(PA12)
			delayMs(2000);
}

// Rural Traffic Light
void rural_traffic_light(void){
	 	GPIOA->MODER = 0xF00055FF; 		// clear PA8-13
		GPIOA->MODER = 0x55555555; 		// set pins to output mode (PA8-PA13)
		
		GPIOA->ODR = 0x00002100;  		// turn on NSG(PA8), EWR(PA13)
			delayMs(5000);
		if((GPIOC->IDR & 0x0400) != 0x0400){
			GPIOA->ODR = 0x00002200; 	// turn on NSY(PA9), EWR(PA13)
			delayMs(2000);
			crosswalk_countdown();
			return;
		}
		while (GPIOC->IDR	&= 0x00000100){			// pin 8
			if((GPIOC->IDR & 0x0400) != 0x0400){
				GPIOA->ODR = 0x00002200; 		// turn on NSY(PA9), EWR(PA13)
				delayMs(2000);
				crosswalk_countdown();
		}
		GPIOA->ODR = 0x00002100;  				// turn on NSG(PA8), EWR(PA13)
			delayMs(5000);	
		}
		GPIOA->ODR =  0x00002200;  				// turn on NSY(PA9), EWR(PA13)
			delayMs(2000);
		if((GPIOC->IDR & 0x0400) != 0x0400){
			crosswalk_countdown();
		}
		GPIOA->ODR =  0x00000C00;  				// turn on NSR(PA10), EWG(PA11)
			delayMs(5000);	
		while(GPIOC->IDR	&= 0x00000200){			// pin 9
			if((GPIOC->IDR & 0x0400) != 0x0400){
				GPIOA->ODR =  0x00001400;  		// turn on NSR(PA10), EWY(PA12)
				delayMs(2000);
				crosswalk_countdown();
		}
		GPIOA->ODR =  0x00000C00;  				// turn on NSR(PA10), EWG(PA11)
			delayMs(5000);	
		}
		if((GPIOC->IDR & 0x0400) != 0x0400){
			GPIOA->ODR = 0x00001400; 			// turn on NSR(PA10), EWY(PA12)
			delayMs(2000);
			crosswalk_countdown();
			return;
		}		
		GPIOA->ODR =  0x00001400;  				// turn on NSR(PA10), EWY(PA12)
			delayMs(2000);	
}

// Countdown Timer for Pedestrian Crosswalk	
void crosswalk_countdown(void){		
unsigned int myCountdown[] = {0xD700, 0xFF00, 0x4700, 0xF500, 0xD500, 0xC600, 0x9F00, 0xB300, 0x0600, 0x7F00, 0x00, 0x7F00, 0x00, 0x7F00};

	GPIOA->ODR = 0x00002400; 				// turn on NSR(PA10), EWR(PA13)
		delayMs(2000);
	for(int i=0; i<14; i++) {
		GPIOB->ODR = myCountdown[i];
		delayMs(1000);
	}
}

// Blinking Yellow Light
void blinking_yellow(void){
		GPIOA->MODER = 0xF00055FF; 			// clear PA8-13
		GPIOA->MODER = 0x55555555; 			// set pins to output mode (PA8-PA13)
		
		GPIOA->ODR = 0x1200;				// Turns on NSY(PA9), EWY(PA12)
			delayMs(2000);
		GPIOA->ODR = 0x0000;				// Turns off NSY(PA9), EWY(PA12)
			delayMs(2000);
}

// initialize port pins then initialize LCD controller
void LCD_init(void) {
    PORTS_init();

    delayMs(30);            // initialization sequence
    LCD_command(0x30);
    delayMs(10);
    LCD_command(0x30);
    delayMs(1);
    LCD_command(0x30);

    LCD_command(0x38);      // set 8-bit data, 2-line, 5x7 font
    LCD_command(0x06);      // move cursor right after each char
    LCD_command(0x01);      // clear screen, move cursor to home
    LCD_command(0x0F);      // turn on display, cursor blinking
}

// Port Initializations for LCD. PA5-R/S, PA6-R/W, PA7-EN, PB0-PB7 for D0-D7, respectively.
void PORTS_init(void) {
    GPIOA->MODER = 0;    			// clear pin mode
    GPIOA->MODER = 0x55555555;    		// set pin output mode
    GPIOA->BSRR  = 0x00C00000;      		// turn off EN and R/W
    GPIOB->MODER = 0;    			// clear pin mode
    GPIOB->MODER = 0x55555555;    		// set pin output mode
		GPIOC->MODER = 0xFF000000;    	// clear pin mode to input
    GPIOC->PUPDR = 0x00150055;    		// enable pull up resistors for column pins, crosswalk & pressure pads
}

// Send command to LCD
void LCD_command(unsigned char command) {
    GPIOA->BSRR = (RS | RW) << 16;  // RS = 0, R/W = 0
    GPIOB->ODR = command;           // put command on data bus
    GPIOA->BSRR = EN;               // pulse E high
    delayMs(0);
    GPIOA->BSRR = EN << 16;         // clear E
    if (command < 4)
        delayMs(2);         	    // command 1 and 2 needs up to 1.64ms
    else
        delayMs(1);         				
}

// Write data to the LCD
void LCD_data(char datawrite) {
    GPIOA->BSRR = RS;               // RS = 1
    GPIOA->BSRR = RW << 16;         // R/W = 0
    GPIOB->ODR = datawrite;         // put data on data bus
    GPIOA->BSRR = EN;               // pulse E high
    delayMs(0);
    GPIOA->BSRR = EN << 16;         // clear E
    delayMs(1);
}

// Get key from keypad
char keypad_getkey(void) {
    int row, col;
    const int row_mode[] = {0x00000100, 0x00000400, 0x00001000, 0x00004000}; // one row is output
    const int row_low[] =  {0x00100000, 0x00200000, 0x00400000, 0x00800000}; // one row is low
    const int row_high[] = {0x00000010, 0x00000020, 0x00000040, 0x00000080}; // one row is high

    // check to see any key pressed
    GPIOC->MODER = 0x00005500;      // make all row pins output
    GPIOC->BSRR =  0x00F00000;      // drive all row pins low
    delayMs(1);                     // wait for signals to settle
    col = GPIOC->IDR & 0x000F;      // read all column pins
    GPIOC->MODER &= ~0x0000FF00;    // disable all row pins drive
    if (col == 0x000F)              // if all columns are high
        return 0;                   // no key pressed

// If a key is pressed, it gets here to find out which key. It activates one row at a time and 
// read the input to see which column is active.
    for (row = 0; row < 4; row++) {
        GPIOC->MODER &= ~0x0000FF00;    // disable all row pins drive
        GPIOC->MODER |= row_mode[row];  // enable one row at a time
        GPIOC->BSRR = row_low[row];     // drive the active row low
        delayMs(1);                     // wait for signal to settle
        col = GPIOC->IDR & 0x000F;      // read all columns
        GPIOC->BSRR = row_high[row];    // drive the active row high
        if (col != 0x000F) break;       // if one of the input is low, some key is pressed.
    }
    GPIOC->BSRR = 0x000000F0;           // drive all rows high before disable them
    GPIOC->MODER &= ~0x0000FF00;        // disable all rows
    if (row == 4)
        return 0;                       // if we get here, no key is pressed

    // gets here when one of the rows has key pressed, check which column it is
    if (col == 0x000E) return row * 4 + 1;    // key in column 0
    if (col == 0x000D) return row * 4 + 2;    // key in column 1
    if (col == 0x000B) return row * 4 + 3;    // key in column 2
    if (col == 0x0007) return row * 4 + 4;    // key in column 3

    return 0;   // just to be safe
}

// Delay timer
void delayMs(int n) {
    int i;

    // Configure SysTick
    SysTick->LOAD = 16000;  // reload with number of clocks per millisecond
    SysTick->VAL = 0;       // clear current value register
    SysTick->CTRL = 0x5;    // Enable the timer

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) // wait until the COUNTFLAG is set
            { }
    }
    SysTick->CTRL = 0;      // Stop the timer (Enable = 0)
}
