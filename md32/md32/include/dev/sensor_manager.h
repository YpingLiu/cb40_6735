/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __SENSOR_MANAGER_H__
#define __SENSOR_MANAGER_H__

#include <sys/types.h>
#include "SCP_sensorHub.h"
#include "sensor_manager_fw.h"
#include <kernel/ipi.h>

#define ANDROID_L_NEW_SENSOR_SUPPORT    0

#define SENSOR_TYPE_ACCELEROMETER       1
#define SENSOR_TYPE_MAGNETIC_FIELD      2
#define SENSOR_TYPE_ORIENTATION         3
#define SENSOR_TYPE_GYROSCOPE           4
#define SENSOR_TYPE_LIGHT               5
#define SENSOR_TYPE_PRESSURE            6
#define SENSOR_TYPE_TEMPERATURE         7
#define SENSOR_TYPE_PROXIMITY           8
#define SENSOR_TYPE_GRAVITY             9
#define SENSOR_TYPE_LINEAR_ACCELERATION 10
#define SENSOR_TYPE_ROTATION_VECTOR     11
#define SENSOR_TYPE_SIGNIFICANT_MOTION  17
#define SENSOR_TYPE_STEP_DETECTOR       18
#define SENSOR_TYPE_STEP_COUNTER        19

#define SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR     20

#define SENSOR_TYPE_HEART_RATE          21
#define SENSOR_TYPE_TILT_DETECTOR       22
#define SENSOR_TYPE_WAKE_GESTURE        23
#define SENSOR_TYPE_GLANCE_GESTURE      24
#define SENSOR_TYPE_PICK_UP             25

#define SENSOR_TYPE_PEDOMETER           26
#define SENSOR_TYPE_INPOCKET            27
#define SENSOR_TYPE_ACTIVITY            28
#define SENSOR_TYPE_FACE_DOWN           29
#define SENSOR_TYPE_SHAKE               30

#define SENSOR_TYPE_ALL                 (31)
#define SENSOR_TYPE_MAX_COUNT           (31)
// @}

#define SENSOR_TYPE_NONE		(0xFF)

/*---------------------------------------------------------------------------*/
#define ID_BASE                    0
#define ID_ACCELEROMETER           (ID_BASE+SENSOR_TYPE_ACCELEROMETER-1)
#define ID_PROXIMITY               (ID_BASE+SENSOR_TYPE_PROXIMITY-1)
#define ID_LIGHT                   (ID_BASE+SENSOR_TYPE_LIGHT-1)
#define ID_PEDOMETER               (ID_BASE+SENSOR_TYPE_PEDOMETER-1)
#define ID_INPOCKET                (ID_BASE+SENSOR_TYPE_INPOCKET-1)
#define ID_ACTIVITY                (ID_BASE+SENSOR_TYPE_ACTIVITY-1)
#define ID_STEP_COUNTER            (ID_BASE+SENSOR_TYPE_STEP_COUNTER-1)
#define ID_STEP_DETECTOR           (ID_BASE+SENSOR_TYPE_STEP_DETECTOR-1)
#define ID_SIGNIFICANT_MOTION      (ID_BASE+SENSOR_TYPE_SIGNIFICANT_MOTION-1)
#define ID_HEART_RATE              (ID_BASE+SENSOR_TYPE_HEART_RATE-1)
#define ID_TILT_DETECTOR           (ID_BASE+SENSOR_TYPE_TILT_DETECTOR-1)
#define ID_WAKE_GESTURE            (ID_BASE+SENSOR_TYPE_WAKE_GESTURE-1)
#define ID_GLANCE_GESTURE          (ID_BASE+SENSOR_TYPE_GLANCE_GESTURE-1)
#define ID_PICK_UP                 (ID_BASE+SENSOR_TYPE_PICK_UP-1)
#define ID_FACE_DOWN               (ID_BASE+SENSOR_TYPE_FACE_DOWN-1)
#define ID_SHAKE                   (ID_BASE+SENSOR_TYPE_SHAKE-1)


#define ID_MAX_HANDLE              (ID_BASE+SENSOR_TYPE_MAX_COUNT-1)
#define ID_NONE                    (ID_BASE+31)

#define MAX_SCP_SENSOR_NUM              (ID_MAX_HANDLE+1)


