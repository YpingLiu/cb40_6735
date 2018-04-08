#ifndef SHF_POLLER_H_
#define SHF_POLLER_H_

#include "shf_define.h"

void shf_poller_run();

/******************************************************************************
 * Unit Test Function
******************************************************************************/
#ifdef SHF_UNIT_TEST_ENABLE
void shf_poller_unit_test();
#endif

#endif /* SHF_POLLER_H_ */
