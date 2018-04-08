/* BEGIN PN:DTS2013053103858 , Added by d00238048, 2013.05.31*/
#ifndef BUILD_LK
#include <linux/string.h>
#endif

#include "lcm_drv.h"
#include <cust_gpio_usage.h>

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <string.h>
#include <platform/mt_i2c.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <mach/mt_gpio.h>
#endif

#ifdef BUILD_LK
#define LCD_DEBUG(...)  dprintf(CRITICAL,__VA_ARGS__)
#else
#define LCD_DEBUG(fmt)  printk(fmt)
#endif

//const static unsigned char LCD_MODULE_ID = 0x02;
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define   LCM_DSI_CMD_MODE                                  0

#define FRAME_WIDTH                                         (1024) 	// (720)
#define FRAME_HEIGHT                                        (1024) 		// (1280)

#define REGFLAG_DELAY                                       0xFC
#define REGFLAG_END_OF_TABLE                               0xFD   // END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

//---------------------------------------------------------------------------
//  gpio set @2016-1-15, by bruce
//---------------------------------------------------------------------------
#define GPIO_LCM_BOOST_EN_PIN    GPIO19	// 1

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define SET_RESET_PIN(v)                                    (lcm_util.set_reset_pin((v)))

#define UDELAY(n)                                           (lcm_util.udelay(n))
#define MDELAY(n)                                           (lcm_util.mdelay(n))

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                      lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                  lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                       lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)               lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define dsi_lcm_set_gpio_out(pin, out)                      lcm_util.set_gpio_out(pin, out)
#define dsi_lcm_set_gpio_mode(pin, mode)                    lcm_util.set_gpio_mode(pin, mode)
#define dsi_lcm_set_gpio_dir(pin, dir)                      lcm_util.set_gpio_dir(pin, dir)
#define dsi_lcm_set_gpio_pull_enable(pin, en)               lcm_util.set_gpio_pull_enable(pin, en)

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[128];
};

//update initial param for IC nt35521 0.01
static struct LCM_setting_table lcm_initialization_setting[] = {
#if 0
    {0x62,1,{0x01}},
    {0xFF,4,{0xAA,0x55,0x25,0x01}},

    // {0x6F,1,{0x16}},
    //{0xF7,1,{0x10}},
    {0x6F,1,{0x1A}},
    {0xF7,1,{0x05}},
    //{0xFF,4,{0xAA,0x55,0x25,0x00}},
    //{REGFLAG_DELAY, 5, {}},

    {0xFF,4,{0xAA,0x55,0xA5,0x80}},

    {0x6F,2,{0x11,0x00}},
    {0xF7,2,{0x20,0x00}},

    {0x6F,1,{0x06}},
    {0xF7,1,{0xA0}},
    {0x6F,1,{0x19}},
    {0xF7,1,{0x12}},

