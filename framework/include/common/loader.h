#ifndef __COMMON_LOADER_H__
#define __COMMON_LOADER_H__

#include <common/module.h>

interface_t* import(module_t *module, const char* layer_id, const char *module_id);
module_t* load(const char* layer_id, const char *module_id);
int unload(module_t *module);

#endif//__COMMON_LOADER_H__
