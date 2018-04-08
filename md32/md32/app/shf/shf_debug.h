#ifndef SHF_DEBUG_H_
#define SHF_DEBUG_H_

#include <debug.h>
#include <printf.h>

#include "shf_define.h"

#define LOGLEVEL 1

#if ((LOGLEVEL) >= 0)
#define logw(fmt, args...)    dprintf(ALWAYS, fmt, ##args)
#else
#define logw(fmt, args...)
#endif

#if ((LOGLEVEL) >= 1) 
#define logd(fmt, args...)    dprintf(ALWAYS, fmt, ##args)
#else
#define logd(fmt, args...)
#endif

#if ((LOGLEVEL) >= 2)
#define logv(fmt, args...)    dprintf(ALWAYS, fmt, ##args)
#else
#define logv(fmt, args...)
#endif

#ifdef SHF_DEBUG_MODE
void shf_debug_print_bytes(void* buffer, size_t size);
void shf_debug_print_sensor(void* buffer, size_t count);
void assert(char* tag, uint8_t cmp1, uint8_t cmp2);
#endif

#endif /* SHF_DEBUG_H_ */
