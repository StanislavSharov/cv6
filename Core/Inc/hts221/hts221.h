#ifndef HTS221_H
#define HTS221_H

#include "sensor_io.h"

#define HTS221_I2C_ADDRESS   0x5FU
#define HTS221_WHO_AM_I_REG  0x0FU
#define HTS221_WHO_AM_I_ID   0xBCU
#define HTS221_CTRL_REG1     0x20U
#define HTS221_CTRL_REG1_1HZ_BDU 0x85U
#define HTS221_STATUS_REG    0x27U
#define HTS221_HUMIDITY_OUT_L 0x28U
#define HTS221_H0_RH_X2 0x30U
#define HTS221_H0_T0_OUT_L 0x36U
#define HTS221_H1_T0_OUT_L 0x3AU
#define HTS221_TEMP_OUT_L    0x2AU
#define HTS221_T0_DEGC_X8   0x32U
#define HTS221_T0_T1_MSB    0x35U
#define HTS221_T0_OUT_L     0x3CU
#define HTS221_AUTO_INCREMENT 0x80U

typedef enum
{
  HTS221_INIT_OK = 0,
  HTS221_INIT_INVALID_ARGUMENT,
  HTS221_INIT_IO_ERROR,
  HTS221_INIT_WRONG_DEVICE,
  HTS221_INIT_CONFIG_MISMATCH,
  HTS221_INIT_BAD_CALIBRATION
} HTS221_InitStatus;

typedef enum
{
  HTS221_MEASURE_OK = 0,
  HTS221_MEASURE_INVALID_ARGUMENT,
  HTS221_MEASURE_NOT_INITIALIZED,
  HTS221_MEASURE_IO_ERROR,
  HTS221_MEASURE_NOT_READY
} HTS221_MeasureStatus;

typedef struct
{
  const SensorIO *io;
  uint8_t detected_id;
  I2C_Status last_io_status;
  float hum_h0_rh;
  float hum_h1_rh;
  int16_t hum_h0_out;
  int16_t hum_h1_out;
  float temp_t0_c;
  float temp_t1_c;
  int16_t temp_t0_out;
  int16_t temp_t1_out;
  uint8_t initialized;
} HTS221_Device;

HTS221_InitStatus HTS221_Init(HTS221_Device *device, const SensorIO *io);
HTS221_MeasureStatus HTS221_ReadTemperature(HTS221_Device *device,
                                              float *temperature_c);

HTS221_MeasureStatus HTS221_ReadHumidity(HTS221_Device *device, float *humidity_rh);

#endif
