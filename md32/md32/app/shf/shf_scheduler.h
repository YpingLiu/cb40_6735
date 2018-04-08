#ifndef SHF_SCHEDULER_H_
#define SHF_SCHEDULER_H_

#include "shf_define.h"

#define INTERVAL_MS    (20)//20ms

void shf_scheduler_notify();
void shf_scheduler_set_state(uint8_t state);
void shf_scheduler_run();
void shf_scheduler_init();

#endif /* SHF_SCHEDULER_H_ */
