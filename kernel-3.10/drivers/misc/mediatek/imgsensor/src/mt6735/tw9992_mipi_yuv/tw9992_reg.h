#ifndef TW9992Reg_H
#define TW9992Reg_H


/* v4l2_subdev_core_ops.ioctl  ioctl_cmd macro */
#define RK29_CAM_SUBDEV_ACTIVATE            0x00
#define RK29_CAM_SUBDEV_DEACTIVATE          0x01
#define RK29_CAM_SUBDEV_IOREQUEST			0x02

/* register addr */

#define     TW9992_REG_NUM                              0x100   // Total number of registers

#define     TW9992_REG_VIN_SOURCE                       0x00   // video input source selection; R/W
#define     TW9992_REG_ANALOG_CHAN_CTRL                 0x01   // Analog channel controls; R/W
#define     TW9992_REG_OPERATION_MOD_CTRL               0x02   // Operation mode controls; R/W
#define     TW9992_REG_MISC_CTRL                        0x03   // Miscellaneous controls; R/W

#define     TW9992_REG_AUTOSWITCH_MASK                  0x04   // Autoswitch mask; R/W

#define     TW9992_REG_SOFTWARE_RESET                   0x05   // Software reset; R/W

#define     TW9992_REG_COLOR_KILLER_THRESHOLD_CTRL      0x06   // Software reset; R/W
#define     TW9992_REG_LUM_PROCESS_CTRL1                0x07   // Luminance processing control #1; R/W
#define     TW9992_REG_LUM_PROCESS_CTRL2                0x08   // Luminance processing control #2; R/W

#define     TW9992_REG_BRIGHTNESS                       0x09   // Luminance control; R/W
#define     TW9992_REG_SATURATION                       0x0A   // Chroma saturation; R/W
#define     TW9992_REG_HUE_CTRL                         0x0B   // Chroma hue control; R/W
#define     TW9992_REG_CONTRAST                         0x0C   // Luminance contrast; R/W

#define     TW9992_REG_OUT_DATA_RATE_SEL                0x0D   // Output and data rates select; R/W

#define     TW9992_REG_LUM_PROCESS_CTRL3                0x0E   // Luminance processing control #3; R/W

#define     TW9992_REG_CONFIG_SHARED_PINS               0x0F   // Configuration shared pins; R/W
// 0x10 reserved
#define     TW9992_REG_ACTIVE_CROPPING_START_MSB        0x11   // Active video cropping start MSB; R/W
#define     TW9992_REG_ACTIVE_CROPPING_START_LSB        0x12   // Active video cropping start LSB; R/W
#define     TW9992_REG_ACTIVE_CROPPING_STOP_MSB         0x13   // Active video cropping stop MSB; R/W
#define     TW9992_REG_ACTIVE_CROPPING_STOP_LSB         0x14   // Active video cropping stop LSB; R/W

#define     TW9992_REG_HSYNC_START_REG         			0x16   // Horizontal Sync (HSYNC) Start Register

#define     TW9992_REG_VBLK_START                      0x18   // Vertial blanking start; R/W
#define     TW9992_REG_VBLK_STOP                       0x19   // Vertial blanking stop; R/W

#define     TW9992_REG_CHROMINANCE_CTRL1       			0x1A   // Chrominance Control #1 Register
#define     TW9992_REG_CHROMINANCE_CTRL2       			0x1B   // Chrominance Control #2 Register

#define     TW9992_REG_VIDEO_STANDARD                   0x28   // Video standard; R/W
#define 	TVP5150_REV_SELECT          				0x30   /* revision select (TVP5150AM1 only) */

#define TVP5150_MSB_DEV_ID          					0x80 /* MSB of device ID */
#define TVP5150_LSB_DEV_ID          					0x81 /* LSB of device ID */
#define TVP5150_ROM_MAJOR_VER       					0x82 /* ROM major version */
#define TVP5150_ROM_MINOR_VER       					0x83 /* ROM minor version */
#define TVP5150_VERT_LN_COUNT_MSB   					0x84 /* Vertical line count MSB */
#define TVP5150_VERT_LN_COUNT_LSB   					0x85 /* Vertical line count LSB */
#define TVP5150_INT_STATUS_REG_B    					0x86 /* Interrupt status register B */
#define TVP5150_INT_ACTIVE_REG_B    					0x87 /* Interrupt active register B */
#define TVP5150_STATUS_REG_1        					0x88 /* Status register #1 */
#define TVP5150_STATUS_REG_2        					0x89 /* Status register #2 */
#define TVP5150_STATUS_REG_3        					0x8a /* Status register #3 */
#define TVP5150_STATUS_REG_4        					0x8b /* Status register #4 */
#define TVP5150_STATUS_REG_5        					0x8c /* Status register #5 */

// TW9992_REG_CHROMA_CTRL
#define     CHROMA_CTRL_COLORMODE_MASK                  0x0f    // Mask of colour mode setting
#define     CHROMA_CTRL_COLORMODE_NTSCM                 0x02    // Colour mode of (M)NTSC ITU-R.601
#define     CHROMA_CTRL_COLORMODE_PAL                   0x04    // Colour mode of (B.G.H.I.N)PAL ITU-R.601
#define     CHROMA_CTRL_COLORMODE_PALM                  0x06    // Colour mode of (M)PAL ITU-R.601
#define     CHROMA_CTRL_COLORMODE_PALN                  0x08    // Colour mode of (Combination-N) ITU-R.601
#define     CHROMA_CTRL_COLORMODE_NTSC443               0x0a    // Colour mode of NTSC 4.43 ITU-R.601
#define     CHROMA_CTRL_COLORMODE_SECAM                 0x0c    // Colour mode of SECAM ITU-R.601


#define     OFTDEL_CTRL_OUTPUT_FORMAT_MASK              0x07    // Mask of output format
#define     OFTDEL_CTRL_OUTPUT_FORMAT_YUV422            0x00    // Output YUV422 8bits
#define     OFTDEL_CTRL_OUTPUT_FORMAT_CCIR656           0x07    // Output YUV CCIR-656 8bits


// TW9992_REG_STATUS
#define     STATUS_FIELD_FREQ_60HZ                      0x20    // Detected field frequency; LOW = 50Hz, HIGH = 60Hz
#define     STATUS_COLOUR_SIGNAL_DETECTED               0x01    // Colour signal in accordance with selected mode hase been detected



#endif
