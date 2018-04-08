#ifndef SHF_PROCESS_H_
#define SHF_PROCESS_H_

#include "shf_define.h"

void shf_process_run();
void shf_process_init();

/******************************************************************************
 * Unit Test Function
******************************************************************************/
#ifdef SHF_UNIT_TEST_ENABLE
void shf_process_unit_test();
#endif

#endif /* SHF_PROCESS_H_ */
