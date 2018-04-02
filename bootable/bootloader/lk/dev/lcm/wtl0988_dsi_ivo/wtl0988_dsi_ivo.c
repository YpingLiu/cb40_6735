#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"
#include <cust_gpio_usage.h>
#ifdef BUILD_LK
    #include <platform/mt_gpio.h>
    #include <string.h>
#elif defined(BUILD_UBOOT)
    #include <asm/arch/mt_gpio.h>
#else
    #include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                    (400)
#define FRAME_HEIGHT                   (1600)
#define LCM_ID_WTL0988                (0x0988)
#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

//---------------------------------------------------------------------------
//  gpio set @2016-1-15, by bruce
//---------------------------------------------------------------------------
#define GPIO_LCM_BOOST_EN_PIN 			GPIO19

#define REGFLAG_DELAY                   0xFC
#define REGFLAG_UDELAY                  0xFB

#define REGFLAG_END_OF_TABLE            0xFD   // END OF REGISTERS MARKER
#define REGFLAG_RESET_LOW               0xFE
#define REGFLAG_RESET_HIGH              0xFF


#ifndef BUILD_LK
//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
#endif
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)                                        (lcm_util.set_reset_pin((v)))

#define UDELAY(n)                                               (lcm_util.udelay(n))
#define MDELAY(n)                                               (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)              lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)         lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)            lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                           lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                       lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                            lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)                    lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define dsi_lcm_set_gpio_out(pin, out)                           lcm_util.set_gpio_out(pin, out)
#define dsi_lcm_set_gpio_mode(pin, mode)                         lcm_util.set_gpio_mode(pin, mode)
#define dsi_lcm_set_gpio_dir(pin, dir)                           lcm_util.set_gpio_dir(pin, dir)
#define dsi_lcm_set_gpio_pull_enable(pin, en)                    lcm_util.set_gpio_pull_enable(pin, en)

#define   LCM_DSI_CMD_MODE                                       0