    {0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
    {0xC8,1,{0x80}},

    {0xB1,2,{0x6c,0x21}},

    {0xB6,1,{0x0F}},
    {0x6F,1,{0x02}},
    {0xB8,1,{0x08}},

    {0xBB,2,{0x74,0x44}},

    {0xBC,2,{0x00,0x00}},

    {0xBD,5,{0x02,0xB0,0x1e,0x1e,0x00}},

    {0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},

    {0xB0,2,{0x05,0x05}},
    {0xB1,2,{0x05,0x05}},

    {0xBC,2,{0x70,0x00}},
    {0xBD,2,{0x70,0x00}},

    {0xCA,1,{0x00}},

    {0xC0,1,{0x04}},

    {0xBE,1,{0x40}},

    {0xB9,2,{0x46,0x46}},
    {0xB3,2,{0x2D,0x2D}},

    {0xBA,2,{0x25,0x25}},
    {0xB4,2,{0x19,0x19}},

    {0xF0,5,{0x55,0xAA,0x52,0x08,0x02}},

    {0xEE,1,{0x02}},

    {0xB0,16,{0x00,0x6E,0x00,0x82,0x00,0xA6,0x00,0xBD,0x00,0xCA,0x00,0xE9,0x01,0x05,0x01,0x31}},
    {0xB1,16,{0x01,0x53,0x01,0x8A,0x01,0xB2,0x01,0xF7,0x02,0x31,0x02,0x33,0x02,0x67,0x02,0x9C}},
    {0xB2,16,{0x02,0xBC,0x02,0xEC,0x03,0x0B,0x03,0x32,0x03,0x4B,0x03,0x6E,0x03,0x84,0x03,0xA0}},
    {0xB3,4,{0x03,0xBF,0x03,0xD3}},

    {0xBC,16,{0x00,0x15,0x00,0x26,0x00,0x45,0x00,0x63,0x00,0x83,0x00,0xAF,0x00,0xD1,0x01,0x02}},
    {0xBD,16,{0x01,0x2C,0x01,0x70,0x01,0xA0,0x01,0xEE,0x02,0x27,0x02,0x29,0x02,0x5C,0x02,0x9B}},
    {0xBE,16,{0x02,0xC0,0x02,0xF4,0x03,0x16,0x03,0x40,0x03,0x5B,0x03,0x80,0x03,0x96,0x03,0xAF}},
    {0xBF,4,{0x03,0xC5,0x03,0xD3}},

    {0xF0,5,{0x55,0xAA,0x52,0x08,0x06}},

    {0xB0,2,{0x29,0x2A}},
    {0xB1,2,{0x10,0x12}},
    {0xB2,2,{0x14,0x16}},
    {0xB3,2,{0x18,0x1A}},
    {0xB4,2,{0x08,0x0A}},
    {0xB5,2,{0x2E,0x2E}},
    {0xB6,2,{0x2E,0x2E}},
    {0xB7,2,{0x2E,0x2E}},
    {0xB8,2,{0x2E,0x00}},
    {0xB9,2,{0x2E,0x2E}},

    {0xBA,2,{0x2E,0x2E}},
    {0xBB,2,{0x01,0x2E}},
    {0xBC,2,{0x2E,0x2E}},
    {0xBD,2,{0x2E,0x2E}},
    {0xBE,2,{0x2E,0x2E}},
    {0xBF,2,{0x0B,0x09}},
    {0xC0,2,{0x1B,0x19}},
    {0xC1,2,{0x17,0x15}},
    {0xC2,2,{0x13,0x11}},
    {0xC3,2,{0x2A,0x29}},
    {0xE5,2,{0x2E,0x2E}},

    {0xC4,2,{0x29,0x2A}},
    {0xC5,2,{0x1B,0x19}},
    {0xC6,2,{0x17,0x15}},
    {0xC7,2,{0x13,0x11}},
    {0xC8,2,{0x01,0x0B}},
    {0xC9,2,{0x2E,0x2E}},
    {0xCA,2,{0x2E,0x2E}},
    {0xCB,2,{0x2E,0x2E}},
    {0xCC,2,{0x2E,0x09}},
    {0xCD,2,{0x2E,0x2E}},

    {0xCE,2,{0x2E,0x2E}},
    {0xCF,2,{0x08,0x2E}},
    {0xD0,2,{0x2E,0x2E}},
    {0xD1,2,{0x2E,0x2E}},
    {0xD2,2,{0x2E,0x2E}},
    {0xD3,2,{0x0A,0x00}},
    {0xD4,2,{0x10,0x12}},
    {0xD5,2,{0x14,0x16}},
    {0xD6,2,{0x18,0x1A}},
    {0xD7,2,{0x2A,0x29}},

    {0xE6,2,{0x2E,0x2E}},

    {0xD8,5,{0x00,0x00,0x00,0x00,0x00}},
    {0xD9,5,{0x00,0x00,0x00,0x00,0x00}},

    {0xE7,1,{0x00}},

    {0xF0,5,{0x55,0xAA,0x52,0x08,0x03}},

    {0xB0,2,{0x00,0x00}},
    {0xB1,2,{0x00,0x00}},
    {0xB2,5,{0x05,0x00,0x00,0x00,0x00}},
    {0xB6,5,{0x05,0x00,0x00,0x00,0x00}},
    {0xB7,5,{0x05,0x00,0x00,0x00,0x00}},
    {0xBA,5,{0x57,0x00,0x00,0x00,0x00}},
    {0xBB,5,{0x57,0x00,0x00,0x00,0x00}},
    {0xC0,4,{0x00,0x00,0x00,0x00}},
    {0xC1,4,{0x00,0x00,0x00,0x00}},
    {0xC4,1,{0x60}},
    {0xC5,1,{0x40}},

    {0xF0,5,{0x55,0xAA,0x52,0x08,0x05}},

    {0xBD,5,{0x03,0x01,0x03,0x03,0x03}},
    {0xB0,2,{0x17,0x06}},
    {0xB1,2,{0x17,0x06}},
    {0xB2,2,{0x17,0x06}},
    {0xB3,2,{0x17,0x06}},
    {0xB4,2,{0x17,0x06}},
    {0xB5,2,{0x17,0x06}},
    {0xB8,1,{0x00}},
    {0xB9,1,{0x00}},
    {0xBA,1,{0x00}},
    {0xBB,1,{0x02}},
    {0xBC,1,{0x00}},

    {0xC0,1,{0x07}},
    {0xC4,1,{0x80}},
    {0xC5,1,{0xA4}},
    {0xC8,2,{0x05,0x30}},
    {0xC9,2,{0x01,0x31}},
    {0xCC,3,{0x00,0x00,0x3C}},
    {0xCD,3,{0x00,0x00,0x3C}},

    {0xD1,5,{0x00,0x05,0x09,0x07,0x10}},
    {0xD2,5,{0x00,0x05,0x0E,0x07,0x10}},

    {0xE5,1,{0x06}},
    {0xE6,1,{0x06}},
    {0xE7,1,{0x06}},
    {0xE8,1,{0x06}},
    {0xE9,1,{0x06}},
    {0xEA,1,{0x06}},

    {0xED,1,{0x30}},

    {0x6F,1,{0x11}},

    {0xF3,1,{0x01}},

    {0x11, 1,   {0x00}},
    {REGFLAG_DELAY, 120, {}},

    {0x29, 1, {0x00}},
    //{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
    //{0xEF,2,{0x07,0xFF}},
    //{0xEE,4,{0x87,0x78,0x02,0x40}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
#endif

	{REGFLAG_END_OF_TABLE, 0x00, {}}

};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    //Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
    // Display off sequence
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},

    // Sleep Mode On
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {
        unsigned cmd;
        cmd = table[i].cmd;
        switch (cmd) {
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DSI;
   	params->width  = FRAME_WIDTH;
   	params->height = FRAME_HEIGHT;
	

    params->dsi.mode = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE; //BURST_VDO_MODE; // SYNC_PULSE_VDO_MODE
   

    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM = LCM_FOUR_LANE;

	params->dbi.te_mode                                 = LCM_DBI_TE_MODE_VSYNC_ONLY;                                                              
    params->dbi.te_edge_polarity                = LCM_POLARITY_RISING;
	
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	//video mode timing
	
	params->dsi.packet_size = 128;
	params->dsi.word_count = FRAME_WIDTH*3;
	
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 
#if 1
	// horizontal blanking = 35
    params->dsi.vertical_frontporch					= 12;
    params->dsi.vertical_sync_active				= 2;
    params->dsi.vertical_backporch					= 20;

	// vertical blanking = 320
	params->dsi.horizontal_frontporch				= 160;
    params->dsi.horizontal_sync_active				= 19;
    params->dsi.horizontal_backporch				= 140;
#else
	// horizontal blanking = 35
    params->dsi.vertical_frontporch					= 160;
    params->dsi.vertical_sync_active				= 19;
    params->dsi.vertical_backporch					= 140;

	// vertical blanking = 320
	params->dsi.horizontal_frontporch				= 12;
    params->dsi.horizontal_sync_active				= 2;
    params->dsi.horizontal_backporch				= 20;
#endif
	/* D-PTHY
	params->dsi.HS_PRPR								= 30;
	params->dsi.HS_ZERO								= 60;
	params->dsi.HS_TRAIL							= 30;

	params->dsi.CLK_HS_PRPR							= 20;
	params->dsi.CLK_ZERO							= 150;
	params->dsi.CLK_TRAIL							= 40;
	*/

    // Video mode setting		
    // params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    // params->dsi.pll_select=1;
    params->dsi.PLL_CLOCK = 156;  // 240;//LCM_DSI_6589_PLL_CLOCK_253_5;//LCM_DSI_6589_PLL_CLOCK_240_5;//LCM_DSI_6589_PLL_CLOCK_227_5;//this value must be in MTK suggested table 227_5
   	params->dsi.ssc_disable = 1;  // disable ssc
   	params->dsi.ssc_range = 4;
    //params->dsi.cont_clock = 1;  // clcok always hs mode

	params->dsi.lcm_int_te_monitor = FALSE;
    params->dsi.lcm_int_te_period = 1;
	  
	if(params->dsi.lcm_int_te_monitor)
	    params->dsi.vertical_frontporch *= 2;
	
	params->dsi.lcm_ext_te_monitor = FALSE;
	params->dsi.noncont_clock = TRUE;
	params->dsi.noncont_clock_period = 2;
	params->dsi.CLK_HS_POST=22;
}

#define ICN6202_CHIP_ADDR		(0X2D << 0) 

#if 0
static kal_uint32 icn6202_i2c_write(kal_uint8 slave_addr, kal_uint8 offset, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[I2C_FIFO_SIZE], len;
	struct mt_i2c_t i2c;

	i2c.id = I2C2;
	i2c.addr = slave_addr;
	i2c.mode = ST_MODE;
	i2c.speed = 100;

	write_data[0]= offset;
	write_data[1] = value;
	len = 2;

	ret_code = i2c_write(&i2c, write_data, len);

	// LCD_DEBUG("[DISPD] I2C ret:%d\n", ret_code);

	return ret_code;		
}
#endif

static void icn6202_register_init()
{
	unsigned int data_array[1];

#if 0

	
	data_array[0] = 0xC17A2300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00202300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x58212300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x24222300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xA0232300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x13242300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x8C252300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00262300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x0C272300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x02282300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x14292300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x80342300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xA0362300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xA0B52300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xFF5C2300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x10132300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x92562300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x326B2300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3A692300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x20B62300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x20512300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x10092300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x47102300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x412A2300;dsi_set_cmdq(data_array, 1, 1);


#else


	data_array[0] = 0xC17A2300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00202300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x58212300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x24222300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xA0232300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x13242300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x8C252300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00262300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x0C272300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x02282300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x14292300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x80342300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xA0362300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xA0B52300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xFF5C2300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x10132300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x92562300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x316B2300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x26692300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x20B62300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x20512300;dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x10092300;dsi_set_cmdq(data_array, 1, 1);

#endif

	// LVDS setting	
	/*
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x20, 0x00);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x21, 0x58);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x22, 0x24);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x23, 0x64);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x24, 0x14);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x25, 0xC8);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x26, 0x00);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x27, 0x0E);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x28, 0x03);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x29, 0x12);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x34, 0x80);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x36, 0x64);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0xB5, 0xA0);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x5C, 0xFF);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x13, 0x10);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x56, 0x90);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x6B, 0x21);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x69, 0x1D);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0xB6, 0x20);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x51, 0x20);
	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x09, 0x10);


	icn6202_i2c_write(ICN6202_CHIP_ADDR, 0x7B, 0x4E);
	*/
}

static void lcm_init(void)
{

	/**
	* utils->dsi_set_cmdq 		= DSI_set_cmdq_wrapper_DSI0;
	* utils->dsi_set_cmdq_V2	= DSI_set_cmdq_V2_Wrapper_DSI0;
	*/
	// push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

	// write registers by I2C
	dsi_lcm_set_gpio_mode(GPIO_LCM_BOOST_EN_PIN, GPIO_MODE_GPIO);
    dsi_lcm_set_gpio_dir(GPIO_LCM_BOOST_EN_PIN, GPIO_DIR_OUT);
    dsi_lcm_set_gpio_out(GPIO_LCM_BOOST_EN_PIN, GPIO_OUT_ONE);

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);

    SET_RESET_PIN(1);
    MDELAY(20);

	icn6202_register_init();
    //LCD_DEBUG("[DISPD] icn6202 lcm_init done\n");
	
}

