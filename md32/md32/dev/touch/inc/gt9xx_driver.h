#ifndef __GT9XX_DRIVER_H__
#define __GT9XX_DRIVER_H__

/*****************************************************************************
 * GT9XX specific definition
 ****************************************************************************/
//Register define
#define GTP_READ_COOR_ADDR          0x814E
#define GTP_REG_SLEEP               0x8040
#define GTP_REG_SENSOR_ID           0x814A
#define GTP_REG_CONFIG_DATA         0x8047
#define GTP_REG_VERSION             0x8140
#define GTP_REG_HW_INFO             0x4220
#define GTP_REG_REFRESH_RATE		0x8056

#define GTP_DBL_CLK_WAKEUP          0
#define GTP_ADDR_LENGTH             2

/*****************************************************************************
 * Platform
 ****************************************************************************/
#if defined(MACH_FPGA)
#define fpga_gpio_base  0x10001E80
#define GTP_GPIO_AS_INPUT(pin)      do{} while(0)
#define GTP_GPIO_AS_INT(pin)        do{} while(0)
#define GTP_GPIO_GET_VALUE(pin)     do{} while(0)
#define GTP_GPIO_OUTPUT(pin,level)  do{\
                                        if(pin == tpd_rst_port){\
                                            *(volatile u32*)(fpga_gpio_base + 0x88) |= (0x1<<16);\
                                            if (level)\
                                                *(volatile u32*)(fpga_gpio_base + 0x84) |= (0x1<<16);\
                                            else\
                                                *(volatile u32*)(fpga_gpio_base + 0x84) &= ~(0x1 << 16);\
                                        }\
                                    }while(0)


#else

#define GTP_GPIO_AS_INPUT(pin)          do{\
                                            if(pin == GPIO_CTP_EINT_PIN)\
                                                mt_set_gpio_mode(pin, GPIO_MODE_GPIO);\
                                            else\
                                                mt_set_gpio_mode(pin, GPIO_MODE_GPIO);\
                                            mt_set_gpio_dir(pin, GPIO_DIR_IN);\
                                            mt_set_gpio_pull_enable(pin, GPIO_PULL_DISABLE);\
                                        }while(0)
#define GTP_GPIO_AS_INT(pin)            do{\
                                            mt_set_gpio_mode(pin, GPIO_MODE_04);\
                                            mt_set_gpio_dir(pin, GPIO_DIR_IN);\
                                            mt_set_gpio_pull_enable(pin, GPIO_PULL_DISABLE);\
                                        }while(0)
#define GTP_GPIO_GET_VALUE(pin)         mt_get_gpio_in(pin)
#define GTP_GPIO_OUTPUT(pin,level)      do{\
                                            if(pin == GPIO_CTP_EINT_PIN)\
                                                mt_set_gpio_mode(pin, GPIO_MODE_GPIO);\
                                            else\
                                                mt_set_gpio_mode(pin, GPIO_MODE_GPIO);\
                                            mt_set_gpio_dir(pin, GPIO_DIR_OUT);\
                                            mt_set_gpio_out(pin, level);\
                                        }while(0)
#endif

/*****************************************************************************
 * General macro
 ****************************************************************************/
#define GTP_DEBUG_ON                   0
#define GTP_ERROR(fmt,arg...)          dprintf(CRITICAL,"<GTP-ERR>"fmt"\n", ##arg)
#define GTP_INFO(fmt,arg...)           dprintf(CRITICAL,"<GTP-INF>"fmt"\n", ##arg)
#if GTP_DEBUG_ON
#define GTP_DEBUG(fmt,arg...)          do{\
                                         dprintf(CRITICAL,"<GTP-DBG>"fmt"\n", ##arg);\
                                       }while(0)
#else
#define GTP_DEBUG(fmt,arg...)		   	
#endif
// System & Platform dependent 
#define TPD_THREAD_ID   THREAD_TOUCH
#define TPD_INT_PORT    0
#define TPD_HW_SEM      SEMAPHORE_TOUCH

#define MAX_TRANSACTION_LENGTH        I2C_FIFO_SIZE
#define I2C_MASTER_CLOCK              100
#define MAX_I2C_TRANSFER_SIZE         (MAX_TRANSACTION_LENGTH - GTP_ADDR_LENGTH)

typedef void (*GES_CBFUNC)(u8);
/*****************************************************************************
 * ENUM
 ****************************************************************************/
typedef enum
{
    GTP_UNKNOWN = 0,
    GTP_NORMAL,
    GTP_DOZE,
    GTP_SLEEP,
}GTP_WORK_STATE;

typedef enum
{
    DOZE_INPOCKET = 0,
    DOZE_NOT_INPOCKET = 1,
}TOUCH_DOZE_T1;

typedef enum
{
    DOZE_DISABLE = 0,
    DOZE_ENABLE = 1,
}TOUCH_DOZE_T2;

typedef enum
{
    TOUCH_WAKE_BY_NONE,
    TOUCH_WAKE_BY_INT,
    TOUCH_WAKE_BY_IPI,    
    TOUCH_WAKE_BY_SWITCH
}TOUCH_WAKE_T;

typedef enum
{
    //SCP->AP
    IPI_COMMAND_SA_GESTURE_TYPE,

    //AP->SCP
    IPI_COMMAND_AS_CUST_PARAMETER,
    IPI_COMMAND_AS_ENTER_DOZEMODE,
    IPI_COMMAND_AS_ENABLE_GESTURE,
    IPI_COMMAND_AS_GESTURE_SWITCH,

}TOUCH_IPI_CMD_T;

/*****************************************************************************
 * STRUCTURE
 ****************************************************************************/
typedef struct
{
	u8 id;
	GES_CBFUNC cb;
}Touch_SmartWake_ID;

typedef struct
{
    u32 i2c_num;
    u32 int_num;
    u32 io_int;
    u32 io_rst;
}Touch_Cust_Setting;

typedef struct 
{
    u32 cmd;

    union {
        u32 data;
        Touch_Cust_Setting tcs;
    } param;
}Touch_IPI_Packet;

#endif //__GT9XX_DRIVER_H__

