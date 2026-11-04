#ifndef HTS221_H
#define HTS221_H

#include "sensor_io.h"

#define HTS221_I2C_ADDRESS   0x5FU
#define HTS221_WHO_AM_I_REG  0x0FU
#define HTS221_WHO_AM_I_ID   0xBCU
#define HTS221_CTRL_REG1     0x20U
#define HTS221_CTRL_REG1_1HZ_BDU 0x85U

typedef enum
{
  HTS221_INIT_OK = 0,
  HTS221_INIT_INVALID_ARGUMENT,
  HTS221_INIT_IO_ERROR,
  HTS221_INIT_WRONG_DEVICE,
  HTS221_INIT_CONFIG_MISMATCH
} HTS221_InitStatus;

typedef struct
{
  const SensorIO *io;
  uint8_t detected_id;
  I2C_Status last_io_status;
} HTS221_Device;

HTS221_InitStatus HTS221_Init(HTS221_Device *device, const SensorIO *io);

#endif
