#include <sys/types.h>
#define VIC_CTR             (*(volatile unsigned int *)(0xF0000000))
#define VIC_IMR             (*(volatile unsigned int *)(0xF0000004))
#define VIC_PEND            (*(volatile unsigned int *)(0xF0000008))
#define VIC_VEC             (*(volatile unsigned int *)(0xF000000C))

#define	IRQ_UNMASK_INT  0x1FF0000
#define	IRQ_MASK_INT   ~0x1FFffff

#define MT_IRQ_ID_RESET                     0
#define MT_IRQ_ID_DIV_ZERO                  6
#define MT_IRQ_ID_UNALIGNED_MEM_ACCESS      7
#define MT_IRQ_ID_IRQ_AGGREGATOR            16
#define MT_IRQ_ID_INTERNAL_TIMER            17
#define MT_IRQ_ID_MAD_FIFO                  18
#define MT_IRQ_ID_IPC0                      19
#define MT_IRQ_ID_SPM                       20
#define MT_IRQ_ID_EINT                      21
#define MT_IRQ_ID_TIMER1                    22
#define MT_IRQ_ID_TIMER0                    23
#define MT_IRQ_ID_I2C2                      24

/* Aggregator IRQ*/
#define MT_IRQ_ID_PMIC                      25
#define MT_IRQ_ID_UART                      26
#define MT_IRQ_ID_CLK_CTRL                  27
#define MT_IRQ_ID_I2C1                      28
#define MT_IRQ_ID_I2C0                      29
#define MT_IRQ_ID_CIRQ                      30
#define MT_IRQ_ID_IPC3                      31
#define MT_IRQ_ID_IPC2                      32
#define MT_IRQ_ID_IPC1                      33
#define MT_IRQ_ID_TIMER3                    34
#define MT_IRQ_ID_TIMER2                    35
#define MT_IRQ_ID_UART_RX                   36
#define MT_IRQ_ID_LAST                      MT_IRQ_ID_UART_RX
#define MT_AG_IRQ_ID_FIRST                  MT_IRQ_ID_PMIC
#define MT_AG_IRQ_TOTAL                     (MT_IRQ_ID_LAST - MT_AG_IRQ_ID_FIRST + 1)

#define IRQ_MAX_CHANNEL  (MT_IRQ_ID_LAST + 1)

#define MT_IPC_HOST_IRQ_ID  MT_IRQ_ID_IPC0

typedef handler_return (*irq_handler_t)(void);

struct irq_desc_t{
	irq_handler_t handler;
	const char  *name;
};

void request_irq(unsigned int irq, irq_handler_t handler, const char *name);
void init_intr_ctrl(void);
void mask_irq(unsigned int irq);
void unmask_irq(unsigned int irq);
int mask_all_irq(unsigned int *mask);
int restore_irq_mask(unsigned int *mask);
void scp_wakeup_src_setup(unsigned int irq, int enable);