static void lcm_suspend(void)
{
#if 1
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(20); // 1ms

    SET_RESET_PIN(1);
    MDELAY(120);
#endif
    // push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

    dsi_lcm_set_gpio_out(GPIO_LCM_BOOST_EN_PIN, GPIO_OUT_ZERO);
    MDELAY(5);
    printk(KERN_INFO "[DISPD] ICN6202 lcm_suspend\n");
}

static void lcm_resume(void)
{

    lcm_init();
    //push_table(lcm_initialization_setting_tm, sizeof(lcm_initialization_setting_tm) / sizeof(struct LCM_setting_table), 1);
    printk(KERN_INFO "[DISPD] ICN6202 lcm_resume\n");
}

static unsigned int lcm_compare_id(void)
{
	LCD_DEBUG("[DISPD] ICN6202 check id:always true");

	return 1;

#if 0
    unsigned int id = 0, id1=0, id2 = 0;
    unsigned char buffer[5];
    unsigned int data_array[16];

    SET_RESET_PIN(1);   //NOTE:should reset LCM firstly
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(50);

 //*************Enable CMD2 Page1  *******************//
    data_array[0]=0x00063902;
    data_array[1]=0x52AA55F0;
    data_array[2]=0x00000108;
    dsi_set_cmdq(data_array, 3, 1);
    MDELAY(10);

    data_array[0] = 0x00023700;// read id return two byte,version and id
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);

    read_reg_v2(0xC5, buffer, 2);
    id1 = buffer[0]; //we only need ID
    id2 = buffer[1];  //we test buffer 1
    id  = (id1 << 8) | id2;

#ifdef BUILD_LK
    printf("[mly]%s,  nt35521 id = 0x%08x\n", __func__, id);
#else
    printk("[mly]%s,  nt35521 id = 0x%08x\n", __func__, id);
#endif

    return (0x5521 == id)?1 : 0;
#endif

}

LCM_DRIVER icn6202_lcm_drv =
{
    .name           = "icn6202_1024_600_dsi_lvds_bridge",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id     = lcm_compare_id,
};
