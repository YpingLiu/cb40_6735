/*
 * Copyright (c) 2009 Travis Geiselbrecht
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
  
#define PMIC_WRAP_BASE                  (0xE000D000)

#define PMIC_WRAP_MUX_SEL               (PMIC_WRAP_BASE+0x0)
#define PMIC_WRAP_WRAP_EN               (PMIC_WRAP_BASE+0x4)
#define PMIC_WRAP_DIO_EN                (PMIC_WRAP_BASE+0x8)
#define PMIC_WRAP_SIDLY                 (PMIC_WRAP_BASE+0xC)
#define PMIC_WRAP_RDDMY                 ((PMIC_WRAP_BASE+0x10))
#define PMIC_WRAP_SI_CK_CON             ((PMIC_WRAP_BASE+0x14))
#define PMIC_WRAP_CSHEXT_WRITE          ((PMIC_WRAP_BASE+0x18))
#define PMIC_WRAP_CSHEXT_READ           ((PMIC_WRAP_BASE+0x1C))
#define PMIC_WRAP_CSLEXT_START          ((PMIC_WRAP_BASE+0x20))
#define PMIC_WRAP_CSLEXT_END            ((PMIC_WRAP_BASE+0x24))
#define PMIC_WRAP_STAUPD_PRD            ((PMIC_WRAP_BASE+0x28))
#define PMIC_WRAP_STAUPD_GRPEN          ((PMIC_WRAP_BASE+0x2C))
#define PMIC_WRAP_EINT_STA0_ADR         ((PMIC_WRAP_BASE+0x30))
#define PMIC_WRAP_EINT_STA1_ADR         ((PMIC_WRAP_BASE+0x34))
#define PMIC_WRAP_EINT_STA              ((PMIC_WRAP_BASE+0x38))
#define PMIC_WRAP_EINT_CLR              ((PMIC_WRAP_BASE+0x3C))
#define PMIC_WRAP_STAUPD_MAN_TRIG       ((PMIC_WRAP_BASE+0x40))
#define PMIC_WRAP_STAUPD_STA            ((PMIC_WRAP_BASE+0x44))
#define PMIC_WRAP_WRAP_STA              ((PMIC_WRAP_BASE+0x48))
#define PMIC_WRAP_HARB_INIT             ((PMIC_WRAP_BASE+0x4C))
#define PMIC_WRAP_HARB_HPRIO            ((PMIC_WRAP_BASE+0x50))
#define PMIC_WRAP_HIPRIO_ARB_EN         ((PMIC_WRAP_BASE+0x54))
#define PMIC_WRAP_HARB_STA0             ((PMIC_WRAP_BASE+0x58))
#define PMIC_WRAP_HARB_STA1             ((PMIC_WRAP_BASE+0x5C))
#define PMIC_WRAP_MAN_EN                ((PMIC_WRAP_BASE+0x60))
#define PMIC_WRAP_MAN_CMD               ((PMIC_WRAP_BASE+0x64))
#define PMIC_WRAP_MAN_RDATA             ((PMIC_WRAP_BASE+0x68))
#define PMIC_WRAP_MAN_VLDCLR            ((PMIC_WRAP_BASE+0x6C))
#define PMIC_WRAP_WACS0_EN              ((PMIC_WRAP_BASE+0x70))
#define PMIC_WRAP_INIT_DONE0            ((PMIC_WRAP_BASE+0x74))
#define PMIC_WRAP_WACS0_CMD             ((PMIC_WRAP_BASE+0x78))
#define PMIC_WRAP_WACS0_RDATA           ((PMIC_WRAP_BASE+0x7C))
#define PMIC_WRAP_WACS0_VLDCLR          ((PMIC_WRAP_BASE+0x80))
#define PMIC_WRAP_WACS1_EN              ((PMIC_WRAP_BASE+0x84))
#define PMIC_WRAP_INIT_DONE1            ((PMIC_WRAP_BASE+0x88))
#define PMIC_WRAP_WACS1_CMD             ((PMIC_WRAP_BASE+0x8C))
#define PMIC_WRAP_WACS1_RDATA           ((PMIC_WRAP_BASE+0x90))
#define PMIC_WRAP_WACS1_VLDCLR          ((PMIC_WRAP_BASE+0x94))
#define PMIC_WRAP_WACS2_EN              ((PMIC_WRAP_BASE+0x98))
#define PMIC_WRAP_INIT_DONE2            ((PMIC_WRAP_BASE+0x9C))
#define PMIC_WRAP_WACS2_CMD             ((PMIC_WRAP_BASE+0xA0))
#define PMIC_WRAP_WACS2_RDATA           ((PMIC_WRAP_BASE+0xA4))
#define PMIC_WRAP_WACS2_VLDCLR          ((PMIC_WRAP_BASE+0xA8))
#define PMIC_WRAP_INT_EN                ((PMIC_WRAP_BASE+0xAC))
#define PMIC_WRAP_INT_FLG_RAW           ((PMIC_WRAP_BASE+0xB0))
#define PMIC_WRAP_INT_FLG               ((PMIC_WRAP_BASE+0xB4))
#define PMIC_WRAP_INT_CLR               ((PMIC_WRAP_BASE+0xB8))
#define PMIC_WRAP_SIG_ADR               ((PMIC_WRAP_BASE+0xBC))
#define PMIC_WRAP_SIG_MODE              ((PMIC_WRAP_BASE+0xC0))
#define PMIC_WRAP_SIG_VALUE             ((PMIC_WRAP_BASE+0xC4))
#define PMIC_WRAP_SIG_ERRVAL            ((PMIC_WRAP_BASE+0xC8))
#define PMIC_WRAP_CRC_EN                ((PMIC_WRAP_BASE+0xCC))
#define PMIC_WRAP_TIMER_EN              ((PMIC_WRAP_BASE+0xD0))
#define PMIC_WRAP_TIMER_STA             ((PMIC_WRAP_BASE+0xD4))
#define PMIC_WRAP_WDT_UNIT              ((PMIC_WRAP_BASE+0xD8))
#define PMIC_WRAP_WDT_SRC_EN            ((PMIC_WRAP_BASE+0xDC))
#define PMIC_WRAP_WDT_FLG               ((PMIC_WRAP_BASE+0xE0))
#define PMIC_WRAP_DEBUG_INT_SEL         ((PMIC_WRAP_BASE+0xE4))
#define PMIC_WRAP_DVFS_ADR0             ((PMIC_WRAP_BASE+0xE8))
#define PMIC_WRAP_DVFS_WDATA0           ((PMIC_WRAP_BASE+0xEC))
#define PMIC_WRAP_DVFS_ADR1             ((PMIC_WRAP_BASE+0xF0))
#define PMIC_WRAP_DVFS_WDATA1           ((PMIC_WRAP_BASE+0xF4))
#define PMIC_WRAP_DVFS_ADR2             ((PMIC_WRAP_BASE+0xF8))
#define PMIC_WRAP_DVFS_WDATA2           ((PMIC_WRAP_BASE+0xFC))
#define PMIC_WRAP_DVFS_ADR3             ((PMIC_WRAP_BASE+0x100))
#define PMIC_WRAP_DVFS_WDATA3           ((PMIC_WRAP_BASE+0x104))
#define PMIC_WRAP_DVFS_ADR4             ((PMIC_WRAP_BASE+0x108))
#define PMIC_WRAP_DVFS_WDATA4           ((PMIC_WRAP_BASE+0x10C))
#define PMIC_WRAP_DVFS_ADR5             ((PMIC_WRAP_BASE+0x110))
#define PMIC_WRAP_DVFS_WDATA5           ((PMIC_WRAP_BASE+0x114))
#define PMIC_WRAP_DVFS_ADR6             ((PMIC_WRAP_BASE+0x118))
#define PMIC_WRAP_DVFS_WDATA6           ((PMIC_WRAP_BASE+0x11C))
#define PMIC_WRAP_DVFS_ADR7             ((PMIC_WRAP_BASE+0x120))
#define PMIC_WRAP_DVFS_WDATA7           ((PMIC_WRAP_BASE+0x124))
#define PMIC_WRAP_SPMINF_STA            ((PMIC_WRAP_BASE+0x128))
#define PMIC_WRAP_CIPHER_KEY_SEL        ((PMIC_WRAP_BASE+0x12C))
#define PMIC_WRAP_CIPHER_IV_SEL         ((PMIC_WRAP_BASE+0x130))
#define PMIC_WRAP_CIPHER_EN             ((PMIC_WRAP_BASE+0x134))
#define PMIC_WRAP_CIPHER_RDY            ((PMIC_WRAP_BASE+0x138))
#define PMIC_WRAP_CIPHER_MODE           ((PMIC_WRAP_BASE+0x13C))
#define PMIC_WRAP_CIPHER_SWRST          ((PMIC_WRAP_BASE+0x140))
#define PMIC_WRAP_DCM_EN                ((PMIC_WRAP_BASE+0x144))
#define PMIC_WRAP_DCM_DBC_PRD           ((PMIC_WRAP_BASE+0x148))
#define PMIC_WRAP_EXT_CK                ((PMIC_WRAP_BASE+0x14C))

 
#define E_PWR_INVALID_ARG               1
#define E_PWR_INVALID_RW                2
#define E_PWR_INVALID_ADDR              3
#define E_PWR_INVALID_WDAT              4
#define E_PWR_INVALID_OP_MANUAL         5
#define E_PWR_NOT_IDLE_STATE            6
#define E_PWR_NOT_INIT_DONE             7
#define E_PWR_NOT_INIT_DONE_READ        8
#define E_PWR_WAIT_IDLE_TIMEOUT         9
#define E_PWR_WAIT_IDLE_TIMEOUT_READ    10
#define E_PWR_INIT_SIDLY_FAIL           11
#define E_PWR_RESET_TIMEOUT             12
#define E_PWR_TIMEOUT                   13

//macro for WACS_FSM
#define WACS_FSM_IDLE               (0x00)
#define WACS_FSM_REQ                (0x02)
#define WACS_FSM_WFDLE              (0x04) //wait for dle,wait for read data done,
#define WACS_FSM_WFVLDCLR           (0x06) //finish read data , wait for valid flag clearing
#define WACS_INIT_DONE              (0x01)
#define WACS_SYNC_IDLE              (0x01)
#define WACS_SYNC_BUSY              (0x00)


#define GET_WACS0_RDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS0_FSM(x)             ((x>>16) & 0x00000007)
#define GET_INIT_DONE0(x)            ((x>>21) & 0x00000001)

#define WRAP_RD32(addr)            (*(volatile uint32_t *)(addr))
#define WRAP_WR32(addr,data)       ((*(volatile uint32_t *)(addr)) = (uint32_t)data)


#define PMIC_WRAP_AUDIO_BASE       (0x2000)

