#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include <stdint.h>

// DHT22
#define DHT 4

// LDR
#define LDR_CHANNEL ADC1_CHANNEL_6  // GPIO34

#define led_vermelho 2

// --------------------
int ler_dht22(int *temp, int *umi) {

    uint8_t data[5] = {0};

    gpio_set_direction(DHT, GPIO_MODE_OUTPUT);

    gpio_set_level(DHT, 0);
    esp_rom_delay_us(1000);

    gpio_set_level(DHT, 1);
    esp_rom_delay_us(30);

    gpio_set_direction(DHT, GPIO_MODE_INPUT);

    int timeout = 10000;
    while (gpio_get_level(DHT) == 1 && timeout--);
    timeout = 10000;
    while (gpio_get_level(DHT) == 0 && timeout--);
    timeout = 10000;
    while (gpio_get_level(DHT) == 1 && timeout--);

    for (int i = 0; i < 40; i++) {

        timeout = 10000;
        while (gpio_get_level(DHT) == 0 && timeout--);

        int start = esp_timer_get_time();

        timeout = 10000;
        while (gpio_get_level(DHT) == 1 && timeout--);

        int duracao = esp_timer_get_time() - start;

        data[i / 8] <<= 1;
        if (duracao > 40) data[i / 8] |= 1;
    }

    *umi = data[0];
    *temp = data[2];

    return 1;
}

// --------------------
int ler_ldr() {
    return adc1_get_raw(LDR_CHANNEL);
}

// --------------------
void app_main(void) {

    gpio_set_direction(DHT, GPIO_MODE_INPUT);
    gpio_set_direction(led_vermelho, GPIO_MODE_OUTPUT);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LDR_CHANNEL, ADC_ATTEN_DB_11);

    int temp = 0;
    int umi = 0;
    int luz = 0;
    int state = 0;

    while (1) {

        ler_dht22(&temp, &umi);
        luz = ler_ldr();

        if (temp < 0) {
            printf("Erro no DHT22\n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        // lógica
        if (temp < 30 && umi < 60 && luz < 2000) {
            state = 0;
        }
        else if (temp < 40 && umi < 80 && luz < 4000) {
            state = 1;
        }
        else {
            state = 2;
        }

        // saída

        if (state == 0) {
            printf("Estado: NORMAL\n");
            gpio_set_level(led_vermelho, 0);
        }
        else if (state == 1) {
            printf("Estado: ATENCAO\n");
                        gpio_set_level(led_vermelho, 0);
        }
        else {
            printf("Estado: CRITICO\n");
            printf("Temperatura: %d\n", temp);
            printf("Umidade: %d\n", umi);
            printf("Luminosidade: %d\n", luz);
                        gpio_set_level(led_vermelho, 1);
        }

        printf("\n");

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}