#include "sensor_io.h"

#include <stddef.h>

I2C_Status SensorIO_Register(SensorIO *io, SensorIO_ReadCallback read,
                             SensorIO_WriteCallback write)
{
  if (io == NULL || read == NULL || write == NULL)
  {
    return I2C_INVALID_ARGUMENT;
  }

  io->read = read;
  io->write = write;
  return I2C_OK;
}

I2C_Status SensorIO_Read(const SensorIO *io, uint8_t device_address,
                         uint8_t register_address, uint8_t *data, uint8_t length)
{
  if (io == NULL || io->read == NULL)
  {
    return I2C_INVALID_ARGUMENT;
  }
  return io->read(device_address, register_address, data, length);
}

I2C_Status SensorIO_Write(const SensorIO *io, uint8_t device_address,
                          uint8_t register_address, const uint8_t *data,
                          uint8_t length)
{
  if (io == NULL || io->write == NULL)
  {
    return I2C_INVALID_ARGUMENT;
  }
  return io->write(device_address, register_address, data, length);
}
