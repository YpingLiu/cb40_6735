#include "shf_unit_test.h"

#ifdef SHF_UNIT_TEST_ENABLE

#ifndef MOCK_DEVICE_H_
#define MOCK_DEVICE_H_

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef status_t
typedef int status_t;
#endif

#ifndef SHF_STATUS_OK
#define SHF_STATUS_OK (0x00)
#endif

#ifndef UINT32
typedef uint32_t UINT32;
#endif

#ifndef UINT16
typedef uint16_t UINT16;
#endif

#ifndef UINT8
typedef uint8_t UINT8;
#endif

#ifndef BOOL
typedef unsigned char  BOOL;
#endif

struct timespec {
    long tv_sec;
    long tv_nsec;
};
struct timekeeper {
    long xtime_sec;
    long xtime_nsec;
};
void getnstimeofday(struct timespec* ts);

//--------- Flag section
#define VFIFO_TX_ENABLE_FLAG	(1<<0)
#define VFIFO_RX_ENABLE_FLAG	(1<<1)

//---------------------------------------------------------------------------
typedef enum {
	uart_port1=0,
	//uart_port2,
	//uart_port3,
	//uart_port4,
	uart_max_port,
	uart_port_null = 99  /* a dummy port for those who doesn't use physical port */
}UART_port_id;

typedef enum {
	len_5=5,
	len_6,
	len_7,
	len_8
}UART_bits_per_ch;

typedef enum {
	sb_1=1,
	sb_2,
	sb_1_5
}UART_stop_bits;

typedef enum {
	pa_none=0,
	pa_odd,
	pa_even,
	pa_space,
	pa_mark,
}UART_parity;

typedef enum {
	fc_none=1,
	fc_hw,
	fc_sw,
	fc_sw_mtk,
}UART_flow_ctrl_mode;

typedef UINT32 UART_baudrate;
#define     UART_BAUD_115200        115200

typedef struct
{
	UART_baudrate		baud;
	UART_bits_per_ch	dataBits;
	UART_stop_bits		stopBits;
	UART_parity		parity;
	UART_flow_ctrl_mode	flowControl;
	UINT8			xonChar;
	UINT8			xoffChar;
	UINT8			xon1Char;
	UINT8			xoff1Char;
	UINT8			escapeChar;
}UART_dcb_struct;

typedef struct
{
      UINT8	*rx_adrs;
      UINT8	*tx_adrs;
      UINT16	rx_len;
      UINT16	tx_len;
}UART_ring_buffer_struct;

typedef struct __BUFFER_INFO
{
	uint16_t	Read;		                       /* @field Current Read index.  */
	uint16_t	Write;		                    /* @field Current Write index. */
	uint16_t	Length;		                    /* @field Length of buffer     */
	uint8_t	   *CharBuffer;		              /* @field Start of buffer      */
}BUFFER_INFO;

typedef struct
{
	UART_port_id		port_id;
	BOOL			initialized;
	BOOL			opened;
	BOOL			use_vfifo;
	UART_dcb_struct		dcb;
	BUFFER_INFO		rx_buffer;  /* receive buffer */
	BUFFER_INFO		tx_buffer;  /* transmit buffer */
	UINT32			uart_reg_base;
	UINT32			uart_irq_id;
	UINT32			rx_dma_reg_base;
	UINT32			tx_dma_reg_base;
	UINT32			rx_dma_irq_id;
	UINT32			tx_dma_irq_id;
	UINT32			dropped;
	UINT32			flag;
	UINT32			init_flag;
	void*			rx_dma_static_info;
	void*			rx_dma_run_time_info;
	void*			tx_dma_static_info;
	void*			tx_dma_run_time_info;
	void*			uart_static_info;
	void*			uart_run_time_info;
}UART_ctrl_struct;


BOOL UART_init(UART_port_id port, UINT32 flag,
		UINT32 vff_tx_nc_mem, UINT32 vff_tx_len,
		UINT32 vff_rx_nc_mem, UINT32 vff_rx_len);
BOOL UART_config(UART_port_id port, UART_dcb_struct *UART_Config);
UINT32 UART_read(UART_port_id port, UINT8 *Buffaddr, UINT32 NumOfBytes);
UINT32 UART_write(UART_port_id port, UINT8 *Buffaddr, UINT32 NumOfBytes);
void UART_btif_enable(UINT32 enable);
void UART_btif_register(UART_port_id port, UINT8 (*BTIF_RELAY_RX_HANDLER)(UINT8 *Buffaddr, UINT32 NumOfBytes));
BOOL UART_open(UART_port_id port, UINT32 flag);
BOOL UART_close(UART_port_id port);

enum ipi_id
{
    IPI_SHF,
};
enum ipi_status {
    ERROR = -1,
    DONE,
    BUSY
} ;
typedef enum ipi_status ipi_status;

typedef void(*ipi_handler_t)(int id, void * data, uint len);
enum ipi_status md32_ipi_registration(enum ipi_id id, ipi_handler_t handler, const char *name);
enum ipi_status md32_ipi_send(enum ipi_id id, void* buf, uint len, unsigned int wait);

void touch_gesture_switch(BOOL enable);

#endif /* MOCK_DEVICE_H_ */
#endif /* SHF_UNIT_TEST_ENABLE */
