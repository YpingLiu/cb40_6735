#ifndef SHF_UNIT_TEST_H_
#define SHF_UNIT_TEST_H_

#ifdef SHF_UNIT_TEST_ENABLE

#include "mock_sensor.h"
#include "shf_define.h"

//Note: we use test functions to test real functions, so we should keep test functions as simple as we can.
//So, we should pay less effort for debug test functions.
int unit_SCP_Sensor_Manager_open(uint sensor_type);
int unit_SCP_Sensor_Manager_control(uint sensor_type, uint cmd, void* arg);
int unit_SCP_Sensor_Manager_close(uint sensor_type);

void unit_SCP_Sensor_Manager_set_return(uint sensor_type, int value);
bool_t unit_SCP_Sensor_Manager_isopen(uint sensor_type);
uint unit_SCP_Sensor_Manager_control_cmd(uint sensor_type);
bool_t unit_SCP_Sensor_Manager_control_active(uint sensor_type);
void unit_SCP_Sensor_Manager_clear();

void unit_assert(char* tag, uint32_t cmp1, uint32_t cmp2);
void unit_assert_u64(char* tag, uint64_t cmp1, uint64_t cmp2);

#endif

#endif /* SHF_UNIT_TEST_H_ */
