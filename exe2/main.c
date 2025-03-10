#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int flag_timer_r = 0;
volatile int flag_timer_g = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_callback_r(repeating_timer_t *rt) {
    flag_timer_r = 1;
    return true;
}

bool timer_callback_g(repeating_timer_t *rt) {
    flag_timer_g = 1;
    return true;
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    repeating_timer_t timer_r;
    repeating_timer_t timer_g;
    int timer_exist_r = 0;
    int timer_exist_g = 0;
    int flag_last_r = 0;
    int flag_last_g = 0;
    int led_state_r = 0;
    int led_state_g = 0;

    gpio_put(LED_PIN_R, led_state_r);
    gpio_put(LED_PIN_G, led_state_g);

    while (true) {

        if (flag_r) {
            flag_r = 0;
            if (!timer_exist_r) {
                if (!add_repeating_timer_ms(500, timer_callback_r, NULL, &timer_r)) {
                    printf("Failed to add timer\n");
                } else {
                    timer_exist_r = 1;
                }
            } else {
                cancel_repeating_timer(&timer_r);
                timer_exist_r = 0;
                flag_last_r = 1;
                flag_timer_r = 0;
            }
        }

        if (flag_g) {
            flag_g = 0;
            if (!timer_exist_g) {
                if (!add_repeating_timer_ms(250, timer_callback_g, NULL, &timer_g)) {
                    printf("Failed to add timer\n");
                } else {
                    timer_exist_g = 1;
                }
            } else {
                cancel_repeating_timer(&timer_g);
                timer_exist_g = 0;
                flag_last_g = 1;
                flag_timer_g = 0;
            }
        }

        if (flag_timer_r) {
            flag_timer_r = 0;
            gpio_put(LED_PIN_R, !led_state_r);
            led_state_r = !led_state_r;
        }

        if (flag_timer_g) {
            flag_timer_g = 0;
            gpio_put(LED_PIN_G, !led_state_g);
            led_state_g = !led_state_g;
        }

        if (flag_last_r) {
            flag_last_r = 0;
            gpio_put(LED_PIN_R, 0);
            led_state_r = 0;
        }

        if (flag_last_g) {
            flag_last_g = 0;
            gpio_put(LED_PIN_G, 0);
            led_state_g = 0;
        }
    }
}
