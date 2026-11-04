#ifndef APP_I2C_H
#define APP_I2C_H

#include <stdint.h>

typedef enum
{
  I2C_OK = 0,
  I2C_INVALID_ARGUMENT,
  I2C_BUSY,
  I2C_NACK,
  I2C_TIMEOUT,
  I2C_BUS_ERROR
} I2C_Status;

/* device_address is the unshifted 7-bit I2C address. */
I2C_Status I2C_WriteRegister(uint8_t device_address, uint8_t register_address,
                             uint8_t value);
/* Reads one byte after sending the register address with a repeated START. */
I2C_Status I2C_ReadRegister(uint8_t device_address, uint8_t register_address,
                            uint8_t *value);

#endif
