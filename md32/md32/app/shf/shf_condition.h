#ifndef SHF_CONDITION_H_
#define SHF_CONDITION_H_

#include "shf_define.h"

shf_action_index_t shf_condition_action_add(shf_condition_index_t condition_index,
        shf_action_id_t action);
status_t shf_condition_action_remove(shf_condition_index_t condition_index,
        shf_action_index_t action_index);
//allocate a new condition and add action to slot 0
shf_condition_index_t shf_condition_add(const shf_condition_t* const condition);
status_t shf_condition_update(shf_condition_index_t condition_index,
        const shf_condition_t* const condition);
bool_t shf_condition_check(shf_condition_t* condition, shf_condition_index_t condition_index);
bool_t shf_condition_unique_data_index(shf_condition_t* condition, shf_data_index_t* list,
        shf_data_index_t* size);
bool_t shf_condition_index_valid(shf_condition_index_t condition_index);
bool_t shf_action_index_valid(shf_action_index_t action_index);

void shf_condition_run();
void shf_condition_init();
void shf_condition_config();

#ifdef SHF_DEBUG_MODE
void shf_condition_dump_one(shf_condition_t* condition, shf_condition_index_t condition_index);
void shf_condition_dump();
#endif

/******************************************************************************
 * Unit Test Function
******************************************************************************/
#ifdef SHF_UNIT_TEST_ENABLE
void shf_condition_unit_test();
#endif

#endif /* SHF_CONDITION_H_ */
