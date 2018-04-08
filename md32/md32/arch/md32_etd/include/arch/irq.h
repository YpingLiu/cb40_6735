#include <sys/types.h>
#define VIC_CTR             (*(volatile unsigned int *)(0xF0000000))
#define VIC_IMR             (*(volatile unsigned int *)(0xF0000004))
#define VIC_PEND            (*(volatile unsigned int *)(0xF0000008))
#define VIC_VEC             (*(volatile unsigned int *)(0xF000000C))


#define ANC_260K_IRQ   16
#define ANC_16K_IRQ    18
#define	IRQ_UNMASK_INT  0x1FF0000
#define	IRQ_MASK_INT   ~0x1FFffff

#define IRQ_MAX_CHANNEL 25

#define MT_SPM_IRQ_ID  20
#define MT_IPC_HOST_IRQ_ID 22
#define MT_XGPT_IRQ_ID  24

typedef handler_return (*irq_handler_t)(void);

struct irq_desc{
	irq_handler_t handler;
	const char  *name;
};

void request_irq(unsigned int irq, irq_handler_t handler, const char *name);
void init_intr_ctrl(void);
void mask_irq(unsigned int irq);
void unmask_irq(unsigned int irq);
int mask_all_irq(unsigned int *mask);
int restore_irq_mask(unsigned int *mask);
