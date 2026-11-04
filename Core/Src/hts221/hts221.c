#include "hts221/hts221.h"

#include <stddef.h>

HTS221_InitStatus HTS221_Init(HTS221_Device *device, const SensorIO *io)
{
  uint8_t control_value = HTS221_CTRL_REG1_1HZ_BDU;
  uint8_t readback = 0U;

  if (device == NULL || io == NULL || io->read == NULL || io->write == NULL)
  {
    return HTS221_INIT_INVALID_ARGUMENT;
  }

  device->io = io;
  device->detected_id = 0U;
  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_WHO_AM_I_REG,
                                          &device->detected_id, 1U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_INIT_IO_ERROR;
  }
  if (device->detected_id != HTS221_WHO_AM_I_ID)
  {
    return HTS221_INIT_WRONG_DEVICE;
  }

  /* PD=1, BDU=1, ODR=01: powered on, coherent samples at 1 Hz. */
  device->last_io_status = SensorIO_Write(io, HTS221_I2C_ADDRESS,
                                           HTS221_CTRL_REG1,
                                           &control_value, 1U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_INIT_IO_ERROR;
  }

  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_CTRL_REG1,
                                          &readback, 1U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_INIT_IO_ERROR;
  }
  if ((readback & HTS221_CTRL_REG1_1HZ_BDU) != control_value)
  {
    return HTS221_INIT_CONFIG_MISMATCH;
  }

  return HTS221_INIT_OK;
}
