#ifndef SHF_DATA_POOL_H_
#define SHF_DATA_POOL_H_

#include "shf_define.h"

uint32_t* shf_data_pool_get_uint32(shf_data_index_t index);
uint64_t* shf_data_pool_get_uint64(shf_data_index_t index);
uint32_t* shf_data_pool_get_last_uint32(shf_data_index_t index);
uint64_t* shf_data_pool_get_last_uint64(shf_data_index_t index);
void shf_data_pool_set_uint32(shf_data_index_t index, uint32_t value);
void shf_data_pool_set_uint64(shf_data_index_t index, uint64_t value);
shf_data_type_t shf_data_pool_get_type(shf_data_index_t index);
bool_t shf_data_pool_is_changed(shf_data_index_t index);
void shf_data_pool_clear(shf_data_index_t index);
void shf_data_pool_run();
void shf_data_pool_set_gesture(shf_gesture_t gid);
shf_gesture_t shf_data_pool_get_gesture();

#ifdef SHF_DEBUG_MODE
void shf_data_pool_dump();
#endif

/******************************************************************************
 * Unit Test Function
******************************************************************************/
#ifdef SHF_UNIT_TEST_ENABLE
void shf_data_pool_unit_test();
#endif

#ifdef SHF_INTEGRATION_TEST_ENABLE
void shf_data_pool_it();
#endif

#endif /* SHF_DATA_POOL_H_ */