#define SCP_SENSOR_TYPE_ACCELEROMETER       (0)
#define SCP_SENSOR_TYPE_LIGHT               (1)
#define SCP_SENSOR_TYPE_PROXIMITY           (2)
#define SCP_SENSOR_TYPE_SIGNIFICANT_MOTION  (3)
#define SCP_SENSOR_TYPE_STEP_DETECTOR       (4)
#define SCP_SENSOR_TYPE_STEP_COUNTER        (5)
#define SCP_SENSOR_TYPE_PEDOMETER           (6)
#define SCP_SENSOR_TYPE_INPOCKET            (7)
#define SCP_SENSOR_TYPE_ACTIVITY            (8)
#define SCP_SENSOR_TYPE_PICK_UP             (9)
#define SCP_SENSOR_TYPE_FACE_DOWN           (10)
#define SCP_SENSOR_TYPE_SHAKE               (11)
#define SCP_SENSOR_TYPE_HEART_RATE          (12)
#define SCP_SENSOR_TYPE_TILT_DETECTOR       (13)
#define SCP_SENSOR_TYPE_WAKE_GESTURE        (14)
#define SCP_SENSOR_TYPE_GLANCE_GESTURE      (15)
#define SCP_SENSOR_TYPE_ALL                 (16)
//#define SCP_SENSOR_TYPE_MAX_COUNT           (9)
#define SCP_SENSOR_TYPE_NONE                (0xFF)

#define SCP_SENSOR_TYPE_PHYSICAL_SENSORS    (3)

#define SCP_SENSOR_BATCH_FIFO_BATCH_SIZE    (2048)
#define SCP_SENSOR_BATCH_FIFO_THRESHOLD     (256)
#define DRAM_SENSOR_BATCH_FIFO_THRESHOLD    (SCP_SENSOR_BATCH_FIFO_BATCH_SIZE)

#define SCP_SENSOR_DEFAULT_DELAY            (200)
/*
 * flags for (*batch)()
 * Availability: SENSORS_DEVICE_API_VERSION_1_0
 * see (*batch)() documentation for details
 */
enum {
    SENSORS_BATCH_DRY_RUN               = 0x00000001,
    SENSORS_BATCH_WAKE_UPON_FIFO_FULL   = 0x00000002
};


typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} acceleration16_t; //mg

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} acceleration_t; //m-m^2/s

typedef struct {
	uint16_t light;
	uint16_t reserved[3];
} light_t;

typedef struct {
	uint16_t proximity;
	uint16_t reserved[3];
} proximity_t;

typedef struct {
	uint32_t length; //milli-meter
	uint32_t frequency; //freq*1000
	uint32_t count;
	uint32_t distance;
} pedometer_t;

typedef struct {
	uint16_t in_vehicle;
	uint16_t on_bicycle;
	uint16_t on_foot;
	uint16_t still;
	uint16_t unknown;
	uint16_t tilt;
} activity_t;

typedef struct {
	uint16_t inpocket;
} inpocket_t;

typedef struct {
	uint32_t step_counter;
} step_counter_t;

typedef struct {
	uint32_t step_detector;
} step_detector_t;

typedef struct {
	uint32_t significant_motion;
} significant_motion_t;

typedef struct {
	uint16_t pick_up;
} pick_up_t;

typedef struct {
	uint16_t face_down;
} face_down_t;

typedef struct {
	uint16_t shake;
} shake_t;

typedef struct {
	uint16_t bpm;
	uint16_t status;	
} heart_rate_t;

typedef struct {
	uint16_t tilt_detector;
} tilt_detector_t;

typedef struct {
	uint16_t wake_gesture;
} wake_gesture_t;

typedef struct {
	uint16_t glance_gesture;
} glance_gesture_t;

typedef struct {
	uint8_t data_length; //including header: data_length, sensor_type, reserved, timestamp
	uint8_t sensor_type;
	int8_t state;
	uint8_t reserved;
	uint64_t timestamp;
	union {
		uint16_t data[6];
		acceleration16_t acceleration16;
		acceleration_t acceleration;
		uint16_t light;
		uint16_t proximity;
		pedometer_t pedometer;
		activity_t activity;
		uint16_t inpocket;
		uint32_t step_counter;
		uint32_t step_detector;
		uint32_t significant_motion;
		// Added by Chuan for IT @{
		uint16_t pick_up;
		uint16_t face_down;
		uint16_t shake;
#if ANDROID_L_NEW_SENSOR_SUPPORT
		heart_rate_t heart_rate;
		uint16_t tilt_detector;
		uint16_t wake_gesture;
		uint16_t glance_gesture;
#endif //#if ANDROID_L_NEW_SENSOR_SUPPORT
	} data;
} sensor_event_t;

