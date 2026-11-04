#include "hts221/hts221.h"

#include <stddef.h>

static int16_t signed_le16(const uint8_t *bytes)
{
  return (int16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8U));
}

HTS221_InitStatus HTS221_Init(HTS221_Device *device, const SensorIO *io)
{
  uint8_t control_value = HTS221_CTRL_REG1_1HZ_BDU;
  uint8_t readback = 0U;
  uint8_t hum_points[2] = {0U, 0U};
  uint8_t hum_outputs[2] = {0U, 0U};
  uint8_t temp_points[2] = {0U, 0U};
  uint8_t temp_msb = 0U;
  uint8_t temp_outputs[4] = {0U, 0U, 0U, 0U};
  uint16_t t0_x8;
  uint16_t t1_x8;

  if (device == NULL || io == NULL || io->read == NULL || io->write == NULL)
  {
    return HTS221_INIT_INVALID_ARGUMENT;
  }

  device->io = io;
  device->initialized = 0U;
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

  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_T0_DEGC_X8 | HTS221_AUTO_INCREMENT,
                                          temp_points, 2U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_INIT_IO_ERROR;
  }
  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_T0_T1_MSB, &temp_msb, 1U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_INIT_IO_ERROR;
  }
  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_T0_OUT_L | HTS221_AUTO_INCREMENT,
                                          temp_outputs, 4U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_INIT_IO_ERROR;
  }

  t0_x8 = (uint16_t)temp_points[0] | ((uint16_t)(temp_msb & 0x03U) << 8U);
  t1_x8 = (uint16_t)temp_points[1] | ((uint16_t)(temp_msb & 0x0CU) << 6U);
  device->temp_t0_c = (float)t0_x8 / 8.0f;
  device->temp_t1_c = (float)t1_x8 / 8.0f;
  device->temp_t0_out = signed_le16(&temp_outputs[0]);
  device->temp_t1_out = signed_le16(&temp_outputs[2]);
  if (device->temp_t0_out == device->temp_t1_out)
  {
    return HTS221_INIT_BAD_CALIBRATION;
  }
  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_H0_RH_X2 | HTS221_AUTO_INCREMENT,
                                          hum_points, 2U);
  if (device->last_io_status != I2C_OK) return HTS221_INIT_IO_ERROR;
  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_H0_T0_OUT_L | HTS221_AUTO_INCREMENT,
                                          hum_outputs, 2U);
  if (device->last_io_status != I2C_OK) return HTS221_INIT_IO_ERROR;
  device->hum_h0_out = signed_le16(hum_outputs);
  device->last_io_status = SensorIO_Read(io, HTS221_I2C_ADDRESS,
                                          HTS221_H1_T0_OUT_L | HTS221_AUTO_INCREMENT,
                                          hum_outputs, 2U);
  if (device->last_io_status != I2C_OK) return HTS221_INIT_IO_ERROR;
  device->hum_h1_out = signed_le16(hum_outputs);
  device->hum_h0_rh = (float)hum_points[0] / 2.0f;
  device->hum_h1_rh = (float)hum_points[1] / 2.0f;
  if (device->hum_h0_out == device->hum_h1_out)
    return HTS221_INIT_BAD_CALIBRATION;
  device->initialized = 1U;

  return HTS221_INIT_OK;
}

HTS221_MeasureStatus HTS221_ReadTemperature(HTS221_Device *device,
                                              float *temperature_c)
{
  uint8_t status = 0U;
  uint8_t output[2] = {0U, 0U};
  int16_t raw;

  if (device == NULL || temperature_c == NULL)
  {
    return HTS221_MEASURE_INVALID_ARGUMENT;
  }
  if (device->initialized == 0U)
  {
    return HTS221_MEASURE_NOT_INITIALIZED;
  }
  device->last_io_status = SensorIO_Read(device->io, HTS221_I2C_ADDRESS,
                                          HTS221_STATUS_REG, &status, 1U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_MEASURE_IO_ERROR;
  }
  if ((status & 0x01U) == 0U)
  {
    return HTS221_MEASURE_NOT_READY;
  }
  device->last_io_status = SensorIO_Read(device->io, HTS221_I2C_ADDRESS,
                                          HTS221_TEMP_OUT_L | HTS221_AUTO_INCREMENT,
                                          output, 2U);
  if (device->last_io_status != I2C_OK)
  {
    return HTS221_MEASURE_IO_ERROR;
  }

  raw = signed_le16(output);
  *temperature_c = device->temp_t0_c +
      ((float)raw - (float)device->temp_t0_out) *
      (device->temp_t1_c - device->temp_t0_c) /
      ((float)device->temp_t1_out - (float)device->temp_t0_out);
  return HTS221_MEASURE_OK;
}

HTS221_MeasureStatus HTS221_ReadHumidity(HTS221_Device *device, float *humidity_rh)
{
  uint8_t status = 0U, output[2] = {0U, 0U};
  int16_t raw;
  float value;
  if (device == NULL || humidity_rh == NULL) return HTS221_MEASURE_INVALID_ARGUMENT;
  if (!device->initialized) return HTS221_MEASURE_NOT_INITIALIZED;
  device->last_io_status = SensorIO_Read(device->io, HTS221_I2C_ADDRESS,
                                          HTS221_STATUS_REG, &status, 1U);
  if (device->last_io_status != I2C_OK) return HTS221_MEASURE_IO_ERROR;
  if ((status & 0x02U) == 0U) return HTS221_MEASURE_NOT_READY;
  device->last_io_status = SensorIO_Read(device->io, HTS221_I2C_ADDRESS,
                                          HTS221_HUMIDITY_OUT_L | HTS221_AUTO_INCREMENT,
                                          output, 2U);
  if (device->last_io_status != I2C_OK) return HTS221_MEASURE_IO_ERROR;
  raw = signed_le16(output);
  value = device->hum_h0_rh + ((float)raw - (float)device->hum_h0_out) *
      (device->hum_h1_rh - device->hum_h0_rh) /
      ((float)device->hum_h1_out - (float)device->hum_h0_out);
  if (value < 0.0f) value = 0.0f;
  if (value > 100.0f) value = 100.0f;
  *humidity_rh = value;
  return HTS221_MEASURE_OK;
}