static bool lcm_is_init = false;

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {

#if 1		
	{0xB0,1,{0x5A}},
	{0xB1,1,{0x00}},
	{0x89,1,{0x01}},
	{0x91,1,{0x17}},
	{0xB1,1,{0x03}},
	{0x2C,1,{0x28}},
	{0x00,1,{0xB7}},
	{0x01,1,{0x1B}},
	{0x02,1,{0x00}},
	{0x03,1,{0x00}},
	{0x04,1,{0x00}},
	{0x05,1,{0x00}},
	{0x06,1,{0x00}},
	{0x07,1,{0x00}},
	{0x08,1,{0x00}},
	{0x09,1,{0x00}},
	{0x0A,1,{0x01}},
	{0x0B,1,{0x01}},
	{0x0C,1,{0x00}},
	{0x0D,1,{0x00}},
	{0x0E,1,{0x24}},
	{0x0F,1,{0x1C}},
	{0x10,1,{0xC9}},
	{0x11,1,{0x60}},
	{0x12,1,{0x70}},
	{0x13,1,{0x01}},
	{0x14,1,{0xE7}},
	{0x15,1,{0xFF}},
	{0x16,1,{0x3D}},
	{0x17,1,{0x0E}},
	{0x18,1,{0x01}},
	{0x19,1,{0x00}},
	{0x1A,1,{0x00}},
	{0x1B,1,{0xFC}},
	{0x1C,1,{0x0B}},
	{0x1D,1,{0xA0}},
	{0x1E,1,{0x03}},
	{0x1F,1,{0x04}},
	{0x20,1,{0x0C}},
	{0x21,1,{0x00}},
	{0x22,1,{0x04}},
	{0x23,1,{0x81}},
	{0x24,1,{0x1F}},
	{0x25,1,{0x10}},
	{0x26,1,{0x9B}},
	{0x2D,1,{0x01}},
	{0x2E,1,{0x84}},
	{0x2F,1,{0x00}},
	{0x30,1,{0x02}},
	{0x31,1,{0x08}},
	{0x32,1,{0x01}},
	{0x33,1,{0x1C}},
	{0x34,1,{0x40}},
	{0x35,1,{0xFF}},
	{0x36,1,{0xFF}},
	{0x37,1,{0xFF}},
	{0x38,1,{0xFF}},
	{0x39,1,{0xFF}},
	{0x3A,1,{0x05}},
	{0x3B,1,{0x00}},
	{0x3C,1,{0x00}},
	{0x3D,1,{0x00}},
	{0x3E,1,{0x0F}},
	{0x3F,1,{0x8C}},
	{0x40,1,{0x2A}},
	{0x41,1,{0xFC}},
	{0x42,1,{0x01}},
	{0x43,1,{0x40}},
	{0x44,1,{0x05}},
	{0x45,1,{0xE8}},
	{0x46,1,{0x16}},
	{0x47,1,{0x00}},
	{0x48,1,{0x00}},
	{0x49,1,{0x88}},
	{0x4A,1,{0x08}},
	{0x4B,1,{0x05}},
	{0x4C,1,{0x03}},
	{0x4D,1,{0xD0}},
	{0x4E,1,{0x13}},
	{0x4F,1,{0xFF}},
	{0x50,1,{0x0A}},
	{0x51,1,{0x53}},
	{0x52,1,{0x26}},
	{0x53,1,{0x22}},
	{0x54,1,{0x09}},
	{0x55,1,{0x22}},
	{0x56,1,{0x00}},
	{0x57,1,{0x1C}},
	{0x58,1,{0x03}},
	{0x59,1,{0x3F}},
	{0x5A,1,{0x28}},
	{0x5B,1,{0x01}},
	{0x5C,1,{0xCC}},
	{0x5D,1,{0x21}},
	{0x5E,1,{0x84}},
	{0x5F,1,{0x84}},
	{0x60,1,{0x8E}},
	{0x61,1,{0x89}},
	{0x62,1,{0xF0}},
	{0x63,1,{0xB9}},
	{0x64,1,{0xC6}},
	{0x65,1,{0x96}},
	{0x66,1,{0x0A}},
	{0x67,1,{0x62}},
	{0x68,1,{0x90}},
	{0x69,1,{0x12}},
	{0x6A,1,{0x42}},
	{0x6B,1,{0x48}},
	{0x6C,1,{0xE8}},
	{0x6D,1,{0x98}},
	{0x6E,1,{0x08}},
	{0x6F,1,{0x9F}},
	{0x70,1,{0x6B}},
	{0x71,1,{0x6C}},
	{0x72,1,{0xA9}},
	{0x73,1,{0x20}},
	{0x74,1,{0x06}},
	{0x75,1,{0x29}},
	{0x76,1,{0x00}},
	{0x77,1,{0x00}},
	{0x78,1,{0x0F}},
	{0x79,1,{0xE0}},
	{0x7A,1,{0x01}},
	{0x7B,1,{0xFF}},
	{0x7C,1,{0xFF}},
	{0x7D,1,{0xFF}},
	{0x7E,1,{0x4F}},
	{0x7F,1,{0xFE}},
 	{0xB1,1,{0x02}},
	{0x00,1,{0xFF}},
	{0x01,1,{0x05}},
	{0x02,1,{0xC8}},
	{0x03,1,{0x00}},
	{0x04,1,{0x59}},
	{0x05,1,{0x42}},
	{0x06,1,{0x20}},
	{0x07,1,{0x0D}},
	{0x08,1,{0xC0}},
	{0x09,1,{0x00}},
	{0x0A,1,{0x00}},
	{0x0B,1,{0x14}},
	{0x0C,1,{0xE6}},
	{0x0D,1,{0x0D}},
	{0x0F,1,{0x00}},
	{0x10,1,{0xF9}},
	{0x11,1,{0x37}},
	{0x12,1,{0x73}},
	{0x13,1,{0xB3}},
	{0x14,1,{0x29}},
	{0x15,1,{0x80}},
	{0x16,1,{0x07}},
	{0x17,1,{0x8A}},
	{0x18,1,{0x8D}},
	{0x19,1,{0xBF}},
	{0x1A,1,{0x69}},
	{0x1B,1,{0x0E}},
	{0x1C,1,{0xFF}},
	{0x1D,1,{0xFF}},
	{0x1E,1,{0xFF}},
	{0x1F,1,{0xFF}},
	{0x20,1,{0xFF}},
	{0x21,1,{0xFF}},
	{0x22,1,{0xFF}},
	{0x23,1,{0xFF}},
	{0x24,1,{0xFF}},
	{0x25,1,{0xFF}},
	{0x26,1,{0xFF}},
	{0x27,1,{0x1F}},
	{0x28,1,{0xFF}},
	{0x29,1,{0xFF}},
	{0x2A,1,{0xFF}},
	{0x2B,1,{0xFF}},
	{0x2C,1,{0xFF}},
	{0x2D,1,{0x07}},
	{0x33,1,{0x06}},
	{0x35,1,{0x7E}},
	{0x36,1,{0x06}},
	{0x38,1,{0x7E}},
	{0x3A,1,{0x80}},
	{0x3B,1,{0x01}},
	{0x3C,1,{0x80}},
	{0x3D,1,{0x2C}},
	{0x3E,1,{0x00}},
	{0x3F,1,{0x90}},
	{0x40,1,{0x05}},
	{0x41,1,{0x00}},
	{0x42,1,{0xB2}},
	{0x43,1,{0x00}},
	{0x44,1,{0x40}},
	{0x45,1,{0x06}},
	{0x46,1,{0x00}},
	{0x47,1,{0x00}},
	{0x48,1,{0x9B}},
	{0x49,1,{0xD2}},
	{0x4A,1,{0x21}},
	{0x4B,1,{0x43}},
	{0x4C,1,{0x16}},
	{0x4D,1,{0xC0}},
	{0x4E,1,{0x0F}},
	{0x4F,1,{0x61}},
	{0x50,1,{0x78}},
	{0x51,1,{0x7A}},
	{0x52,1,{0x34}},
	{0x53,1,{0x99}},
	{0x54,1,{0xA2}},
	{0x55,1,{0x02}},
	{0x56,1,{0x14}},
	{0x57,1,{0xB8}},
	{0x58,1,{0xDC}},
	{0x59,1,{0xD4}},
	{0x5A,1,{0xEF}},
	{0x5B,1,{0xF7}},
	{0x5C,1,{0xFB}},
	{0x5D,1,{0xFD}},
	{0x5E,1,{0x7E}},
	{0x5F,1,{0xBF}},
	{0x60,1,{0xEF}},
	{0x61,1,{0xE6}},
	{0x62,1,{0x76}},
	{0x63,1,{0x73}},
	{0x64,1,{0xBB}},
	{0x65,1,{0xDD}},
	{0x66,1,{0x6E}},
	{0x67,1,{0x37}},
	{0x68,1,{0x8C}},
	{0x69,1,{0x08}},
	{0x6A,1,{0x31}},
	{0x6B,1,{0xB8}},
	{0x6C,1,{0xB8}},
	{0x6D,1,{0xB8}},
	{0x6E,1,{0xB8}},
	{0x6F,1,{0xB8}},
	{0x70,1,{0x5C}},
	{0x71,1,{0x2E}},
	{0x72,1,{0x17}},
	{0x73,1,{0x00}},
	{0x74,1,{0x00}},
	{0x75,1,{0x00}},
	{0x76,1,{0x00}},
	{0x77,1,{0x00}},
	{0x78,1,{0x00}},
	{0x79,1,{0x00}},
	{0x7A,1,{0xDC}},
	{0x7B,1,{0xDC}},
	{0x7C,1,{0xDC}},
	{0x7D,1,{0xDC}},
	{0x7E,1,{0xDC}},
	{0x7F,1,{0x6E}},
	{0x0B,1,{0x00}},

	{0xB1,1,{0x03}},
	{0x2C,1,{0x2C}},

	{0xB1,1,{0x00}},
	{0x89,1,{0x03}}
#endif

 //   {0x00,1,{0x00}},
 //   {REGFLAG_DELAY, 120, {}},

 // Display ON
 //   {0x29, 1, {0x00}},
 //   {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
 // Display off sequence
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

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

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode   = CMD_MODE;
#else
    params->dsi.mode   = SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;
#endif

// DSI
/* Command mode setting */
//1 Three lane or Four lane
    params->dsi.LANE_NUM    = LCM_FOUR_LANE;
//The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

// Video mode setting
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active    = 20;  //old 0x20; //0x04;// 3    2
    params->dsi.vertical_backporch      = 30;  //old 0x30;  //0x14;// 20   1
    params->dsi.vertical_frontporch     = 30;  //old 0x30; //0x16; // 1  12
    params->dsi.vertical_active_line    = FRAME_HEIGHT;

//params->dsi.horizontal_sync_active    = 0x12;// 50  2
//params->dsi.horizontal_backporch    = 0x5f;
//params->dsi.horizontal_frontporch    = 0x5f;
    params->dsi.horizontal_sync_active  = 30;  //old 0x30;  //8;// 50  2
    params->dsi.horizontal_backporch    = 50;  //old 0x30;     //0x44;
    params->dsi.horizontal_frontporch   = 50;  //old 0x30;    //46;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;

    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;

//params->dsi.lcm_esd_check_table[0].cmd          = 0x0A;
//params->dsi.lcm_esd_check_table[0].count        = 1;
//params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;

//params->dsi.lcm_esd_check_table[1].cmd          = 0x0E;
//params->dsi.lcm_esd_check_table[1].count        = 1;
//params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;

    params->dsi.lcm_esd_check_table[0].cmd          = 0xAC;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x00;
//params->dsi.LPX=8;

    params->dsi.noncont_clock    = 1;
    params->dsi.noncont_clock_period    = 1;
  // Bit rate calculation
    params->dsi.PLL_CLOCK = 150;  //old 250;   //220;  // 240->220 evade 2G RFI
  //1 Every lane speed
  //params->dsi.pll_div1=0;  // div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
  //params->dsi.pll_div2=0;  // div2=0,1,2,3;div1_real=1,2,4,4
  //params->dsi.fbk_div =9;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)
    params->physical_width= 68;
    params->physical_height= 121;
}

