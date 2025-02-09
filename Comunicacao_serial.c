#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include <math.h>
#include "lib/animacao_0.h"
#include "lib/animacao_1.h"
#include "lib/animacao_2.h"
#include "lib/animacao_3.h"
#include "lib/animacao_4.h"
#include "lib/animacao_5.h"
#include "lib/animacao_6.h"
#include "lib/animacao_7.h"
#include "lib/animacao_8.h"
#include "lib/animacao_9.h"
#include "pio_matrix.pio.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_font.h"
#include "hardware/i2c.h"
#include <ctype.h>
#include <string.h>


#define NUMERO_DE_LEDS 25
#define OUT_PINO 7

//GPIO LEDS
#define GPIO_LED_RED 13
#define GPIO_LED_GREEN 11
#define GPIO_LED_BLUE 12
#define DEBOUNCE_TIME_MS 200  // Tempo mínimo entre leituras válidas

uint32_t last_press_time_0 = 0;  // Última ativação do botão_0
uint32_t last_press_time_1 = 0;  // Última ativação do botão_1

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

//botão de interupção
const uint button_0 = 5;
const uint button_1 = 6;


// Variáveis globais para controlar o estado do LED e o tempo
bool led_on = false;
unsigned long last_time = 0;
bool animacao_ativa = false; // Estado da animação

int animacao_atual = 0;  // Começa na animação 0
const int num_animacoes = 10; // Número total de animações disponíveis

// Variáveis voláteis para sinalizar atualização do display (usadas na IRQ)
volatile bool display_update_flag = false;
volatile int display_message_type = 0; 


// Função para gerar cores RGB para matriz de LEDs
uint32_t matrix_rgb(double r, double g, double b) {
    return ((int)(g * 255) << 24) | ((int)(r * 255) << 16) | ((int)(b * 255) << 8);
}

