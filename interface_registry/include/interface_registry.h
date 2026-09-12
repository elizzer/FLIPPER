
#ifndef INTERFACE_REGISTRY_H
#define INTERFACE_REGISTRY_H

#include <stdint.h>

typedef enum {
    INTERFACE_NONE,
    INTERFACE_GPIO,
    INTERFACE_I2C,
    INTERFACE_SPI,
    INTERFACE_UART,
    INTERFACE_PWM,
} InterfaceId_e;

typedef int8_t (*InterfaceHandlerInitCB_t)(void **);
typedef int8_t (*InterfaceHandlerDeInitCB_t)(void *);
typedef int8_t (*InterfaceHandlerCmdCB_t)(void *, const char *);
typedef void (*InterfaceHandlerHelpCB_t)();

//define the structure of the registry entry
typedef struct{

    uint8_t id;
    char name[32];
    InterfaceHandlerInitCB_t init;
    InterfaceHandlerDeInitCB_t de_init;
    InterfaceHandlerCmdCB_t cmd_handler;
    InterfaceHandlerHelpCB_t help_handler;

}InterfaceRegistryEntry_t;
//define the size of the registry entry buffer
#define INTERFACE_REGISTRY_SIZE 10

//function to initialize the registry
uint8_t interface_registry_init(void);
//function to register an interface
uint8_t interface_registry_register(InterfaceRegistryEntry_t *entry);
//function to unregister an interface
uint8_t interface_registry_unregister(uint8_t id);
//function to look up an interface by name
InterfaceRegistryEntry_t *interface_registry_lookup_name(const char *name);
//function to look up an interface by id
InterfaceRegistryEntry_t *interface_registry_lookup_id(uint8_t id);

#endif // INTERFACE_REGISTRY_H