static void lcm_init(void)
{

    lcm_is_init = true;

    dsi_lcm_set_gpio_mode(GPIO_LCM_BOOST_EN_PIN, GPIO_MODE_GPIO);
    dsi_lcm_set_gpio_dir(GPIO_LCM_BOOST_EN_PIN, GPIO_DIR_OUT);
    dsi_lcm_set_gpio_out(GPIO_LCM_BOOST_EN_PIN, GPIO_OUT_ONE);

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);

    SET_RESET_PIN(1);
    MDELAY(20);
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}



static void lcm_suspend(void)
{
    unsigned int data_array[16];

// data_array[0]=0x00280508; // Display Off
// dsi_set_cmdq(data_array, 1, 1);

// data_array[0] = 0x00100508; // Sleep In
// dsi_set_cmdq(data_array, 1, 1);


    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(20); // 1ms

    SET_RESET_PIN(1);
    MDELAY(120);
    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

    dsi_lcm_set_gpio_out(GPIO_LCM_BOOST_EN_PIN, GPIO_OUT_ZERO);

    lcm_is_init = false;
}


static void lcm_resume(void)
{
// lcm_util.set_gpio_out(GPIO_LCD_ENN, GPIO_OUT_ONE);
// lcm_util.set_gpio_out(GPIO_LCD_ENP, GPIO_OUT_ONE);
       //if(!lcm_is_init)
    lcm_init();

    #ifdef BUILD_LK
    printf("[LK]---cmd---hx8394a_hd720_dsi_vdo_tianma----%s------\n",__func__);
    #else
    printk("[KERNEL]---cmd---hx8394a_hd720_dsi_vdo_tianma----%s------,+1\n",__func__);
    #endif
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

    unsigned char x0_MSB = ((x0>>8)&0xFF);
    unsigned char x0_LSB = (x0&0xFF);
    unsigned char x1_MSB = ((x1>>8)&0xFF);
    unsigned char x1_LSB = (x1&0xFF);
    unsigned char y0_MSB = ((y0>>8)&0xFF);
    unsigned char y0_LSB = (y0&0xFF);
    unsigned char y1_MSB = ((y1>>8)&0xFF);
    unsigned char y1_LSB = (y1&0xFF);

    unsigned int data_array[16];

    data_array[0]= 0x00053902;
    data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
    data_array[2]= (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0]= 0x00053902;
    data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
    data_array[2]= (y1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0]= 0x002c3909;
    dsi_set_cmdq(data_array, 1, 0);

}
#endif


