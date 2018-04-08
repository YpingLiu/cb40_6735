#ifndef SHF_SENSOR_H_
#define SHF_SENSOR_H_

#include "shf_define.h"
#include <dev/sensor_manager.h>

#ifndef SHF_DISABLE_ACTIVITY
extern uint32_t activity_window_size;
extern uint32_t activity_data_count;
#endif

//collect listened data
void shf_sensor_collect_scp_state(shf_data_index_t data_index, uint8_t enable_disable);
//init algorithm
void shf_sensor_init_algo(uint8_t sensor_type);
//enable/disable sensor according to sensor data listened state
void shf_sensor_config();
//listen sensor real state change for that
//sensor driver may don't enable/disable the sensor for other purpose,
//such as the sensor is also used by AP.
void shf_sensor_ap_state_callback(int sensor_type, sensor_manager_state_e enable_disable);
//Proximity sensor is needed by inpocket and pickup.
//Bit 0 indicates whether the pickup needs ps, bit 1 indicates whether the inpocket needs ps.
uint8_t shf_sensor_mask_proximity(uint8_t sensor_type, uint32_t enable);
//register ap state callback function
void shf_sensor_init();

/******************************************************************************
 * Unit Test Function
******************************************************************************/
#ifdef SHF_UNIT_TEST_ENABLE
void shf_sensor_unit_test();
#endif

#endif /* SHF_SENSOR_H_ */
