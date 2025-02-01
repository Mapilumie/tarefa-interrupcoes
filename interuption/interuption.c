#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"

//arquivo .pio
#include "interuption.pio.h"

// Define GPIO pins
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define LED_RGB_RED_PIN 11
#define LED_RGB_GREEN_PIN 12
#define LED_RGB_BLUE_PIN 13
#define WS2812_PIN 7

// Global variables
volatile int number = 0; // Number to display
volatile bool button_a_pressed = false;
volatile bool button_b_pressed = false;

double number_1[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double number_2[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double number_3[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double number_4[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double number_5[25] = {
    1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 1.0
};

double number_6[25] = {
    1.0, 0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 1.0, 0.0, 1.0
};

double number_7[25] = {
    1.0, 0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 1.0, 0.0, 1.0
};

double number_8[25] = {
    1.0, 0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 1.0, 0.0, 1.0
};

double number_9[25] = {
    1.0, 0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 1.0, 0.0, 1.0
};

// Function prototypes
void button_a_isr();
void button_b_isr();
void display_number(int num);
void debounce();
void init_all_pins();

int main() {
    // Initialize stdio
    stdio_init_all();
    init_all_pins();// Initialize all pins

    // Set up interrupts
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_a_isr);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &button_b_isr);

    // Initialize WS2812
    ws2812_init(WS2812_PIN);

    // Main loop
    while (true) {
        // Blink the red LED continuously at 5 Hz
        gpio_put(LED_RGB_RED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_RGB_RED_PIN, 0);
        sleep_ms(100);

        // Check for button presses
        debounce();
        if (button_a_pressed) {
            number++;
            display_number(number);
            button_a_pressed = false;
        }
        if (button_b_pressed) {
            number--;
            display_number(number);
            button_b_pressed = false;
        }
    }
}

//************************************ */

void init_all_pins() {
    // Initialize all pins
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_init(LED_RGB_RED_PIN);
    gpio_init(LED_RGB_GREEN_PIN);
    gpio_init(LED_RGB_BLUE_PIN);
    gpio_init(WS2812_PIN);

    // Set directions
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_set_dir(LED_RGB_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_RGB_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(LED_RGB_BLUE_PIN, GPIO_OUT);
    gpio_set_dir(WS2812_PIN, GPIO_OUT);

    // Set pull-up resistors
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);
}

// Interrupt Service Routine for Button A
void button_a_isr() {
    button_a_pressed = true;
}

// Interrupt Service Routine for Button B
void button_b_isr() {
    button_b_pressed = true;
}

// Debounce function
void debounce() {
    static uint32_t last_interrupt_time = 0;
    uint32_t interrupt_time = to_ms_since_boot(get_absolute_time());

    if (interrupt_time - last_interrupt_time > 200) { // 200 ms debounce time
        last_interrupt_time = interrupt_time;
    } else {
        button_a_pressed = false;
        button_b_pressed = false;
    }
}

// Function to display number on WS2812
void display_number(int num) {
    // Ensure the number is between 0 and 9
   if(num>9){
       num=0; 
    } 

    switch (num)
    {
    case 1:
        
        break;
    

    
    default:
        break;
    }
    // Convert number to LED pattern (example implementation)
    //ws2812_set_color(WS2812_PIN, num); // This function should set the color based on the number
}