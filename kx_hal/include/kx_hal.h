#ifndef KX_HAL_H
#define KX_HAL_H

#include <stdint.h>

/* ---- Base register access ---- */
#define KX_HAL_REG_READ(addr)            (*(volatile uint32_t *)(addr))
#define KX_HAL_REG_WRITE(addr, val)      (*(volatile uint32_t *)(addr) = (uint32_t)(val))

/* ---- Single-bit operations ---- */
#define KX_HAL_REG_SET_BIT(addr, bit)    KX_HAL_REG_WRITE((addr), KX_HAL_REG_READ(addr) |  (1U << (bit)))
#define KX_HAL_REG_CLEAR_BIT(addr, bit)  KX_HAL_REG_WRITE((addr), KX_HAL_REG_READ(addr) & ~(1U << (bit)))
#define KX_HAL_REG_TOGGLE_BIT(addr, bit) KX_HAL_REG_WRITE((addr), KX_HAL_REG_READ(addr) ^  (1U << (bit)))
#define KX_HAL_REG_GET_BIT(addr, bit)    ((KX_HAL_REG_READ(addr) >> (bit)) & 0x1U)

/* ---- Multi-bit mask operations ---- */
#define KX_HAL_REG_SET_BITS(addr, mask)      KX_HAL_REG_WRITE((addr), KX_HAL_REG_READ(addr) |  (mask))
#define KX_HAL_REG_CLEAR_BITS(addr, mask)    KX_HAL_REG_WRITE((addr), KX_HAL_REG_READ(addr) & ~(mask))
#define KX_HAL_REG_GET_BITS(addr, mask)      (KX_HAL_REG_READ(addr) & (mask))

/* ---- Field operations (mask + shift) ---- */
#define KX_HAL_REG_GET_FIELD(addr, mask, shift) \
    ((KX_HAL_REG_READ(addr) & (mask)) >> (shift))

#define KX_HAL_REG_SET_FIELD(addr, mask, shift, val)                        \
    KX_HAL_REG_WRITE((addr),                                                \
        (KX_HAL_REG_READ(addr) & ~(mask)) | (((uint32_t)(val) << (shift)) & (mask)))

#endif //KX_HAL_H