// Atualiza os LEDs da matriz
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm) {
    for (int i = 0; i < NUMERO_DE_LEDS; i++) {
        valor_led = matrix_rgb(desenho[24 - i], desenho[24 - i], desenho[24 - i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Exibe animação caso ativada
void exibir_animacao(double* animacao[], int num_desenhos, uint32_t valor_led, PIO pio, uint sm) {
    if (!animacao_ativa) return; // Só roda se estiver ativada
    for (int i = 0; i < num_desenhos; i++) {
        desenho_pio(animacao[i], valor_led, pio, sm);
        sleep_ms(100);
    }
}


double* animacao_0[] = {desenho1, desenho2, desenho3, desenho4, desenho5, desenho6, desenho7, desenho8, desenho9};
int num_desenhos = sizeof(animacao_0) / sizeof(animacao_0[0]);

double* animacao_1[] = {frame_1_1, frame_1_2, frame_1_3, frame_1_4, frame_1_5, frame_1_6, frame_1_7};
int num_frames = sizeof(animacao_1) / sizeof(animacao_1[0]);

double* animacao_2[] = {frame0, frame1, frame2, frame3, frame4, frame5, frame6, frame7, frame8, frame9};
int num_desenhos2 = sizeof(animacao_2) / sizeof(animacao_2[0]);

double* animacao_3[] = {frame00, frame01, frame02, frame03, frame04, frame05, frame06, frame07, frame08, frame09};
int num_desenhos_3 = sizeof(animacao_3) / sizeof(animacao_3[0]);

double* animacao_4[] = {desenho1_1, desenho2_2, desenho3_3, desenho4_4, desenho5_5, desenho6_6, desenho7_7, desenho8_8, desenho9_9, desenho10_10, desenho11_11, desenho12_12, desenho13_13, desenho14_14, desenho15_15};
int num_desenhos_4 = sizeof(animacao_4) / sizeof(animacao_4[0]);

double* animacao_5[] = {quadro0, quadro1, quadro2, quadro3, quadro4, quadro5, quadro6, quadro7, quadro8, quadro9, quadro10, quadro11, quadro12, quadro13, quadro14, quadro15, quadro16};
int num_desenhos_5 = sizeof(animacao_5) / sizeof(animacao_5[0]);

double* animacao_6[] = {frame1_6, frame2_6, frame3_6, frame4_6};
int num_desenhos_6 = sizeof(animacao_6) / sizeof(animacao_6[0]);

double* animacao_7[] = {frame1_7, frame2_7, frame3_7, frame4_7, frame5_7, frame6_7};
int num_desenhos_7 = sizeof(animacao_7) / sizeof(animacao_7[0]);

double* animacao_8[] = {frame1_8, frame2_8, frame3_8, frame4_8, frame5_8, frame6_8};
int num_desenhos_8 = sizeof(animacao_8) / sizeof(animacao_8[0]);

double* animacao_9[] = {frame1_9};
int num_desenhos_9 = sizeof(animacao_9) / sizeof(animacao_9[0]);


void executar_animacao(int animacao_idx, uint32_t valor_led, PIO pio, uint sm) {
    switch (animacao_idx) {
        
        case 0:
            exibir_animacao(animacao_0, num_desenhos, valor_led, pio, sm);
            break;
        case 1:
            exibir_animacao(animacao_1, num_frames, valor_led, pio, sm);
            break;
        case 2:
            exibir_animacao(animacao_2, num_desenhos2, valor_led, pio, sm);
            break;
        case 3:
            exibir_animacao(animacao_3, num_desenhos_3, valor_led, pio, sm);
            break;  
        case 4:
            exibir_animacao(animacao_4, num_desenhos_4, valor_led, pio, sm);
            break;
        case 5:
            exibir_animacao(animacao_5, num_desenhos_5, valor_led, pio, sm);
            break;
        case 6:
            exibir_animacao(animacao_6, num_desenhos_6, valor_led, pio, sm);
            break;
        case 7:
            exibir_animacao(animacao_7, num_desenhos_7, valor_led, pio, sm);
            break;            
        case 8:
            exibir_animacao(animacao_8, num_desenhos_8, valor_led, pio, sm);
            break;
        case 9:
            exibir_animacao(animacao_9, num_desenhos_9, valor_led, pio, sm);
            break;            
        default:
            printf("Animação inválida\n");
    }
}

// Função para leitura não bloqueante da entrada serial
char get_key() {
    int ch = getchar_timeout_us(0); // Tenta ler sem bloquear
    if (ch != PICO_ERROR_TIMEOUT) {
        return (char) ch;
    }
    return 0;
}

// Interrupção do botão para ativar/desativar animação
static void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual em ms

    if (gpio == button_0 && led_on == false) {
        if (current_time - last_press_time_0 < DEBOUNCE_TIME_MS) return; // Ignora se estiver dentro do tempo de debounce
        last_press_time_0 = current_time; // Atualiza o tempo do último acionamento

        gpio_put(GPIO_LED_GREEN, 1);
        led_on = true;
        printf("Led verde ligado\n");
        display_message_type = 1;           // Código para "Led verde Ligado"
        display_update_flag = true;

    
    } 
    else if (gpio == button_0 && led_on == true) {
        
        gpio_put(GPIO_LED_GREEN, 0);
        led_on = false;
        printf("Led verde desligado\n");
        display_message_type = 2;           // Código para "Led verde Desligado"
        display_update_flag = true;
    }
    else if (gpio == button_1 && led_on == false) {
        if (current_time - last_press_time_1 < DEBOUNCE_TIME_MS) return;
        last_press_time_1 = current_time;

        gpio_put(GPIO_LED_BLUE, 1);
        led_on = true;
        printf("Led azul ligado\n");
        display_message_type = 3;           // Código para "Led azul Ligado"
        display_update_flag = true;

    }
    else if (gpio == button_1 && led_on == true) {
        
        gpio_put(GPIO_LED_BLUE, 0);
        led_on = false;
        printf("Led azul desligado\n");
        display_message_type = 4;           // Código para "Led azul Desligado"
        display_update_flag = true;
    }    

}


int main() {
    PIO pio = pio0;
    uint32_t valor_led = 0;
    double r = 0.0, b = 0.0, g = 0.0;

    set_sys_clock_khz(128000, false);
    stdio_init_all();

    // Configurar interrupção no botão
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(button_1, GPIO_IRQ_EDGE_FALL, true);


    //Inicialização dos LEDs
    gpio_init(GPIO_LED_RED);
    gpio_init(GPIO_LED_GREEN);
    gpio_init(GPIO_LED_BLUE);

    //Definindo os LEDs como saída
    gpio_set_dir(GPIO_LED_RED, GPIO_OUT);
    gpio_set_dir(GPIO_LED_GREEN, GPIO_OUT);
    gpio_set_dir(GPIO_LED_BLUE, GPIO_OUT);

    //inicializar o botão de interrupção - GPIO5
    gpio_init(button_0);
    gpio_set_dir(button_0, GPIO_IN);
    gpio_pull_up(button_0);

    //inicializar o botão de interrupção - GPIO6
    gpio_init(button_1);
    gpio_set_dir(button_1, GPIO_IN);
    gpio_pull_up(button_1);

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PINO);

    //Inicializa a matriz de leds desligados
    desenho_pio(desenho0, valor_led, pio, sm);

    // Declaração de uma estrutura de temporizador de repetição.
    // Esta estrutura armazenará informações sobre o temporizador configurado.
    struct repeating_timer timer;

        // Inicialização do i2c
        i2c_init(i2c1, ssd1306_i2c_clock * 1000);
        gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
        gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
        gpio_pull_up(I2C_SDA);
        gpio_pull_up(I2C_SCL);
    
        // Processo de inicialização completo do OLED SSD1306
        ssd1306_init();
    
    // Configura a área de renderização do display
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    
    // Cria um buffer para o display e limpa o display
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    
    while (true) {
    // Lê a tecla do Serial Monitor
    char key = get_key();
    if (key != 0) {
        if (key >= '0' && key <= '9') {
            animacao_atual = key - '0';
            animacao_ativa = true;
        } 
        
        // Atualizar o display com a tecla pressionada
        memset(ssd, 0, ssd1306_buffer_length);  // Limpa o display
        char msg[26];  // Buffer para a mensagem a ser exibida
        snprintf(msg, sizeof(msg), "Tecla: %c", key);
        ssd1306_draw_string(ssd, 5, 0, msg);  // Exibe a tecla pressionada
        render_on_display(ssd, &frame_area);  // Atualiza o display
    }

        // Se a flag de atualização do display estiver setada, atualiza o display com a mensagem
        if (display_update_flag) {
            memset(ssd, 0, ssd1306_buffer_length);
            
            if (display_message_type == 1) {
                ssd1306_draw_string(ssd, 5, 0, "Led verde Ligado");
            } 
            else if (display_message_type == 2) {
                ssd1306_draw_string(ssd, 5, 0, "Led verde Desligado");
            } 
            else if (display_message_type == 3)
            {
                ssd1306_draw_string(ssd, 5, 0, "Led azul ligado");
            } 
            else if (display_message_type == 4)
            {
                ssd1306_draw_string(ssd, 5, 0, "Led azul Desligado");
            }
            
            
            render_on_display(ssd, &frame_area);
            display_update_flag = false;
        }

        // Se uma animação foi solicitada, executa-a
        if (animacao_ativa) {
            executar_animacao(animacao_atual, valor_led, pio, sm);
        }
        sleep_ms(10);
    }
    return 0;
}