#ifndef LPS25HB_H
#define LPS25HB_H
#include "sensor_io.h"
#define LPS25HB_ADDRESS 0x5DU
#define LPS25HB_WHO_AM_I 0x0FU
#define LPS25HB_ID 0xBDU
#define LPS25HB_CTRL_REG1 0x20U
#define LPS25HB_CTRL_1HZ_BDU 0x94U
#define LPS25HB_STATUS_REG 0x27U
#define LPS25HB_PRESS_OUT_XL 0x28U
#define LPS25HB_AUTO_INCREMENT 0x80U
typedef enum { LPS25HB_OK=0, LPS25HB_INVALID, LPS25HB_IO_ERROR,
               LPS25HB_WRONG_DEVICE, LPS25HB_CONFIG_MISMATCH,
               LPS25HB_NOT_READY } LPS25HB_Status;
typedef struct { const SensorIO *io; I2C_Status last_io_status;
                 uint8_t detected_id, initialized; } LPS25HB_Device;
LPS25HB_Status LPS25HB_Init(LPS25HB_Device *device, const SensorIO *io);
LPS25HB_Status LPS25HB_ReadPressure(LPS25HB_Device *device, float *hpa);
float LPS25HB_RelativeAltitude(float pressure_hpa, float reference_hpa);
#endif
