#ifndef __IPI_H
#define __IPI_H

#include <platform/mt_ipi.h>

#define MD32_TO_HOST_REG  (*(volatile unsigned int *)0xD005001C)
#define MD32_TO_SPM_REG   (*(volatile unsigned int *)0xD0050020)
#define HOST_TO_MD32_REG  (*(volatile unsigned int *)0xD0050024)
#define SEM_REG           (*(volatile unsigned int *)0xD0050090)

#define SEMA_0_MD32       (1 << 0)
#define IPC_MD2HOST       (1 << 0)
#define IPC_MD2SPM        (1 << 0)
#define MD32_IPC_INT      (1 << 8)
#define WDT_INT           (1 << 9)
#define PMEM_DISP_INT     (1 << 10)
#define DMEM_DISP_INT     (1 << 11)

enum ipi_status
{
    ERROR =-1,
    DONE,
    BUSY,
};

typedef void(*ipi_handler_t)(int id, void * data, uint len);

struct ipi_desc_t{
    ipi_handler_t handler;
    const char  *name;
};

#define SHARE_BUF_SIZE 64
struct share_obj {
    enum ipi_id id;
    unsigned int len;
    unsigned char reserve[8];
    unsigned char share_buf[SHARE_BUF_SIZE - 16];
};

void md32_ipi_init(void);
ipi_status md32_ipi_registration(enum ipi_id id, ipi_handler_t handler, const char *name);

ipi_status md32_ipi_send(enum ipi_id id, void* buf, uint len, unsigned int wait);
ipi_status md32_ipi_status(enum ipi_id id);

#endif
