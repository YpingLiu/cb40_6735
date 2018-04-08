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
#define FRAME_HEIGHT                   (1280)
#define LCM_ID_OTA7290B                (0x7290)
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
    params->dsi.horizontal_backporch    = 40;  //old 0x30;     //0x44;
    params->dsi.horizontal_frontporch   = 40;  //old 0x30;    //46;
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
    return (LCM_ID_OTA7290B == id)?1:0;

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


LCM_DRIVER ota7290b_dsi_ivo_lcm_drv =
{
    .name           = "ota7290b_dsi_ivo",
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
