#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int flag_timer = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = 1;
    } else if (events == 0x8) { // rise edge
    }
}

bool timer_callback(repeating_timer_t *rt) {
    flag_timer = 1;
    return true;
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    
    repeating_timer_t timer;
    int timer_exist = 0;
    int flag_last = 0;
    int led_state = 0;

    gpio_put(LED_PIN_R, led_state);

    while (true) {
        if (flag_f_r) {
            flag_f_r = 0;
            if (!timer_exist) {
                if (!add_repeating_timer_ms(500, timer_callback, NULL, &timer)) {
                    printf("Failed to add timer\n");
                } else {
                    timer_exist = 1;
                }
            } else {
                cancel_repeating_timer(&timer);
                timer_exist = 0;
                flag_last = 1;
                flag_timer = 0;
            }
        }

        if (flag_timer) {
            flag_timer = 0;
            gpio_put(LED_PIN_R, !led_state); 
            led_state = !led_state;
        }

        if (flag_last) {
            flag_last = 0;
            gpio_put(LED_PIN_R, 0);
            led_state = 0;
        }
    }
}
