#ifndef SENSOR_IO_H
#define SENSOR_IO_H

#include "i2c.h"

typedef I2C_Status (*SensorIO_ReadCallback)(uint8_t device_address,
                                            uint8_t register_address,
                                            uint8_t *data, uint8_t length);
typedef I2C_Status (*SensorIO_WriteCallback)(uint8_t device_address,
                                             uint8_t register_address,
                                             const uint8_t *data, uint8_t length);

typedef struct
{
  SensorIO_ReadCallback read;
  SensorIO_WriteCallback write;
} SensorIO;

I2C_Status SensorIO_Register(SensorIO *io, SensorIO_ReadCallback read,
                             SensorIO_WriteCallback write);
I2C_Status SensorIO_Read(const SensorIO *io, uint8_t device_address,
                         uint8_t register_address, uint8_t *data, uint8_t length);
I2C_Status SensorIO_Write(const SensorIO *io, uint8_t device_address,
                          uint8_t register_address, const uint8_t *data,
                          uint8_t length);

#endif
