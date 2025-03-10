#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN_R)
            flag_f_r = 1;

    } else if (events == 0x8) { // rise edge
        if (gpio == BTN_PIN_R)
            flag_f_r = 0;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    int led_state = 0;
    gpio_put(LED_PIN_R, led_state);
    
    while (true) {
        if (flag_f_r) {
            uint32_t start_ms = to_ms_since_boot(get_absolute_time());
            while (flag_f_r) {
                if (to_ms_since_boot(get_absolute_time()) - start_ms > 500) {
                    led_state = !led_state;
                    gpio_put(LED_PIN_R, led_state);
                    flag_f_r = 0;
                }
            }
        }
    }
}