typedef struct {
    uint8_t data_length; //including header: data_length, sensor_type, reserved, timestamp
    uint8_t sensor_type;
    int8_t state;
    uint8_t reserved;
    uint64_t timestamp;
} sensor_event_header_t;

typedef struct {
	sensor_event_header_t acceleration_header;
	acceleration16_t acceleration16;
	sensor_event_header_t light_header;
	light_t light;
	sensor_event_header_t proximity_header;
	proximity_t proximity;
	sensor_event_header_t pedometer_header;
	pedometer_t pedometer;
	sensor_event_header_t activity_header;
	activity_t activity;
	sensor_event_header_t inpocket_header;
	uint16_t inpocket;
	sensor_event_header_t step_counter_header;
	uint32_t step_counter;
	sensor_event_header_t step_detector_header;
	uint32_t step_detector;
	sensor_event_header_t significant_motion_header;
	uint32_t significant_motion;
	sensor_event_header_t pick_up_header;
	uint16_t pick_up;
	sensor_event_header_t face_down_header;
	uint16_t face_down;
	sensor_event_header_t shake_header;
	uint16_t shake;
#if ANDROID_L_NEW_SENSOR_SUPPORT
	sensor_event_header_t heart_rate_header;
	heart_rate_t heart_rate;
	sensor_event_header_t tilt_detector_header;
	uint16_t tilt_detector;
	sensor_event_header_t wake_gesture_header;
	uint16_t wake_gesture;
	sensor_event_header_t glance_gesture_header;
	uint16_t glance_gesture;
#endif //#if ANDROID_L_NEW_SENSOR_SUPPORT
} sensor_all_event_t;

typedef struct {
	SCP_SENSOR_HUB_REQ req;
	sensor_all_event_t data;
} sensor_ipi_data_t;

typedef struct {
	void *pointer;
	uint32_t size;
} sensor_data_info_t;

typedef enum {
	CLOSE,
	DEACTIVATE = 0,
	ACTIVATE
} sensor_manager_state_e;

typedef enum {
	NONE_PROCRSS,
	TIMEOUT_PROCESS,
	IPI_PROCESS,
	FRAMEWORK_PROCESS = 2
} sensor_manager_process_e;

typedef void (*sm_activate_cbfunc)(int ap_sensor_type, sensor_manager_state_e state);

typedef struct {
	uint32_t   ap_delay[SCP_SENSOR_TYPE_ALL];
	uint32_t   ap_batch_timeout[SCP_SENSOR_TYPE_ALL];    //sampling time in ms
	uint32_t   scp_delay[SCP_SENSOR_TYPE_ALL];
	uint32_t   ap_next_delay; //bit map, command from AP side
	uint32_t   ap_next_get_data; // AP side sensor type
	uint32_t   scp_next_delay; //bit map, command from scp	
	uint32_t   delayCount[SCP_SENSOR_TYPE_ALL];
	uint32_t   delayCountSet[SCP_SENSOR_TYPE_ALL];
	uint32_t   delay;
	uint32_t   faster;
	uint32_t   ap_batch; //bit map for batching
	uint32_t   batch_wake_upon_fifo_full; //bit map for wake when fifo full
	uint32_t   ap_next_state; //bit map, command from AP side
	uint32_t   scp_next_state; //bit map, command from scp	
	uint32_t   state; //bit map, current each sensor state
	uint32_t   ap_set_cust; //bit map, specify which sensor set cust
	uint32_t   flush_to_ap;// bit map
	uint32_t   bufferSize;
	uint32_t   ap_scp_sensor_hub_data_len;
	uint32_t   ap_ipi_cmd;
	uint32_t   process;
	sensorFIFO *bufferBase;
	SCP_SENSOR_HUB_DATA ap_scp_sensor_hub_data;
	sm_activate_cbfunc activate_cb;
} sensor_manager_t;

