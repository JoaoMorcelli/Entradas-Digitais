//Dois sensores FINALIZADO PRONTO CERTO VELOCIDADE LENTA
#include <zephyr/kernel.h>            
#include <zephyr/device.h>            
#include <zephyr/drivers/gpio.h>  
#include <pwm_z42.h>

#define INPUT_PORT  DEVICE_DT_NAME(DT_NODELABEL(gpioe))   // Porta E = GPIO_4 no seu .dts
#define INPUT_PIN_E   20         // PTE20
#define INPUT_PIN_D  21         // PTE21
#define TPM_MODULE 1000        
uint16_t duty_1  = TPM_MODULE/1.3;
uint16_t duty_2  = TPM_MODULE/1.3;

int main(void)
{
    const struct device *input_dev;
    int val_e, val_d;
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    //pwm_tpm_Ch_Init(TPM0, 0, TPM_PWM_H, GPIOD, 0);
    pwm_tpm_Ch_Init(TPM0, 4, TPM_PWM_H, GPIOD, 4);
    pwm_tpm_Ch_Init(TPM0, 5, TPM_PWM_H, GPIOD, 5);
    pwm_tpm_Ch_Init(TPM0, 2, TPM_PWM_H, GPIOD, 2);
    pwm_tpm_Ch_Init(TPM0, 3, TPM_PWM_H, GPIOD, 3);

    input_dev = device_get_binding(INPUT_PORT);
 
    gpio_pin_configure(input_dev, INPUT_PIN_E, GPIO_INPUT);
    gpio_pin_configure(input_dev, INPUT_PIN_D, GPIO_INPUT);

    while (1) {
        val_e = gpio_pin_get(input_dev, INPUT_PIN_E);
        val_d = gpio_pin_get(input_dev, INPUT_PIN_D);

        if (val_e==0 && val_d==1){
        pwm_tpm_CnV(TPM0, 2, duty_2);
        pwm_tpm_CnV(TPM0, 4, 0);
        pwm_tpm_CnV(TPM0, 3, 0);
         pwm_tpm_CnV(TPM0, 5, duty_1);
         printk ("direita\n");
        }
        else if (val_d==0 && val_e==1){
        pwm_tpm_CnV(TPM0, 2, 0);
        pwm_tpm_CnV(TPM0, 4, duty_2);
        pwm_tpm_CnV(TPM0, 3, duty_1);
        pwm_tpm_CnV(TPM0, 5, 0);
        printk ("esquerda\n");
        }
        else if (val_e==0 && val_d==0){
        pwm_tpm_CnV(TPM0, 2, duty_1);
        pwm_tpm_CnV(TPM0, 4, duty_1);
        pwm_tpm_CnV(TPM0, 5, 0);
        pwm_tpm_CnV(TPM0, 3, 0);
        printk ("Reto\n");
        }
        else if (val_e==1 && val_d==1){
        pwm_tpm_CnV(TPM0, 2, duty_1);
        pwm_tpm_CnV(TPM0, 4, duty_1);
        pwm_tpm_CnV(TPM0, 3, 0);
        pwm_tpm_CnV(TPM0, 5, 0);
        printk ("reto\n");
        }
        k_msleep(60);
    }
}