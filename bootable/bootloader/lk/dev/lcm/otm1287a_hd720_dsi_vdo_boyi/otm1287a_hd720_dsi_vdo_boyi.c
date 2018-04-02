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

#define FRAME_WIDTH                    (720)
#define FRAME_HEIGHT                   (1280)
#define LCM_ID_OTM1287                 (0x1287)
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


    {0x00,1,{0x00}},
    {0xff,3,{0x12,0x87,0x01}}, //EXTC=1
    {0x00,1,{0x80}},         //Orise mode enable
    {0xff,2,{0x12,0x87}},

    {0x00,1,{0x92}},
    {0xff,2,{0x30,0x02}},  //MIPI 4 Lane

//--------- panel setting --------------------//
    {0x00,1,{0x80}},            //TCON Setting
    {0xc0,9,{0x00,0x66,0x00,0x10,0x10,0x00,0x64,0x10,0x10}},

    {0x00,1,{0x90}},             //Panel Timing Setting
    {0xc0,6,{0x00,0x5c,0x00,0x01,0x00,0x04}},

    {0x00,1,{0xb3}},             //Interval Scan Frame: 0 frame, column inversion
    {0xc0,2,{0x00,0x55}},

    {0x00,1,{0x81}},            //frame rate:60Hz
    {0xc1,1,{0x77}},//55

//--------- power setting --------------------//
    {0x00,1,{0xa0}},             //dcdc setting
    {0xc4,14,{0x05,0x10,0x04,0x02,0x05,0x15,0x11,0x05,0x10,0x07,0x02,0x05,0x15,0x11}},

    {0x00,1,{0xb0}},             //clamp voltage setting
    {0xc4,2,{0x00,0x00}},

    {0x00,1,{0x91}},            //VGH=16V, VGL=-12V, pump ratio:VGH=8x, VGL=-5x
    {0xc5,2,{0x69,0xd2}},

    {0x00,1,{0x00}},             //GVDD=5.008V, NGVDD=-5.008V
    {0xd8,2,{0xc7,0xc7}},

    {0x00,1,{0x00}},            //VCOM=-1.63V
    {0xd9,1,{0x56}},

    {0x00,1,{0xb3}},             //VDD_18V=1.7V, LVDSVDD=1.6V
    {0xc5,1,{0x84}},

    {0x00,1,{0xbb}},             //LVD voltage level setting
    {0xc5,1,{0x8a}},

    {0x00,1,{0x80}},
    {0xC4,1,{0x01}},

    {0x00,1,{0x88}},
    {0xC4,1,{0x01}},

    {0x00,1,{0xc2}},
    {0xf5,1,{0xc0}},

    {0x00,1,{0x82}}, //chopper
    {0xC4,1,{0x0a}},

    {0x00,1,{0xc6}},  //debounce
    {0xb0,1,{0x03}},

//--------- control setting --------------------//
    {0x00,1,{0x00}},             //ID1
    {0xd0,1,{0x40}},

    {0x00,1,{0x00}},             //ID2, ID3
    {0xd1,2,{0x00,0x00}},

//--------- power on setting --------------------//
    {0x00,1,{0xb2}},             //VGLO1
    {0xf5,2,{0x00,0x00}},

    {0x00,1,{0xb6}},             //VGLO2
    {0xf5,2,{0x00,0x00}},

    {0x00,1,{0x94}},            //VCL pump dis
    {0xf5,2,{0x00,0x00}},

    {0x00,1,{0xd2}},             //VCL reg. en
    {0xf5,2,{0x06,0x15}},

    {0x00,1,{0xb4}},             //VGLO1/2 Pull low setting
    {0xc5,1,{0xcc}},       //d[7] vglo1 d[6] vglo2 => 0: pull vss, 1: pull vgl

//-------- for Power IC ---------------------------------
    {0x00,1,{0x90}},            //Mode-3
    {0xf5,4,{0x02,0x11,0x02,0x15}},

    {0x00,1,{0x90}},             //2xVPNL, 1.5*=00, 2*=50, 3*=a0
    {0xc5,1,{0x50}},

    {0x00,1,{0x94}},            //Frequency
    {0xc5,1,{0x66}},

