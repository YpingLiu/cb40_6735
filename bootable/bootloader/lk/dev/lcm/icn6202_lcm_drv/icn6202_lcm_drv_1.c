#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
        #include <platform/mt_i2c.h>
        #include <platform/mt_gpio.h>
        #include <platform/upmu_common.h>
#elif defined(BUILD_UBOOT)
        #include <asm/arch/mt_gpio.h>
#else
        #include <mach/mt_gpio.h>
        #include <mach/mt_pm_ldo.h>
        #include <mach/upmu_common.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                                                                                  (1024) 
#define FRAME_HEIGHT                                                                                 (1024)

#define REGFLAG_DELAY                                                                     0XFE
#define REGFLAG_END_OF_TABLE                                                              0xFFF   // END OF REGISTERS MARKER


#define LCM_DSI_CMD_MODE                                                                        0

#ifndef TRUE
    #define   TRUE     1
#endif

#ifndef FALSE
    #define   FALSE    0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)                                                                    (lcm_util.set_reset_pin((v)))

#define UDELAY(n)                                                                                         (lcm_util.udelay(n))
#define MDELAY(n)                                                                                         (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
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

                // enable tearing-free
                params->dbi.te_mode                                 = LCM_DBI_TE_MODE_VSYNC_ONLY;
                params->dbi.te_edge_polarity                = LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
                params->dsi.mode   = CMD_MODE;
#else
                params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

                // DSI
                /* Command mode setting */
                params->dsi.LANE_NUM                                = LCM_FOUR_LANE;
                //The following defined the fomat for data coming from LCD engine.
                params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
                params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
                params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
                params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

                // Highly depends on LCD driver capability.
                // Not support in MT6573
                params->dsi.packet_size=256;

                // Video mode setting
                params->dsi.intermediat_buffer_num = 2;

                params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

                params->dsi.vertical_sync_active                                = 19;
                params->dsi.vertical_backporch                                        = 140;
                params->dsi.vertical_frontporch                                        = 160;
                params->dsi.vertical_active_line                                = FRAME_HEIGHT;

                params->dsi.horizontal_sync_active                                = 19;
                params->dsi.horizontal_backporch                                = 140;
                params->dsi.horizontal_frontporch                                = 160;
                params->dsi.horizontal_active_pixel                                = FRAME_WIDTH;

#if 0
                // Bit rate calculation
                params->dsi.pll_div1=29;                // fref=26MHz, fvco=fref*(div1+1)        (div1=0~63, fvco=500MHZ~1GHz)
                params->dsi.pll_div2=1;                 // div2=0~15: fout=fvo/(2*div2)
#else
                                // Bit rate calculation
                //1 Every lane speed
                //params->dsi.pll_select=1;
                //params->dsi.PLL_CLOCK  = LCM_DSI_6589_PLL_CLOCK_377;
                params->dsi.PLL_CLOCK=160;
//                params->dsi.pll_div1=1;                // div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
//                params->dsi.pll_div2=0;                // div2=0,1,2,3;div1_real=1,2,4,4
#if (LCM_DSI_CMD_MODE)
                params->dsi.fbk_div =9;
#else
//                params->dsi.fbk_div =16;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)
#endif
#endif
                /* ESD or noise interference recovery For video mode LCM only. */ // Send TE packet to LCM in a period of n frames and check the response.
                params->dsi.lcm_int_te_monitor = FALSE;
                params->dsi.lcm_int_te_period = 1; // Unit : frames

                // Need longer FP for more opportunity to do int. TE monitor applicably.
                if(params->dsi.lcm_int_te_monitor)
                        params->dsi.vertical_frontporch *= 2;

                // Monitor external TE (or named VSYNC) from LCM once per 2 sec. (LCM VSYNC must be wired to baseband TE pin.)
                params->dsi.lcm_ext_te_monitor = FALSE;
                // Non-continuous clock
                params->dsi.noncont_clock = TRUE;
                params->dsi.noncont_clock_period = 2; // Unit : frames
                params->dsi.CLK_HS_POST=22;
}

static void lcm_gpio_set(int num, int enable)
{
#ifdef BUILD_LK
printf("\n\n\n===================== %s %d: num:%d => %d =======================\n\n\n", __func__, __LINE__, num, enable);
#endif
        lcm_util.set_gpio_mode(num, GPIO_MODE_00);
        lcm_util.set_gpio_dir(num, GPIO_DIR_OUT);

        if (enable)
                lcm_util.set_gpio_out(num, GPIO_OUT_ONE);
        else
                lcm_util.set_gpio_out(num, GPIO_OUT_ZERO);
}

#ifdef BUILD_LK 

static U8 Write_register(struct mt_i2c_t *i2c, U8 reg, U8 data)
{
  U32 ret = 0,i=0,j=0;
  U32 len=0;
  unsigned char write_byte[2] = {0};
  printf("wirte reg = 0x%x, data 0x%x\n", reg, data);
  write_byte[0] = reg;
  write_byte[1] = data;
  len=2;
  ret = i2c_write(i2c, write_byte, 2);
  //delay
  ret = 0xFFFFF;
  while(ret--);
}

