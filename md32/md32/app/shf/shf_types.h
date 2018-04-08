#ifndef SHF_TYPES_H_
#define SHF_TYPES_H_

#include <sys/types.h>
#include <platform/mt_typedefs.h>

#ifndef bool_t
typedef unsigned char bool_t;
#endif

#ifndef float_t
typedef float float_t;
#endif

/*
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef status_t
typedef int status_t;
#endif

#ifndef NO_ERROR
#define NO_ERROR (0x00)
#endif

#ifndef
#define ERROR -1
#endif
*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

//TODO: should enable it if align 4 bytes for uint64. otherwise, comment it.
#define ALIGN_UINT64
#ifndef ALIGN_UINT64
typedef struct {
    uint32_t time_l;
    uint32_t time_h;
} timestamp_t;
#else
typedef uint64_t timestamp_t;
#endif

bool_t contain(const uint8_t* const list, uint8_t size, uint8_t find);
bool_t unique_add(uint8_t* const list, uint8_t max, uint8_t* size, uint8_t find);

uint64_t timestamp_get(timestamp_t* timestamp);
void timestamp_set(timestamp_t* timestamp, uint64_t value);
void timestamp_set_now(timestamp_t* timestamp);
uint64_t timestamp_get_now();

uint16_t convert_uint8_2_uint16(uint8_t* buf);
uint64_t convert_uint8_2_uint64(uint8_t* buf);

size_t save_uint16_2_uint8(uint16_t v, uint8_t* buf);
size_t save_uint32_2_uint8(uint32_t v, uint8_t* buf);
size_t save_uint64_2_uint8(uint64_t v, uint8_t* buf);
size_t save_float_2_uint8(float v, uint8_t* buf);

#ifdef SHF_UNIT_TEST_ENABLE
void shf_types_unit_test();
#endif

#endif /* SHF_TYPES_H_ */
