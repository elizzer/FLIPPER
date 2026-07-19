#include <stdio.h>
#include "interface_registry.h"
#include <string.h>

InterfaceRegistryEntry_t interface_registry[INTERFACE_REGISTRY_SIZE];

static void reset_registry_entry(int index);
static int8_t find_empty_slot(void);

uint8_t interface_registry_init(void){
    //init all the values of the registry to 0
    for(int i = 0; i < INTERFACE_REGISTRY_SIZE; i++){
       reset_registry_entry(i);
    }
    return 0;
}
//function to register an interface
uint8_t interface_registry_register(InterfaceRegistryEntry_t *entry){
    int8_t slot = find_empty_slot();
    if(slot == -1){
        return 1; // no empty slot found
    }

    //before entry check for duplicate id or name

    for(int i = 0; i < INTERFACE_REGISTRY_SIZE; i++){
        if(interface_registry[i].id == entry->id || strcmp(interface_registry[i].name, entry->name) == 0){
            return 2; // duplicate id or name found
        }
    }

    //copy the entry into the registry
    memcpy(&interface_registry[slot], entry, sizeof(InterfaceRegistryEntry_t));

    return 0;
}
//function to unregister an interface
uint8_t interface_registry_unregister(uint8_t id){
    //serach for the id and reset it if found, if not found return 1
    for(int i = 0; i < INTERFACE_REGISTRY_SIZE; i++){
        if(interface_registry[i].id == id){
            reset_registry_entry(i);
            return 0;
        }
    }
    return 1;
}
//function to look up an interface by name
InterfaceRegistryEntry_t *interface_registry_lookup_name(const char *name){
    for(int i = 0; i < INTERFACE_REGISTRY_SIZE; i++){
        if(strcmp(interface_registry[i].name, name) == 0){
            return &interface_registry[i];
        }
    }
    return NULL;
}
//function to look up an interface by id
InterfaceRegistryEntry_t *interface_registry_lookup_id(uint8_t id){
    for(int i = 0; i < INTERFACE_REGISTRY_SIZE; i++){
        if(interface_registry[i].id == id){
            return &interface_registry[i];
        }
    }
    return NULL;
}

// function to search for empty slot in the registry
static int8_t find_empty_slot(void){
    for(int i = 0; i < INTERFACE_REGISTRY_SIZE; i++){
        if(interface_registry[i].id == 0){
            return i;
        }
    }
    return -1; // no empty slot found
}

//static function to reset the registry entry at a given index
static void reset_registry_entry(int index){
    if(index < 0 || index >= INTERFACE_REGISTRY_SIZE){
        return;
    }
    interface_registry[index].id = 0;
    interface_registry[index].name[0] = '\0';
    interface_registry[index].init = NULL;
    interface_registry[index].de_init = NULL;
    interface_registry[index].cmd_handler = NULL;
    interface_registry[index].help_handler = NULL;
}