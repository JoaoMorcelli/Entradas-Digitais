//ultrassom SOLO FINALIZADO
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "pwm_z42.h"

#define TPM_IRQ_LINE TPM1_IRQn
#define TPM_IRQ_PRIORITY 1
#define TPM_INPUT_CAPTURE_BOTH (TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK)
#define TPM_CHANNEL_INTERRUPT (TPM_CnSC_CHIE_MASK)
// Definição dos pinos
#define ECHO_PIN 12   // PTA12
#define TRIG_PIN 31    // PTE31

// Variáveis para cálculo
volatile uint32_t tempo_subida = 0;
volatile uint32_t largura_pulso = 0;

// Device de GPIO para o Trigger
const struct device *gpioa_dev = DEVICE_DT_GET(DT_NODELABEL(gpioa));
const struct device *gpioe_dev = DEVICE_DT_GET(DT_NODELABEL(gpioe));

void tpm1_isr(void *arg)
{
    uint32_t valor_atual = TPM1->CONTROLS[0].CnV;

    // Se o pino PTE20 estiver em 1, é borda de subida
    if (GPIOA->PDIR & (1 << ECHO_PIN)) { 
        tempo_subida = valor_atual;
    } else {
        // Se estiver em 0, é borda de descida (calcula a diferença)
        if (valor_atual >= tempo_subida) {
            largura_pulso = valor_atual - tempo_subida;
        } else {
            largura_pulso = (65535 - tempo_subida) + valor_atual;
        }
    }
    TPM1->STATUS |= TPM_STATUS_CH0F_MASK; 
}

void main(void)
{
    gpio_pin_configure(gpioe_dev, TRIG_PIN, GPIO_OUTPUT_INACTIVE);

    // 2. Configurar Interrupção e Timer
    IRQ_CONNECT(TPM_IRQ_LINE, TPM_IRQ_PRIORITY, tpm1_isr, NULL, 0);
    irq_enable(TPM_IRQ_LINE);
    
    // Inicializa TPM1 com OSCERCLK (8MHz) e Prescaler 128
    pwm_tpm_Init(TPM1, TPM_OSCERCLK, 65535, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(TPM1, 0, TPM_INPUT_CAPTURE_BOTH | TPM_CHANNEL_INTERRUPT, GPIOA, ECHO_PIN);

    while (1)
    {
        // 3. Disparar o Trigger (pulso de 10us)
        gpio_pin_set(gpioe_dev, TRIG_PIN, 1);
        k_busy_wait(10); 
        gpio_pin_set(gpioe_dev, TRIG_PIN, 0);

        // 4. Calcular distância
        // Cada tick = 16us (8MHz / 128 = 62.5kHz -> 1/62.5k = 16us)
        // Distância = (Tempo_total_us * Velocidade_som_cm_us) / 2
        float tempo_total_us = largura_pulso * 16;
        float dist_cm = (tempo_total_us * 0.0343) / 2;
        if (dist_cm <= 5 && dist_cm >=1){
        printk("até 5 cm\n");
        }
        else if (dist_cm <= 10 && dist_cm>5){
             printk("Até 10 cm\n");
        }
        else if (dist_cm <= 15 && dist_cm > 10){
            printk ("Até 15 cm\n");
        }
        else if (dist_cm <= 20 && dist_cm > 15){
            printk ("Até 20 cm\n");
        }
        else {
            printk ("Mais de 20 cm\n");
        }

        //if (dist_cm < 22) {
        //    printk("Carrinho parado\n");
        //} else {
        //    printk("Mais de 20cm.\n");
        //}

        k_msleep(300); // Espera um pouco antes da próxima leitura
    }
}