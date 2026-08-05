#ifndef KX_HAL_TYPES_H
#define KX_HAL_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef int8_t Kx_IO;

typedef enum Kx_ErrorCode {
    KX_HAL_OK               = 0,
    KX_HAL_ERR_FAIL         = -1,
    KX_HAL_ERR_INVALID_ARG  = -2,
    KX_HAL_ERR_TIMEOUT      = -3,
    KX_HAL_ERR_NOT_INIT     = -4,
    KX_HAL_ERR_BUSY         = -5,
    KX_HAL_ERR_NOT_SUPPORTED = -6,
    KX_HAL_ERR_NO_MEM       = -7,
} Kx_ErrorCode;

#endif // KX_HAL_TYPES_H