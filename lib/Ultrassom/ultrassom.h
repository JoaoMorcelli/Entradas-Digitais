#ifndef ULTRASSOM_H
#define ULTRASSOM_H

#include <zephyr/kernel.h>

#define ECHO_PIN 12    // PTA12
#define TRIG_PIN 31    // PTE31

void ultrassom_init(void);
void ultrassom_trigger(void);
float ultrassom_get_distancia(void);

#endif