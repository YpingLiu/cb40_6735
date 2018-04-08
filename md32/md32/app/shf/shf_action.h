#ifndef SHF_ACTION_H_
#define SHF_ACTION_H_

#include "shf_define.h"

typedef struct {
    shf_condition_index_t cid;
    shf_action_index_t* aid;
    shf_condition_t* condition;
} shf_action_data_t;

typedef status_t (*action_handler)(shf_action_data_t* data);

shf_action_id_t shf_action_add(action_handler handler);
bool_t shf_action_valid(shf_action_id_t action);
//should be called before run every condition
void shf_action_clear_sent(shf_condition_t* condition);
void shf_action_run(shf_action_id_t action, shf_action_data_t* data);

/******************************************************************************
 * Unit Test Function
******************************************************************************/
#ifdef SHF_UNIT_TEST_ENABLE
void shf_action_unit_test();
#endif

#endif /* SHF_ACTION_H_ */
