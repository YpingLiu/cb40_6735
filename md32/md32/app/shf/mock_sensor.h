#ifndef MOCK_SENSOR_H_
#define MOCK_SENSOR_H_

#include "shf_unit_test.h"

#ifdef SHF_UNIT_TEST_ENABLE

#include "shf_types.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} acceleration_t;
typedef struct {
//    float_t length;
//    float_t frequency;
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
    uint8_t data_length;
    uint8_t sensor_type;
    uint8_t reserved[2];
    timestamp_t timestamp;
    union {
        uint16_t data[6];
        acceleration_t acceleration;
        uint16_t light;
        uint16_t proximity;
        pedometer_t pedometer;
        activity_t activity;
        //inpocket_t inpocket;
        uint16_t inpocket;
        uint32_t step_counter;
        uint16_t step_detector;
        uint16_t significant_motion;
		uint16_t pick_up;
		uint16_t face_down;
		uint16_t shake;
    } data;
} sensor_event_t;

/*
typedef struct {
	acceleration_t acceleration;
    uint16_t light;
    uint16_t proximity;
} inpocket_algorithm_input_t;
typedef struct {
    uint8_t confidence;
} inpocket_algorithm_output_t;
int inpocket_algorithm_run(inpocket_algorithm_input_t, void*);

typedef struct{
    int xmg;
    int ymg;
    int zmg;
} acceleromer_g_data_t;
typedef struct {
    uint16_t in_vehicle;
    uint16_t on_bicycle;
    uint16_t on_foot;
    uint16_t still;
    uint16_t unknown;
    uint16_t tilt;
} activityRecognition_algorithm_output_t;
int activityRecognition_algorithm_run(acceleromer_g_data_t, void*);

typedef struct {
    uint16_t value;
} significantMotion_algorithm_output_t;
int significantMotion_algorithm_run(acceleromer_g_data_t, void*);

typedef struct {
    uint16_t value;
} pickup_algorithm_output_t;
int pickup_algorithm_run(acceleromer_g_data_t, void*);

typedef struct {
    uint16_t value;
} shake_algorithm_output_t;
int shake_algorithm_run(acceleromer_g_data_t, void*);

typedef struct {
    uint16_t value;
} facedown_algorithm_output_t;
int facedown_algorithm_run(acceleromer_g_data_t, void*);
*/

//#define SENSOR_TYPE_ALL                  0X00
//#define SENSOR_TYPE_ACCELEROMETER        0X01
//#define SENSOR_TYPE_LIGHT                0X02
//#define SENSOR_TYPE_PROXIMITY            0X03
//
//#define SENSOR_TYPE_SIGNIFICANT          0X11
//#define SENSOR_TYPE_STEP_DETECT          0X12
//#define SENSOR_TYPE_STEP_COUNT           0X13
//#define SENSOR_TYPE_PEDOMETER            0X1D
//#define SENSOR_TYPE_ACTIVITY             0X1E
//#define SENSOR_TYPE_INPOCKET             0X1F

#define SENSOR_TYPE_ACCELEROMETER                   (1)
#define SENSOR_TYPE_MAGNETIC_FIELD                  (2)
#define SENSOR_TYPE_ORIENTATION                     (3)
#define SENSOR_TYPE_GYROSCOPE                       (4)
#define SENSOR_TYPE_LIGHT                           (5)
#define SENSOR_TYPE_PRESSURE                        (6)
#define SENSOR_TYPE_TEMPERATURE                     (7)
#define SENSOR_TYPE_PROXIMITY                       (8)
#define SENSOR_TYPE_GRAVITY                         (9)
#define SENSOR_TYPE_LINEAR_ACCELERATION             (10)
#define SENSOR_TYPE_ROTATION_VECTOR                 (11)

#define SENSOR_TYPE_RELATIVE_HUMIDITY               (12)
#define SENSOR_TYPE_AMBIENT_TEMPERATURE             (13)
#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED     (14)
#define SENSOR_TYPE_GAME_ROTATION_VECTOR            (15)
#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED          (16)
#define SENSOR_TYPE_SIGNIFICANT_MOTION              (17)
#define SENSOR_TYPE_STEP_DETECTOR                   (18)
#define SENSOR_TYPE_STEP_COUNTER                    (19)
#define SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR     (20)

#define SENSOR_TYPE_PEDOMETER           (21)
#define SENSOR_TYPE_INPOCKET            (22)
#define SENSOR_TYPE_ACTIVITY            (23)
#define SENSOR_TYPE_PICK_UP             (24)
#define SENSOR_TYPE_FACE_DOWN           (25)
#define SENSOR_TYPE_SHAKE               (26)

#define SENSOR_TYPE_ALL                 (26)
#define SENSOR_TYPE_MAX_COUNT           (26)

#define SENSOR_ACTIVATE_CMD     0X01
#define SENSOR_SETDELAY_CMD     0X02
#define SENSOR_POLL_CMD         0X03
#define SENSOR_SAVE_CMD         0X04
#define SENSOR_STATUS_CMD       0x05
#define SENSOR_BATCH_CMD        0x06


typedef enum {
	CLOSE,
	DEACTIVATE = 0,
	ACTIVATE
} sensor_manager_state_e;

int SCP_Sensor_Manager_open(uint32_t sensor_type);
int SCP_Sensor_Manager_control(uint32_t sensor_type, uint32_t cmd, void* arg);
int SCP_Sensor_Manager_close(uint32_t sensor_type);

#endif

#endif /* MOCK_SENSOR_H_ */
