#ifndef SHF_COMMUNICATOR_H_
#define SHF_COMMUNICATOR_H_

#include "shf_define.h"

/**
 * Now, we only support max length is 255(uint8),
 * but, we define API length is size_t,
 * so if we want to support more size,
 * we can only modify .c file.
 */

#define SHF_AP_BUFFER_BYTES (SHF_IPI_PROTOCOL_BYTES * 2)

#ifdef SHF_CONSYS_TYPE_UART
#define SHF_CONSYS_BUFFER_BYTES (SHF_UART_PROTOCOL_BYTES * 1)
#else
#define SHF_CONSYS_BUFFER_BYTES (SHF_IPI_PROTOCOL_BYTES * 1)
#endif

#define SHF_PROTOCOL_SEND_BUFFER_BYTES (256)

typedef uint8_t shf_device_t;
// device enum @{
#define SHF_DEVICE_AP              (0x01)
#define SHF_DEVICE_CONSYS          (0x02)
#define SHF_DEVICE_TOUCH_PANEL     (0x03)
#define SHF_DEVICE_DRAM            (0x04)
//@}

typedef void (*communicator_handler_t)(void * data, size_t size);

status_t shf_communicator_send_message(shf_device_t device, void* data, size_t size);
status_t shf_communicator_receive_message(shf_device_t device, communicator_handler_t handler);
status_t shf_communicator_wakeup(shf_device_t device);
status_t shf_communicator_enable_disable(shf_device_t device, bool_t enable);
void shf_communicator_run();
void shf_communicator_init();

#ifdef SHF_UNIT_TEST_ENABLE
void shf_communicator_unit_test();
#endif

#ifdef SHF_INTEGRATION_TEST_ENABLE
void shf_communicator_it();
void shf_communicator_it2();
#endif

#endif /* SHF_COMMUNICATOR_H_ */
