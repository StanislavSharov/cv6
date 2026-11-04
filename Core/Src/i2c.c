#include "i2c.h"

#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_i2c.h"

#define I2C_TIMEOUT_MS 20U

static I2C_Status wait_for_flag(uint32_t flag)
{
  uint32_t start = HAL_GetTick();

  while (1)
  {
    if (LL_I2C_IsActiveFlag_NACK(I2C1))
    {
      LL_I2C_ClearFlag_NACK(I2C1);
      return I2C_NACK;
    }
    if (LL_I2C_IsActiveFlag_BERR(I2C1) || LL_I2C_IsActiveFlag_ARLO(I2C1) ||
        LL_I2C_IsActiveFlag_OVR(I2C1))
    {
      LL_I2C_ClearFlag_BERR(I2C1);
      LL_I2C_ClearFlag_ARLO(I2C1);
      LL_I2C_ClearFlag_OVR(I2C1);
      return I2C_BUS_ERROR;
    }
    if ((I2C1->ISR & flag) != 0U)
    {
      return I2C_OK;
    }
    if ((HAL_GetTick() - start) >= I2C_TIMEOUT_MS)
    {
      return I2C_TIMEOUT;
    }
  }
}

I2C_Status I2C_WriteRegister(uint8_t device_address, uint8_t register_address,
                             uint8_t value)
{
  I2C_Status status;

  if (device_address > 0x7FU)
  {
    return I2C_INVALID_ARGUMENT;
  }
  if (LL_I2C_IsActiveFlag_BUSY(I2C1))
  {
    return I2C_BUSY;
  }

  LL_I2C_ClearFlag_STOP(I2C1);
  LL_I2C_ClearFlag_NACK(I2C1);
  LL_I2C_HandleTransfer(I2C1, (uint32_t)device_address << 1U,
                        LL_I2C_ADDRSLAVE_7BIT, 2U, LL_I2C_MODE_AUTOEND,
                        LL_I2C_GENERATE_START_WRITE);

  status = wait_for_flag(I2C_ISR_TXIS);
  if (status == I2C_OK)
  {
    LL_I2C_TransmitData8(I2C1, register_address);
    status = wait_for_flag(I2C_ISR_TXIS);
  }
  if (status == I2C_OK)
  {
    LL_I2C_TransmitData8(I2C1, value);
    status = wait_for_flag(I2C_ISR_STOPF);
  }

  if (status != I2C_OK && !LL_I2C_IsActiveFlag_STOP(I2C1))
  {
    LL_I2C_GenerateStopCondition(I2C1);
  }
  if (LL_I2C_IsActiveFlag_STOP(I2C1))
  {
    LL_I2C_ClearFlag_STOP(I2C1);
  }
  return status;
}

I2C_Status I2C_ReadRegister(uint8_t device_address, uint8_t register_address,
                            uint8_t *value)
{
  I2C_Status status;

  if (device_address > 0x7FU || value == NULL)
  {
    return I2C_INVALID_ARGUMENT;
  }
  if (LL_I2C_IsActiveFlag_BUSY(I2C1))
  {
    return I2C_BUSY;
  }

  LL_I2C_ClearFlag_STOP(I2C1);
  LL_I2C_ClearFlag_NACK(I2C1);
  LL_I2C_HandleTransfer(I2C1, (uint32_t)device_address << 1U,
                        LL_I2C_ADDRSLAVE_7BIT, 1U, LL_I2C_MODE_SOFTEND,
                        LL_I2C_GENERATE_START_WRITE);

  status = wait_for_flag(I2C_ISR_TXIS);
  if (status == I2C_OK)
  {
    LL_I2C_TransmitData8(I2C1, register_address);
    status = wait_for_flag(I2C_ISR_TC);
  }
  if (status == I2C_OK)
  {
    LL_I2C_HandleTransfer(I2C1, (uint32_t)device_address << 1U,
                          LL_I2C_ADDRSLAVE_7BIT, 1U, LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);
    status = wait_for_flag(I2C_ISR_RXNE);
  }
  if (status == I2C_OK)
  {
    *value = LL_I2C_ReceiveData8(I2C1);
    status = wait_for_flag(I2C_ISR_STOPF);
  }

  if (status != I2C_OK && !LL_I2C_IsActiveFlag_STOP(I2C1))
  {
    LL_I2C_GenerateStopCondition(I2C1);
  }
  if (LL_I2C_IsActiveFlag_STOP(I2C1))
  {
    LL_I2C_ClearFlag_STOP(I2C1);
  }
  return status;
}