extern void sensor_manager_set_activate(uint32_t sensor_type, uint32_t arg);
extern int sensor_manager_set_delay(uint32_t sensor_type, uint32_t arg);
extern int sensor_manager_get_data_struct_size(uint32_t sensor_type);
extern int sensor_manager_get_data(uint32_t sensor_type, void *arg);
extern int sensor_manager_save_data(uint32_t sensor_type, void *arg);
extern sensor_manager_state_e sensor_manager_get_status(uint32_t sensor_type);
extern ipi_status sensor_manager_ipi_send(enum ipi_id id, void* buf, uint len, unsigned int wait);
extern int sensor_manager_type_mapping_to_scp(int sensor_type);
extern int sensor_manager_type_mapping_to_ap(int scp_sensor_type);
extern int sensor_manager_id_mapping_to_scp(int sensor_id);
extern int sensor_manager_init();
extern int sensor_manager_register_activate_callback(uint32_t scp_sensor_type, sm_activate_cbfunc cb);

/////////Sensor Driver///////////////////////////////////////////////////
#define GRAVITY_EARTH_1000              9807	// about (9.80665f)*1000

#define SENSOR_DELAY	                0x01
#define	SENSOR_ENABLE	                0x02
#define	SENSOR_GET_DATA	                0x04
#define SENSOR_SET_CUST                 0x08

#define SENSOR_STATUS_UNRELIABLE        0
#define SENSOR_STATUS_ACCURACY_LOW      1
#define SENSOR_STATUS_ACCURACY_MEDIUM   2
#define SENSOR_STATUS_ACCURACY_HIGH     3

#define SENSOR_INVALID_VALUE -1
/*------------sensors data----------------------------------------------------*/
typedef struct {
	/* sensor identifier */
	int 	sensor;
	/* sensor values */
	int	values[3];
	/* sensor values divide */
	uint32_t value_divide;
	/* sensor accuracy*/
	int8_t status;
	/* whether updata? */
	int	update;
	/* time is in nanosecond */
	int64_t	time;
	uint32_t reserved;
}hwm_sensor_data;

struct sensor_delay 
{
	int handle;
	uint32_t delay;
};

/*-------------Sensor data-----------------------------------------------------*/
//struct sensor_driver_data{
//	hwm_sensor_data sensors_data[SCP_SENSOR_TYPE_MAX_COUNT+1];
//	int data_updata[SCP_SENSOR_TYPE_MAX_COUNT+1];
//};

struct sensor_driver_object {
	void *self;
	int polling;
	int (*sensor_operate)(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout);
};

struct sensor_driver_context { /*sensor context*/
	uint32_t enable;
	uint32_t delay;
//	uint32_t delayCountSet;
//	uint32_t delayCount;
	struct sensor_driver_object    obj;
};

/*----------------------------------------------------------------------------*/
//struct dev_context {
////	int    polling_running;
//	struct sensor_driver_context* cxt[SCP_SENSOR_TYPE_MAX_COUNT+1];
//};
/*----------------------------------------------------------------------------*/
//struct hwmdev_object {
//	struct dev_context *dc;
//	int             delay; /*polling period for reporting input event*/
////	uint32_t	active_sensor;			// Active, but sensor_driver don't need data sensor. Maybe other need it's data.
//	uint32_t	active_data_sensor;		// Active and sensor_driver need data sensor.
//};

#define C_MAX_SENSOR_AXIS_NUM          4

struct sensor_driver_convert {
	int8_t    sign[C_MAX_SENSOR_AXIS_NUM];
	uint8_t   map[C_MAX_SENSOR_AXIS_NUM];
};
/*----------------------------------------------------------------------------*/
extern int sensor_driver_get_convert(int direction, struct sensor_driver_convert *cvt);
extern int sensor_driver_enable(int sensor, int enable, void *req, void* rsp);
extern int sensor_driver_get_data(int sensor, SCP_SENSOR_HUB_DATA *rsp);
//extern int sensor_driver_set_config(int sensor, void *req);
extern int sensor_driver_set_cust(int sensor, void *req, void* rsp, int len);
extern int sensor_driver_set_delay(int sensor, int delay, void *req, void* rsp);
//extern int sensor_driver_notify(int sensor, void *req);
extern int sensor_driver_get_convert(int direction, struct sensor_driver_convert *cvt);
extern int sensor_driver_set_interrupt_data(int sensor, hwm_sensor_data *data, SCP_SENSOR_HUB_DATA* rsp, int rsp_len);
extern int sensor_driver_attach(int sensor, struct sensor_driver_object *obj);
/*----------------------------------------------------------------------------*/

#endif

