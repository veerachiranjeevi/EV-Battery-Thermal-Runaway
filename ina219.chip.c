// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 Veera Chiranjeevi

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint32_t voltage_attr;
  uint32_t current_attr;
  uint8_t register_pointer;
  uint8_t byte_index;
} chip_state_t;

bool on_i2c_connect(void *user_data, uint32_t address, bool read) {
  return address == 0x40;
}

bool on_i2c_write(void *user_data, uint8_t data) {
  chip_state_t *chip = (chip_state_t *)user_data;

  chip->register_pointer = data;
  chip->byte_index = 0;

  return true;
}

uint8_t on_i2c_read(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;

  float voltage = attr_read_float(chip->voltage_attr);
  float current = attr_read_float(chip->current_attr);

  uint16_t value = 0;

  if (chip->register_pointer == 0x01) {
    value = (uint16_t)(voltage * 1000);
  } else if (chip->register_pointer == 0x02) {
    value = (uint16_t)(current * 10);
  } else if (chip->register_pointer == 0x03) {
    value = (uint16_t)(voltage * current * 10);
  }

  uint8_t result;

  if (chip->byte_index == 0) {
    result = (value >> 8) & 0xFF;
    chip->byte_index = 1;
  } else {
    result = value & 0xFF;
    chip->byte_index = 0;
    chip->register_pointer++;
  }

  return result;
}

void on_i2c_disconnect(void *user_data) {
}

void chip_init() {
  printf("INA219 simulation started\n");
  
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->voltage_attr = attr_init_float("voltage", 3.7);
  chip->current_attr = attr_init_float("current", 500.0);

  chip->register_pointer = 0;
  chip->byte_index = 0;

  const i2c_config_t i2c_config = {
    .address = 0x40,
    .sda = pin_init("SDA", INPUT_PULLUP),
    .scl = pin_init("SCL", INPUT_PULLUP),
    .connect = on_i2c_connect,
    .read = on_i2c_read,
    .write = on_i2c_write,
    .disconnect = on_i2c_disconnect,
    .user_data = chip
  };

  i2c_init(&i2c_config);

  
}
