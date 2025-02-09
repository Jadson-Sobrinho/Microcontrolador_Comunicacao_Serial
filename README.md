
# Documentação do Código

Este código foi desenvolvido para controlar uma matriz de LEDs, interagir com um display OLED e gerenciar animações via interrupções e botões. Abaixo estão os detalhes de cada parte importante do código, incluindo a configuração de pinos, animações, controle de LEDs e o uso do display.

---

## Dependências

O código depende das seguintes bibliotecas e módulos:

- `pico/stdlib.h` - Biblioteca padrão para o Raspberry Pi Pico.
- `hardware/pio.h`, `hardware/clocks.h`, `hardware/adc.h` - Funções para controlar os periféricos do hardware, como PIO, clocks e ADC.
- `pio_matrix.pio.h` - Arquivo gerado para o controle da matriz de LEDs via PIO.
- `ssd1306.h`, `ssd1306_font.h` - Bibliotecas para controle do display OLED SSD1306.
- `hardware/i2c.h` - Biblioteca para controle do barramento I2C.
- `lib/animacao_X.h` - Arquivos contendo os quadros de animações predefinidas.

---

## Definições de Pinos e Variáveis

### Pinos de Entrada e Saída

- `GPIO_LED_RED`, `GPIO_LED_GREEN`, `GPIO_LED_BLUE` - Pinos dos LEDs RGB (13, 11, 12).
- `button_0` e `button_1` - Pinos para os botões de interrupção (5 e 6).
- `I2C_SDA`, `I2C_SCL` - Pinos para comunicação I2C (14 e 15).

### Variáveis Globais

- `led_on` - Controla o estado do LED (ligado ou desligado).
- `animacao_atual` - Índice da animação atual a ser exibida.
- `display_update_flag` - Flag para atualizar o display.
- `display_message_type` - Tipo da mensagem a ser exibida no display.

---

## Funções

### `matrix_rgb(double r, double g, double b)`

Função que converte valores de cor RGB para um formato de 32 bits que pode ser usado para controlar os LEDs.

- **Entrada:** Três valores de cor: `r`, `g`, `b`.
- **Saída:** Valor RGB codificado.

### `desenho_pio(double* desenho, uint32_t valor_led, PIO pio, uint sm)`

Atualiza os LEDs de acordo com os quadros de animação.

- **Entrada:**
  - `desenho`: Array de valores que representam o desenho da animação.
  - `valor_led`: Valor RGB para a cor dos LEDs.
  - `pio`: PIO que controla os LEDs.
  - `sm`: State machine (SM) para o PIO.
- **Saída:** Nenhuma, mas os LEDs são atualizados.

### `exibir_animacao(double* animacao[], int num_desenhos, uint32_t valor_led, PIO pio, uint sm)`

Exibe uma animação se ativada.

- **Entrada:**
  - `animacao`: Array de quadros da animação.
  - `num_desenhos`: Número de quadros da animação.
  - `valor_led`: Cor a ser exibida nos LEDs.
  - `pio` e `sm`: Controle do PIO para atualizar os LEDs.
- **Saída:** Nenhuma, mas a animação é exibida.

### `executar_animacao(int animacao_idx, uint32_t valor_led, PIO pio, uint sm)`

Executa uma animação selecionada.

- **Entrada:**
  - `animacao_idx`: Índice da animação a ser executada.
  - `valor_led`: Cor dos LEDs.
  - `pio` e `sm`: Controle do PIO.
- **Saída:** Nenhuma, mas a animação é exibida.

### `get_key()`

Lê uma tecla pressionada na entrada serial de forma não bloqueante.

- **Entrada:** Nenhuma.
- **Saída:** O caractere da tecla pressionada ou `0` se nenhuma tecla foi pressionada.

### `gpio_irq_handler(uint gpio, uint32_t events)`

Função de interrupção para os botões. Quando um botão é pressionado, a animação é ativada/desativada e os LEDs são ligados/desligados.

- **Entrada:**
  - `gpio`: O pino do botão que gerou a interrupção.
  - `events`: Os eventos de interrupção.
- **Saída:** Nenhuma, mas os LEDs podem ser atualizados e a animação pode ser ativada/desativada.

---

## Lógica de Funcionamento

1. **Inicialização do Sistema:**
   - Configura os pinos dos LEDs, botões e comunicação I2C.
   - Inicializa o display OLED SSD1306 e a matriz de LEDs com a configuração inicial.

2. **Interrupções:**
   - O código usa interrupções para monitorar os botões e ativar ou desativar os LEDs. Os botões controlam o estado dos LEDs e também o tipo de mensagem exibida no display OLED.

3. **Animações:**
   - O código contém várias animações definidas nos arrays `animacao_X`. Cada animação é composta por vários quadros (frames) que são exibidos na matriz de LEDs.
   - O usuário pode selecionar a animação a ser exibida pressionando as teclas 0 a 9 no monitor serial.

4. **Display OLED:**
   - O display OLED é atualizado com mensagens relacionadas ao estado dos LEDs (ligados/desligados) ou a tecla pressionada.
   - A atualização do display ocorre sempre que uma alteração no estado dos LEDs ou na animação é feita.

---

## Fluxo de Execução

1. O código inicializa o hardware (botões, LEDs, display OLED e PIO).
2. Monitora a entrada serial para permitir a seleção da animação.
3. Monitora as interrupções dos botões para controlar os LEDs e atualizar o display OLED.
4. Quando uma animação é solicitada, ela é executada e exibida na matriz de LEDs.
5. O display OLED exibe mensagens relacionadas ao estado atual do sistema.

---

## Conclusão

Este código é um exemplo de controle de uma matriz de LEDs, com interação através de botões e um display OLED. Ele pode ser adaptado para exibir diferentes animações ou mensagens no display.
