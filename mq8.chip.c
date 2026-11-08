
#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pin_t ao;
  uint32_t hydrogen_attr;
} chip_state_t;

void update_sensor(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;

  uint32_t hydrogen = attr_read(chip->hydrogen_attr);

  float voltage = (hydrogen / 1000.0f) * 3.3f;

  pin_dac_write(chip->ao, voltage);
}

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->ao = pin_init("AO", ANALOG);

  chip->hydrogen_attr = attr_init("hydrogenLevel", 0);

  const timer_config_t timer_config = {
    .callback = update_sensor,
    .user_data = chip
  };

  timer_t timer = timer_init(&timer_config);

  timer_start(timer, 10000, true);

  update_sensor(chip);

  printf("MQ-8 Hydrogen Sensor Simulation Started\n");
}
