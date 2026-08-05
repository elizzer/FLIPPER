#ifndef KX_PWM_HAL_H
#define KX_PWM_HAL_H

#include <stdint.h>
#include "kx_hal_types.h"


Kx_ErrorCode KxPWM_Init();
Kx_ErrorCode KxPWM_DeInit();
Kx_ErrorCode KxPWM_SetFrequency(Kx_IO pin,uint32_t freq_hz);
Kx_ErrorCode KxPWM_GetFrequency(Kx_IO pin,uint32_t freq_hz);
Kx_ErrorCode KxPWM_SetDutyCycle(Kx_IO pin,uint8_t duty_cycle);
Kx_ErrorCode KxPWM_GetDutyCycle(Kx_IO pin,uint8_t duty_cycle);
Kx_ErrorCode KxPWM_Start(Kx_IO pin);
Kx_ErrorCode KxPWM_Stop(Kx_IO pin);
Kx_ErrorCode KxPWM_SetPin(Kx_IO pin);

#endif // KX_PWM_HAL_H