static unsigned int lcm_compare_id(void)
{

    unsigned int id = 0;
    unsigned char buffer[5];
    unsigned int array[16];
    dsi_lcm_set_gpio_mode(GPIO_LCM_BOOST_EN_PIN, GPIO_MODE_GPIO);
    dsi_lcm_set_gpio_dir(GPIO_LCM_BOOST_EN_PIN, GPIO_DIR_OUT);
    dsi_lcm_set_gpio_out(GPIO_LCM_BOOST_EN_PIN, GPIO_OUT_ONE);

    SET_RESET_PIN(1);   //NOTE:should reset LCM firstly
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(50);

    array[0] = 0x00053700;  // read id return two byte,version and id
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0xa1, buffer, 5);
    id = ((buffer[2] << 8) | buffer[3]);    //we only need ID

#ifdef BUILD_LK
    printf("[mly]%s,  otm1287a id = 0x%08x\n", __func__, id);
#else
    printk("[mly]%s,  otm1287a id = 0x%08x\n", __func__, id);
#endif
    return (LCM_ID_WTL0988 == id)?1:0;

  //return 1;
}




#if 0
static unsigned int lcm_esd_check(void)
{
  #ifndef BUILD_LK
 char  buffer[3];
 int   array[4];

 if(lcm_esd_test)
 {
  lcm_esd_test = FALSE;
  return TRUE;
 }

 array[0] = 0x00013700;
 dsi_set_cmdq(array, 1, 1);

 read_reg_v2(0x0a, buffer, 1);
 if(buffer[0]==0x1c)
 {
  return FALSE;
 }
 else
 {
  return TRUE;
 }
#else
 return FALSE;
#endif

}

static unsigned int lcm_esd_recover(void)
{
 lcm_init();
 lcm_resume();

 return TRUE;
}
#endif


LCM_DRIVER wtl0988_dsi_ivo_lcm_drv =
{
    .name           = "wtl0988_dsi_ivo",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id     = lcm_compare_id,
 //.esd_check = lcm_esd_check,
 //.esd_recover = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
