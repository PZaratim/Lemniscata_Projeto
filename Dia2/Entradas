#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include <stdint.h>

// DHT22
#define DHT 4

// LDR
#define LDR_CHANNEL ADC1_CHANNEL_6  // GPIO34

#define led_vermelho 2

// --------------------
// SERVO
#define SERVO_PIN 19

//Valores
    int temp = 0;
    int umi = 0;
    int luz = 0;
    int state = 0;

// -------------------- SERVO --------------------
uint32_t angle_to_duty(int angle) {
    uint32_t min_us = 500;
    uint32_t max_us = 2500;
    uint32_t pulse = min_us + ((max_us - min_us) * angle) / 180;
    return (pulse * 65535) / 20000;
}

void servo_init() {
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = SERVO_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel);
}

void servo_write(int angle) {
    uint32_t duty = angle_to_duty(angle);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void servo_task(void *pvParameters) {
    while (1) {
        for (int p = 0; p <= 180; p++) {
            servo_write(p);
            vTaskDelay(pdMS_TO_TICKS(15));
        }
        for (int p = 180; p >= 0; p--) {
            servo_write(p);
            vTaskDelay(pdMS_TO_TICKS(15));
        }
    }
}

// --------------------
// Seu código original do DHT22 + LDR + LED
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

int ler_ldr() {
    return adc1_get_raw(LDR_CHANNEL);
}

void app_main(void) {

    gpio_set_direction(DHT, GPIO_MODE_INPUT);
    gpio_set_direction(led_vermelho, GPIO_MODE_OUTPUT);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LDR_CHANNEL, ADC_ATTEN_DB_11);

    // Inicializa servo e cria task separada
    servo_init();
    xTaskCreate(servo_task, "servo_task", 2048, NULL, 1, NULL);



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

        printf("----------------------------------\n");

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
