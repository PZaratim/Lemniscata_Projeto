#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//Motor A
#define IN1 18
#define IN2 19

//Motor B
#define IN3 21
#define IN4 22

//LDR
#define LDR_A 32
#define LDR_B 33

// --- Motor A ---
void motorA_frente() {   // gira Motor A para frente
    gpio_set_level(IN1, 1);
    gpio_set_level(IN2, 0);
}

void motorA_re() {       // gira Motor A para trás
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 1);
}

void motorA_parar() {    // para Motor A
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 0);
}

// --- Motor B ---
void motorB_frente() {   // gira Motor B para frente
    gpio_set_level(IN3, 1);
    gpio_set_level(IN4, 0);
}

void motorB_re() {       // gira Motor B para trás
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 1);
}

void motorB_parar() {    // para Motor B
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 0);
}

void app_main() {

    // Configuração dos pinos dos motores
    gpio_set_direction(IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN3, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN4, GPIO_MODE_OUTPUT);

    // Configuração dos pinos dos LDRs
    gpio_set_direction(LDR_A, GPIO_MODE_INPUT);
    gpio_set_direction(LDR_B, GPIO_MODE_INPUT);

    while (1) {
        // Lê os sensores
        int A = gpio_get_level(LDR_A); // 0 = linha preta
        int B = gpio_get_level(LDR_B); // 0 = linha preta

        // --- Lógica seguidor de linha ---
        if (A == 0 && B == 0) {            // Reto
            motorA_frente();
            motorB_frente();
        }
        else if (A == 1 && B == 0) {       // Corrige para a esquerda
            motorA_parar();
            motorB_frente();
        }
        else if (A == 0 && B == 1) {       // Corrige para a direita
            motorA_frente();
            motorB_parar();
        }
        else {                              // A==1 && B==1 → perdeu linha
            motorA_parar();
            motorB_parar();
        }

        // Pequena pausa para estabilidade
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}