#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile bool timer_fired = false;
void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN_R)
            flag_f_r = 1;

    } else if (events == 0x8) { // rise edge
        if (gpio == BTN_PIN_R)
            flag_f_r = 0;
    }
}
int64_t alarm_callback(alarm_id_t id, void *user_data) {
    timer_fired = true;

    // Can return a value here in us to fire in the future
    return 0;
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
    alarm_id_t alarm ;
    while (true) {
        if (flag_f_r) {
            alarm = add_alarm_in_ms(500, alarm_callback, NULL, false);    
            if (!alarm) {
                printf("Failed to add timer\n");
            }
            if(timer_fired){
                gpio_put(LED_PIN_R,!gpio_get(LED_PIN_R));
                flag_f_r = 0;
                timer_fired = false;
            }
        }
        if(!flag_f_r&&timer_fired){
            cancel_alarm(alarm);
            timer_fired = false;

        }

    }
}
