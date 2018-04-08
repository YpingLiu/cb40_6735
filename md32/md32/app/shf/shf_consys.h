#ifndef SHF_CONSYS_H_
#define SHF_CONSYS_H_

#include <kernel/ipi.h>

#define MD32_TO_CONSYS_REG   (*(volatile unsigned int *)0xD0000094)
#define CONSYS_TO_MD32_REG   (*(volatile unsigned int *)0xD000009C)

typedef void(*consys_handler_t)(void* data, uint len);

#define CONSYS_BUF_SIZE 64
struct consys_obj {
    unsigned int len;
    unsigned char buf[CONSYS_BUF_SIZE - 4];
};

void shf_consys_init(void);
ipi_status shf_consys_registration(consys_handler_t handler);
ipi_status shf_consys_send(void* buf, uint len, unsigned int wait);

#endif
