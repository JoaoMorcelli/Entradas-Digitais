#include "ultrassom.h"
#include <zephyr/drivers/gpio.h>
#include "pwm_z42.h"

#define TPM_IRQ_LINE TPM1_IRQn
#define TPM_IRQ_PRIORITY 1


static volatile uint32_t tempo_subida = 0;
static volatile uint32_t largura_pulso = 0;

static const struct device *gpioa_dev = DEVICE_DT_GET(DT_NODELABEL(gpioa));
static const struct device *gpioe_dev = DEVICE_DT_GET(DT_NODELABEL(gpioe));

void tpm1_isr(void *arg)
{
    uint32_t valor_atual = TPM1->CONTROLS[0].CnV;

    if (GPIOA->PDIR & (1 << ECHO_PIN)) { 
        tempo_subida = valor_atual;
    } else {
        if (valor_atual >= tempo_subida) {
            largura_pulso = valor_atual - tempo_subida;
        } else {
            largura_pulso = (65535 - tempo_subida) + valor_atual;
        }
    }
    TPM1->STATUS |= TPM_STATUS_CH0F_MASK; 
}


void ultrassom_init(void)
{
    gpio_pin_configure(gpioe_dev, TRIG_PIN, GPIO_OUTPUT_INACTIVE);

    IRQ_CONNECT(TPM_IRQ_LINE, TPM_IRQ_PRIORITY, tpm1_isr, NULL, 0);
    irq_enable(TPM_IRQ_LINE);
    
    pwm_tpm_Init(TPM1, TPM_OSCERCLK, 65535, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(TPM1, 0, TPM_INPUT_CAPTURE_BOTH | TPM_CHANNEL_INTERRUPT, GPIOA, ECHO_PIN);
}


void ultrassom_trigger(void)
{
    gpio_pin_set(gpioe_dev, TRIG_PIN, 1);
    k_busy_wait(10); 
    gpio_pin_set(gpioe_dev, TRIG_PIN, 0);
}


float ultrassom_get_distancia(void)
{
    float tempo_total_us = largura_pulso * 16.0f;
    return (tempo_total_us * 0.0343f) / 2.0f;
}