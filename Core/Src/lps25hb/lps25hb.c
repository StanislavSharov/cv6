#include "lps25hb/lps25hb.h"
#include <stddef.h>
#include <math.h>

LPS25HB_Status LPS25HB_Init(LPS25HB_Device *device, const SensorIO *io)
{
  uint8_t config = LPS25HB_CTRL_1HZ_BDU, readback = 0U;
  if (device == NULL || io == NULL || io->read == NULL || io->write == NULL)
    return LPS25HB_INVALID;
  device->io = io;
  device->initialized = 0U;
  device->detected_id = 0U;
  device->last_io_status = SensorIO_Read(io, LPS25HB_ADDRESS,
                                          LPS25HB_WHO_AM_I, &device->detected_id, 1U);
  if (device->last_io_status != I2C_OK) return LPS25HB_IO_ERROR;
  if (device->detected_id != LPS25HB_ID) return LPS25HB_WRONG_DEVICE;
  device->last_io_status = SensorIO_Write(io, LPS25HB_ADDRESS,
                                           LPS25HB_CTRL_REG1, &config, 1U);
  if (device->last_io_status != I2C_OK) return LPS25HB_IO_ERROR;
  device->last_io_status = SensorIO_Read(io, LPS25HB_ADDRESS,
                                          LPS25HB_CTRL_REG1, &readback, 1U);
  if (device->last_io_status != I2C_OK) return LPS25HB_IO_ERROR;
  if ((readback & config) != config) return LPS25HB_CONFIG_MISMATCH;
  device->initialized = 1U;
  return LPS25HB_OK;
}

LPS25HB_Status LPS25HB_ReadPressure(LPS25HB_Device *device, float *hpa)
{
  uint8_t status = 0U, data[3] = {0U, 0U, 0U};
  int32_t raw;
  if (device == NULL || hpa == NULL || !device->initialized) return LPS25HB_INVALID;
  device->last_io_status = SensorIO_Read(device->io, LPS25HB_ADDRESS,
                                          LPS25HB_STATUS_REG, &status, 1U);
  if (device->last_io_status != I2C_OK) return LPS25HB_IO_ERROR;
  if ((status & 0x02U) == 0U) return LPS25HB_NOT_READY;
  device->last_io_status = SensorIO_Read(device->io, LPS25HB_ADDRESS,
                                          LPS25HB_PRESS_OUT_XL | LPS25HB_AUTO_INCREMENT,
                                          data, 3U);
  if (device->last_io_status != I2C_OK) return LPS25HB_IO_ERROR;
  raw = (int32_t)((uint32_t)data[0] | ((uint32_t)data[1] << 8U) |
                  ((uint32_t)data[2] << 16U));
  if ((raw & 0x800000L) != 0L) raw |= (int32_t)0xFF000000UL;
  *hpa = (float)raw / 4096.0f;
  return LPS25HB_OK;
}

float LPS25HB_RelativeAltitude(float pressure_hpa, float reference_hpa)
{
  if (pressure_hpa <= 0.0f || reference_hpa <= 0.0f) return 0.0f;
  return 44330.0f * (1.0f - powf(pressure_hpa / reference_hpa, 0.19029496f));
}