//--------- panel timing state control --------------------//
    {0x00,1,{0x80}},            //panel timing state control
    {0xcb,11,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

    {0x00,1,{0x90}},             //panel timing state control
    {0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

    {0x00,1,{0xa0}},             //panel timing state control
    {0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

    {0x00,1,{0xb0}},            //panel timing state control
    {0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

    {0x00,1,{0xc0}},             //panel timing state control
    {0xcb,15,{0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x05,0x00,0x00,0x00,0x00}},

    {0x00,1,{0xd0}},             //panel timing state control
    {0xcb,15,{0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05}},

    {0x00,1,{0xe0}},            //panel timing state control
    {0xcb,14,{0x05,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00}},

    {0x00,1,{0xf0}},             //panel timing state control
    {0xcb,11,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},

//--------- panel pad mapping control --------------------//
    {0x00,1,{0x80}},           //panel pad mapping control
    {0xcc,15,{0x29,0x2a,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x06,0x00,0x08,0x00,0x00,0x00,0x00}},

    {0x00,1,{0x90}},             //panel pad mapping control
    {0xcc,15,{0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x29,0x2a,0x09,0x0b,0x0d,0x0f,0x11,0x13}},

    {0x00,1,{0xa0}},           //panel pad mapping control
    {0xcc,14,{0x05,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00}},

           {0x00,1,{0xb0}},             //panel pad mapping control
    {0xcc,15,{0x29,0x2a,0x13,0x11,0x0f,0x0d,0x0b,0x09,0x01,0x00,0x07,0x00,0x00,0x00,0x00}},

    {0x00,1,{0xc0}},             //panel pad mapping control
    {0xcc,15,{0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x29,0x2a,0x14,0x12,0x10,0x0e,0x0c,0x0a}},

    {0x00,1,{0xd0}},             //panel pad mapping control
    {0xcc,14,{0x02,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00}},

//--------- panel timing setting --------------------//
    {0x00,1,{0x80}},             //panel VST setting
    {0xce,12,{0x89,0x05,0x00,0x88,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

    {0x00,1,{0x90}},             //panel VEND setting
    {0xce,14,{0x54,0xfc,0x00,0x54,0xfd,0x00,0x55,0x00,0x00,0x55,0x01,0x00,0x00,0x00}},

    {0x00,1,{0xa0}},             //panel CLKA1/2 setting
    {0xce,14,{0x58,0x07,0x05,0x08,0x00,0x00,0x00,0x58,0x06,0x05,0x09,0x00,0x00,0x00}},

    {0x00,1,{0xb0}},            //panel CLKA3/4 setting
    {0xce,14,{0x58,0x05,0x05,0x0A,0x00,0x00,0x00,0x58,0x04,0x05,0x0B,0x00,0x00,0x00}},

    {0x00,1,{0xc0}},             //panel CLKb1/2 setting
    {0xce,14,{0x58,0x03,0x05,0x0C,0x00,0x00,0x00,0x58,0x02,0x05,0x0D,0x00,0x00,0x00}},

    {0x00,1,{0xd0}},            //panel CLKb3/4 setting
    {0xce,14,{0x58,0x01,0x05,0x0E,0x00,0x00,0x00,0x58,0x00,0x05,0x0F,0x00,0x00,0x00}},

    {0x00,1,{0x80}},            //panel CLKc1/2 setting
    {0xcf,14,{0x50,0x00,0x05,0x10,0x00,0x00,0x00,0x50,0x01,0x05,0x11,0x00,0x00,0x00}},

    {0x00,1,{0x90}},             //panel CLKc3/4 setting
    {0xcf,14,{0x50,0x02,0x05,0x12,0x00,0x00,0x00,0x50,0x03,0x05,0x13,0x00,0x00,0x00}},

    {0x00,1,{0xa0}},             //panel CLKd1/2 setting
    {0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

    {0x00,1,{0xb0}},             //panel CLKd3/4 setting
    {0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

    {0x00,1,{0xc0}},             //panel ECLK setting
    {0xcf,11,{0x3D,0x3D,0x20,0x20,0x00,0x00,0x01,0x01,0x20,0x00,0x00}},

    {0x00,1,{0xb5}},
    {0xc5,6,{0x0b,0x95,0xff,0x0b,0x95,0xff}},

///////////////   gamma  2.2     ///////////////
    {0x00,1,{0x00}},                                                                                   ////
    {0xE1,20,{0x05,0x4b,0x56,0x64,0x72,0x7f,0x7f,0xa4,0x92,0xa8,0x5f,0x4c,0x62,0x44,0x44,0x39,0x2f,0x24,0x1c,0x07}},

    {0x00,1,{0x00}},
    {0xE2,20,{0x05,0x4b,0x56,0x64,0x72,0x7f,0x7f,0xa4,0x92,0xa8,0x5f,0x4c,0x62,0x44,0x44,0x39,0x2f,0x23,0x1c,0x07}},
////////////////////////////////////////////////


    {0x00,1,{0x92}}, //ESD add
    {0xb3,1,{0x02}},

    {0x00,1,{0x90}},
    {0xb6,1,{0xb6}},

    {0x00,1,{0xa0}},
    {0xc1,1,{0x02}},

////////   CE Setting   ///////////////////
/*
 {0x00, 1 , {0xA0}},
 {0xD6, 12 ,{0x01,0x0D,0x01,0x0D,0x01,0x66,0x01,0xC0,0x01,0xC0,0x01,0xC0}},

 {0x00, 1 , {0xB0}},
 {0xD6, 12 ,{0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0}},

 {0x00, 1 , {0xC0}},
 {0xD6, 12 ,{0x26,0x11,0x09,0x80,0x11,0x80,0x80,0x11,0x80,0x80,0x11,0x80}},

 {0x00, 1 , {0xD0}},
 {0xD6,  6 ,{0x80,0x11,0x80,0x80,0x11,0x80}},

 {0x00, 1 , {0xE0}},
 {0xD6, 12 ,{0x22,0x11,0x40,0x40,0x11,0x40,0x40,0x11,0x40,0x40,0x11,0x40}},

 {0x00, 1 , {0xF0}},
 {0xD6,  6 ,{0x40,0x11,0x40,0x40,0x11,0x40}},
////////   CE Setting   ///////////////////
*/
    {0x00,1,{0x00}},            //Orise mode disable
    {0xff,3,{0xff,0xff,0xff}},

////////   CE Setting   ///////////////////
 //CE on High
// {0x81,  1 ,{0x83}},    //CE - High
 //CE on Middle
// {0x81,  1 ,{0x81}},    //CE - Middle
 //CE on Low
// {0x81,  1 ,{0x80}},    //CE - Low
 //CE off
// {0x81,  1 ,{0x00}},    //CE  off
////////   CE Setting   ///////////////////
    //{0x35, 1, {0x01}},
 // Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

 // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
 // Display off sequence
    {0x28, 1, {0x00}},

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

    params->dsi.vertical_sync_active    = 0x04;// 3    2
    params->dsi.vertical_backporch     = 0x14;// 20   1
    params->dsi.vertical_frontporch     = 0x16; // 1  12
    params->dsi.vertical_active_line    = FRAME_HEIGHT;

//params->dsi.horizontal_sync_active    = 0x12;// 50  2
//params->dsi.horizontal_backporch    = 0x5f;
//params->dsi.horizontal_frontporch    = 0x5f;
    params->dsi.horizontal_sync_active    = 8;// 50  2
    params->dsi.horizontal_backporch    = 44;
    params->dsi.horizontal_frontporch    = 46;
    params->dsi.horizontal_active_pixel    = FRAME_WIDTH;

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
    params->dsi.PLL_CLOCK = 220;  // 240->220 evade 2G RFI
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
    return (LCM_ID_OTM1287 == id)?1:0;

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


LCM_DRIVER otm1287a_hd720_dsi_vdo_boyi_lcm_drv =
{
    .name           = "otm1287a_hd720_dsi_vdo_boyi",
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