static void init_mini_convert_lvds(void)
{
  struct mt_i2c_t i2c;
  int read_byte;


//0x20 = 0x56
//0x21 = 0x00
//0x22 = 0x35
//0x23 = 0x64
//0x24 = 0x3A
//0x25 = 0x64
//0x26 = 0x00
//0x27 = 0x0D
//0x28 = 0x04
//0x29 = 0x11
//0x34 = 0x80
//0x36 = 0x64
//0xB5 = 0xA0
//0x5C = 0xFF
//0x56 = 0x92
//0x6B = 0x12
//0x69 = 0x26
//0xB6 = 0x20
//0x51 = 0x20
//0x09 = 0x10
//
  i2c.id = 2; 
  i2c.addr = 0x5a >> 1;
  i2c.mode = ST_MODE;
  i2c.speed = 100;
  i2c.delay_len = 20;
  //1st:write addree 00,1byte(0x0A)

  Write_register(&i2c, 0x20, 0x00);
  Write_register(&i2c, 0x21, 0x00);
  Write_register(&i2c, 0x22, 0x44);
  Write_register(&i2c, 0x23, 0x50);
  Write_register(&i2c, 0x24, 0x14);
  Write_register(&i2c, 0x25, 0x50);
  Write_register(&i2c, 0x26, 0x00);
  Write_register(&i2c, 0x27, 0x14);
  Write_register(&i2c, 0x28, 0x0a);
  Write_register(&i2c, 0x29, 0x14);

  Write_register(&i2c, 0x34, 0x80);   //buffer
  Write_register(&i2c, 0x36, 0x50);   //buffer
  Write_register(&i2c, 0xb5, 0xa0);
  Write_register(&i2c, 0x5c, 0xff);    //delay
  Write_register(&i2c, 0x13, 0x10);  // 8bit, 6bit del
//        Write_register(&i2c, 0x15, 0x63);  // 8bit, 6bit del
//        Write_register(&i2c, 0x16, 0x20);  // 8bit, 6bit del
  
  Write_register(&i2c, 0x56, 0x92);  //extern clk , 0x92 inter clk


  Write_register(&i2c, 0x6b, 0x31);  //  lvds clk
  Write_register(&i2c, 0x69, 0x26);  //lvds clk

   //Write_register(&i2c, 0x10, 0x47);
   //Write_register(&i2c, 0x2A, 0x41);

// fix write scren , Repair system initialization failed
  Write_register(&i2c, 0xb6, 0x20);  //lvds clk
//  Write_register(&i2c, 0x7c, 0x4d);
//  Write_register(&i2c, 0x7b, 0x59);
  Write_register(&i2c, 0x51, 0x20); 

  Write_register(&i2c, 0x09, 0x10);  // disply on 

//        read_byte = 0xb6;
//   i2c_write(&i2c, &read_byte, 1);
//   i2c_read(&i2c, &read_byte, 1);
//   printf("felix read_byte = %x",read_byte);
}


#endif

static void lcm_init(void)
{
//lcd_vdd
        lcm_gpio_set(GPIO4, 1); //DSI_TE GPIO147
        MDELAY(20);

           lcm_gpio_set(GPIO147, 1); //DSI_TE GPIO147
      MDELAY(20);

//        lcm_gpio_set(GPIO69, 1);  //DISP_PWM0 gpio69
//        MDELAY(20);
#if 1
        SET_RESET_PIN(0);
        MDELAY(10);
        SET_RESET_PIN(1);
        MDELAY(20);
#endif

///============  init mipi convert lvds
#ifdef BUILD_LK
        lcm_gpio_set(GPIO9, 1); 
        init_mini_convert_lvds();
#endif
/*
//lcd stbyb
        lcm_gpio_set(GPIO77, 1); 
        MDELAY(40);
//lcd avdd vgh vgl
       // lcm_gpio_set(GPIO119, 1); 
        MDELAY(60);
*/
}

static void lcm_suspend(void)
{
//  stbyb
        lcm_gpio_set(GPIO147, 0); 
        MDELAY(80);
// avdd vgh vgl
        lcm_gpio_set(GPIO4, 0); 
        MDELAY(30);
// vdd
//        lcm_gpio_set(GPIO69, 0); 
//      MDELAY(15);

        SET_RESET_PIN(0);
        MDELAY(20);

        return;
}


static void lcm_resume(void)
{
        lcm_init();
}


static unsigned int lcm_compare_id(void)
{
#ifdef BUILD_LK
  U32 ret = 0,i=0,j=0;
  U32 len=0;
  unsigned char read_byte[2] = {0};
  struct mt_i2c_t i2c;

// enable icn6201 
  lcm_gpio_set(GPIO9, 1); 
  MDELAY(5);        
  i2c.id = 2; 
  i2c.addr = 0x5a >> 1;
  i2c.mode = ST_MODE;
  i2c.speed = 100;
  i2c.delay_len = 20;

  read_byte[0] = 0x00;
  len=1;
  ret = i2c_write(&i2c, read_byte, len);
  ret = i2c_read(&i2c, read_byte, len);
  printf("sunny j = %d, Read_byte = 0x%x\n", j, read_byte[0]);

// disable icn6201
  lcm_gpio_set(GPIO9, 0); 

  if(read_byte[0] == 0xc1)
        return 1;
  return 0;
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
