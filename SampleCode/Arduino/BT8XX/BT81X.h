/* * * * * * * * * * * * * * * * * * * * * * * * * *
 * FTDI EVE Firmware for BT81X  				           *
 * Copyright (c) 2019  							               *
 * K.Watanabe,Crescent 							               *
 * Released under the MIT license 				         *
 * http://opensource.org/licenses/mit-license.php  *
 * 19/02/16 v1.0 Initial Release                   *
 * 19/02/27 v1.1 Fix initialization stability      *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Eve IO Port Def	*/
#define irqPin 4        // Interrupt 
#define pwrPin 3        // PD_N
#define csPin  7

/* Chip Identification  */
#define CHIP_ID_REG   0xC0000
#define CHIP_ID_FT810 0x10
#define CHIP_ID_FT811 0x11
#define CHIP_ID_FT812 0x12
#define CHIP_ID_FT813 0x13
#define CHIP_ID_BT815 0x15
#define CHIP_ID_BT816 0x16
#define CHIP_ID_RESET 0x7C

/* Eve Mode Def	*/
//#define EVE_RESOLUTION_WXGA 	//1280x720 //Not Supported, but may work.
//#define EVE_RESOLUTION_XGA 	//1024x768 //Not Supported, but may work.
//#define EVE_RESOLUTION_WSVGA 	//1024x600 //Not Supported, but may work.
//#define EVE_RESOLUTION_SVGA 	//800x600
#define EVE_RESOLUTION_WVGA 	//800x480
//#define EVE_RESOLUTION_VGA 		//640x480
//#define EVE_RESOLUTION_WQVGA 	//480x272
//#define EVE_RESOLUTION_QVGA 	//320x240

// BT81X Memory R/W Bit
#define MEM_WRITE  			  0x80     // BT81X Host Memory Write
#define MEM_READ  			  0x00     // BT81X Host Memory Read

// BT81X Memory Map Addresses
#define RAM_G                 0x000000UL
#define RAM_FONT              0x1E0000UL
#define RAM_FONT_ADDR         0x2FFFFCUL
#define RAM_DL                0x300000UL
#define RAM_REG               0x302000UL
#define RAM_CMD               0x308000UL
#define RAM_ERR_REPORT 	      0x309800UL
#define FLASH_MEMORY 	      0x800000UL

// BT81X State
#define ACTIVE  			   0x00 // Initializes BT81X
#define STANDBY 			   0x41 // Place BT81X in Standby (clk running)
#define SLEEP 			       0x42 // Place BT81X in Sleep (clk off)
#define PWRDOWN 		       0x50 // Place BT81X in Power Down (core off)
#define CLKEXT  			   0x44 // Select external clock source
#define CLKINT  			   0x48 // Select external clock source
#define CLK48M  			   0x62 // Select 48MHz PLL
#define CLK36M  			   0x61 // Select 36MHz PLL
#define CORERST 			   0x68	// Reset core - all registers default

// BT81X Graphics Engine Registers
#define REG_TAG               (0x00007CUL+RAM_REG)
#define REG_TAG_Y             (0x000078UL+RAM_REG)
#define REG_TAG_X             (0x000074UL+RAM_REG)
#define REG_PCLK              (0x000070UL+RAM_REG)
#define REG_PCLK_POL          (0x00006CUL+RAM_REG)
#define REG_CSPREAD           (0x000068UL+RAM_REG)
#define REG_SWIZZLE           (0x000064UL+RAM_REG)
#define REG_DITHER            (0x000060UL+RAM_REG)
#define REG_OUTBITS           (0x00005CUL+RAM_REG)
#define REG_ROTATE            (0x000058UL+RAM_REG)
#define REG_DLSWAP            (0x000054UL+RAM_REG)
#define REG_VSYNC1            (0x000050UL+RAM_REG)
#define REG_VSYNC0            (0x00004CUL+RAM_REG)
#define REG_VSIZE             (0x000048UL+RAM_REG)
#define REG_VOFFSET           (0x000044UL+RAM_REG)
#define REG_VCYCLE            (0x000040UL+RAM_REG)
#define REG_HSYNC1            (0x00003CUL+RAM_REG)
#define REG_HSYNC0            (0x000038UL+RAM_REG)
#define REG_HSIZE             (0x000034UL+RAM_REG)
#define REG_HOFFSET           (0x000030UL+RAM_REG)
#define REG_HCYCLE            (0x00002CUL+RAM_REG)

// BT81X Audio Engine Registers
#define REG_PLAY              (0x00008CUL+RAM_REG)
#define REG_SOUND             (0x000088UL+RAM_REG)
#define REG_VOL_SOUND         (0x000084UL+RAM_REG)
#define REG_VOL_PB            (0x000080UL+RAM_REG)
#define REG_PLAYBACK_PLAY     (0x0000CCUL+RAM_REG)
#define REG_PLAYBACK_LOOP     (0x0000C8UL+RAM_REG)
#define REG_PLAYBACK_FORMAT   (0x0000C4UL+RAM_REG)
#define REG_PLAYBACK_FREQ     (0x0000C0UL+RAM_REG)
#define REG_PLAYBACK_READPTR  (0x0000BCUL+RAM_REG)
#define REG_PLAYBACK_LENGTH   (0x0000B8UL+RAM_REG)
#define REG_PLAYBACK_START    (0x0000B4UL+RAM_REG)
#define REG_PLAYBACK_PAUSE    (0x0005ECUL+RAM_REG)

// BT81X Flash Registers
#define REG_FLASH_STATUS      (0x0005F0UL+RAM_REG)
#define REG_FLASH_SIZE        (0x007024UL+RAM_REG)

// BT81X Touch Screen Engine Registers
#define REG_TOUCH_CONFIG      (0x000168UL+RAM_REG)
#define REG_TOUCH_TRANSFORM_F (0x000164UL+RAM_REG)
#define REG_TOUCH_TRANSFORM_E (0x000160UL+RAM_REG)
#define REG_TOUCH_TRANSFORM_D (0x00015CUL+RAM_REG)
#define REG_TOUCH_TRANSFORM_C (0x000158UL+RAM_REG)
#define REG_TOUCH_TRANSFORM_B (0x000154UL+RAM_REG)
#define REG_TOUCH_TRANSFORM_A (0x000150UL+RAM_REG)
#define REG_TOUCH_TAG         (0x00012CUL+RAM_REG)
#define REG_TOUCH_TAG_XY      (0x000128UL+RAM_REG)
#define REG_TOUCH_SCREEN_XY   (0x000124UL+RAM_REG)
#define REG_TOUCH_DIRECT_Z1Z2 (0x000190UL+RAM_REG)
#define REG_TOUCH_DIRECT_XY   (0x00018CUL+RAM_REG)
#define REG_TOUCH_RZ          (0x000120UL+RAM_REG)
#define REG_TOUCH_RAW_XY      (0x000118UL+RAM_REG)
#define REG_TOUCH_RZTHRESH    (0x000118UL+RAM_REG)
#define REG_TOUCH_OVERSAMPLE  (0x000114UL+RAM_REG)
#define REG_TOUCH_SETTLE      (0x000110UL+RAM_REG)
#define REG_TOUCH_CHARGE      (0x00010CUL+RAM_REG)
#define REG_TOUCH_ADC_MODE    (0x000108UL+RAM_REG)
#define REG_TOUCH_MODE        (0x000104UL+RAM_REG)

// BT81X Capacitive Touch Engine Registers
#define REG_CTOUCH_MODE        (0x000104UL+RAM_REG)
#define REG_CTOUCH_EXTEND      (0x000108UL+RAM_REG)
#define REG_CTOUCH_TOUCH_XY    (0x000124UL+RAM_REG)
#define REG_CTOUCH_TOUCH1_XY   (0x00011CUL+RAM_REG)
#define REG_CTOUCH_TOUCH2_XY   (0x00018CUL+RAM_REG)
#define REG_CTOUCH_TOUCH3_XY   (0x000190UL+RAM_REG)
#define REG_CTOUCH_TOUCH4_X    (0x00016CUL+RAM_REG)
#define REG_CTOUCH_TOUCH4_Y    (0x000120UL+RAM_REG)
#define REG_CTOUCH_RAW_XY      (0x00011CUL+RAM_REG)
#define REG_CTOUCH_TAG         (0x00012CUL+RAM_REG)
#define REG_CTOUCH_TAG1        (0x000134UL+RAM_REG)
#define REG_CTOUCH_TAG2        (0x00013CUL+RAM_REG)
#define REG_CTOUCH_TAG3        (0x000144UL+RAM_REG)
#define REG_CTOUCH_TAG4        (0x00014CUL+RAM_REG)
#define REG_CTOUCH_SCREEN_XY   (0x000128UL+RAM_REG)
#define REG_CTOUCH_TAG1_XY     (0x000130UL+RAM_REG)
#define REG_CTOUCH_TAG2_XY     (0x000138UL+RAM_REG)
#define REG_CTOUCH_TAG3_XY     (0x000140UL+RAM_REG)
#define REG_CTOUCH_TAG4_XY     (0x000148UL+RAM_REG)

// BT81X Co-processor Engine Registers
#define REG_CMD_DL            (0x000100UL+RAM_REG)
#define REG_CMD_WRITE         (0x0000FCUL+RAM_REG)
#define REG_CMD_READ          (0x0000F8UL+RAM_REG)
#define REG_CMDB_SPACE        (0x000574UL+RAM_REG)
#define REG_CMDB_WRITE        (0x000578UL+RAM_REG)

// BT81X Miscellaneous Registers
#define REG_CPURESET          (0x000020UL+RAM_REG)
#define REG_PWM_DUTY          (0x0000D4UL+RAM_REG)
#define REG_PWM_HZ            (0x0000D0UL+RAM_REG)
#define REG_INT_MASK          (0x0000B0UL+RAM_REG)
#define REG_INT_EN            (0x0000ACUL+RAM_REG)
#define REG_INT_FLAGS         (0x0000A8UL+RAM_REG)
#define REG_GPIO_DIR          (0x000090UL+RAM_REG)
#define REG_GPIO              (0x000094UL+RAM_REG)
#define REG_GPIOX_DIR         (0x000098UL+RAM_REG)
#define REG_GPIOX             (0x00009CUL+RAM_REG)
#define REG_FREQUENCY         (0x00000CUL+RAM_REG)
#define REG_CLOCK             (0x000008UL+RAM_REG)
#define REG_FRAMES            (0x000004UL+RAM_REG)
#define REG_ID                (0x000000UL+RAM_REG)
#define REG_SPI_WIDTH         (0x000180UL+RAM_REG)

// BT81X Special Registers
#define REG_TRACKER           (0x007000UL+RAM_REG)
#define REG_TRACKER1          (0x007004UL+RAM_REG)
#define REG_TRACKER2          (0x007008UL+RAM_REG)
#define REG_TRACKER3          (0x00700CUL+RAM_REG)
#define REG_TRACKER4          (0x007010UL+RAM_REG)
#define REG_MEDIAFIFO_READ    (0x007014UL+RAM_G)
#define REG_MEDIAFIFO_WRITE   (0x007018UL+RAM_G)
#define REG_PLAY_CONTROL      (0x00714EUL+RAM_REG)
#define REG_COPRO_PATCH_PTR   (0x007162UL+RAM_REG)

// BT81X Graphics Engine Commands
#define CMD_DLSTART          0xFFFFFF00UL
#define CMD_INTERRUPT        0xFFFFFF02UL
#define CMD_COLDSTART        0xFFFFFF32UL
#define CMD_SWAP             0xFFFFFF01UL
#define CMD_APPEND           0xFFFFFF1EUL
#define CMD_REGREAD          0xFFFFFF19UL
#define CMD_MEMWRITE         0xFFFFFF1AUL
#define CMD_INFLATE          0xFFFFFF22UL
#define CMD_INFLATE2         0xFFFFFF50UL
#define CMD_LOADIMAGE        0xFFFFFF24UL
#define CMD_MEDIAFIFO        0xFFFFFF39UL
#define CMD_PLAYVIDEO        0xFFFFFF3AUL
#define CMD_VIDEOSTART       0xFFFFFF40UL
#define CMD_VIDEOFRAME       0xFFFFFF41UL
#define CMD_MEMCRC    		 0xFFFFFF18UL
#define CMD_MEMZERO          0xFFFFFF1CUL
#define CMD_MEMSET           0xFFFFFF1BUL
#define CMD_MEMCPY           0xFFFFFF1DUL
#define CMD_BUTTON           0xFFFFFF0DUL
#define CMD_CLOCK            0xFFFFFF14UL
#define CMD_FGCOLOR          0xFFFFFF0AUL
#define CMD_BGCOLOR          0xFFFFFF09UL
#define CMD_GRADCOLOR        0xFFFFFF34UL
#define CMD_GAUGE            0xFFFFFF13UL
#define CMD_GRADIENT         0xFFFFFF0BUL
#define CMD_GRADIENTA        0xFFFFFF57UL
#define CMD_KEYS             0xFFFFFF0EUL
#define CMD_PROGRESS         0xFFFFFF0FUL
#define CMD_SCROLLBAR        0xFFFFFF11UL
#define CMD_SLIDER           0xFFFFFF10UL
#define CMD_DIAL             0xFFFFFF2DUL
#define CMD_TOGGLE           0xFFFFFF12UL
#define CMD_FILLWIDTH        0xFFFFFF58UL
#define CMD_TEXT             0xFFFFFF0CUL
#define CMD_SETBASE          0xFFFFFF38UL
#define CMD_NUMBER           0xFFFFFF2EUL
#define CMD_LOADIDENTITY     0xFFFFFF26UL
#define CMD_SETMATRIX        0xFFFFFF2AUL
#define CMD_GETMATRIX        0xFFFFFF33UL
#define CMD_GETPTR           0xFFFFFF23UL
#define CMD_GETPROPS         0xFFFFFF25UL
#define CMD_SCALE            0xFFFFFF28UL
#define CMD_ROTATE           0xFFFFFF29UL
#define CMD_ROTATEAROUND     0xFFFFFF51UL
#define CMD_TRANSLATE        0xFFFFFF27UL
#define CMD_CALIBRATE        0xFFFFFF15UL
#define CMD_SETROTATE        0xFFFFFF36UL
#define CMD_SPINNER          0xFFFFFF16UL
#define CMD_SCREENSAVER      0xFFFFFF2FUL
#define CMD_SKETCH           0xFFFFFF30UL
#define CMD_STOP             0xFFFFFF17UL
#define CMD_SETFONT          0xFFFFFF2BUL
#define CMD_SETFONT2         0xFFFFFF3BUL
#define CMD_SETSCRATCH       0xFFFFFF3CUL
#define CMD_ROMFONT    		 0xFFFFFF3FUL
#define CMD_RESETFONTS       0xFFFFFF52UL
#define CMD_TRACK            0xFFFFFF2CUL
#define CMD_SNAPSHOT         0xFFFFFF1FUL
#define CMD_SNAPSHOT2        0xFFFFFF37UL
#define CMD_SETBITMAP        0xFFFFFF43UL
#define CMD_LOGO             0xFFFFFF31UL
#define CMD_FLASHERASE       0xFFFFFF44UL
#define CMD_FLASHWRITE       0xFFFFFF45UL
#define CMD_FLASHREAD        0xFFFFFF46UL
#define CMD_APPENDF          0xFFFFFF59UL
#define CMD_FLASHUPDATE      0xFFFFFF47UL
#define CMD_FLASHDETACH      0xFFFFFF48UL
#define CMD_FLASHATTACH      0xFFFFFF49UL
#define CMD_FLASHFAST        0xFFFFFF4AUL
#define CMD_FLASHSPIDESEL    0xFFFFFF4BUL
#define CMD_FLASHSPITX       0xFFFFFF4CUL
#define CMD_FLASHSPIRX       0xFFFFFF4DUL
#define CMD_CLEARCACHE       0xFFFFFF4FUL
#define CMD_FLASHSOURCE      0xFFFFFF4EUL
#define CMD_VIDEOSTARTF      0xFFFFFF5FUL
#define CMD_ANIMSTART        0xFFFFFF53UL
#define CMD_ANIMSTOP         0xFFFFFF54UL
#define CMD_ANIMXY           0xFFFFFF55UL
#define CMD_ANIMDRAW         0xFFFFFF56UL
#define CMD_ANIMFRAME        0xFFFFFF5AUL
#define CMD_SYNC             0xFFFFFF42UL
#define CMD_BITMAP_TRANSFORM 0xFFFFFF21UL

// BT81X Display List Commands
#define DL_ALPHA_FUNC       0x09000000UL
#define DL_BITMAP_HANDLE    0x05000000UL
#define DL_BITMAP_LAYOUT    0x07000000UL
#define DL_BITMAP_SIZE      0x08000000UL
#define DL_BITMAP_SOURCE    0x01000000UL
#define DL_BITMAP_TFORM_A   0x15000000UL
#define DL_BITMAP_TFORM_B   0x16000000UL
#define DL_BITMAP_TFORM_C   0x17000000UL
#define DL_BITMAP_TFORM_D   0x18000000UL
#define DL_BITMAP_TFORM_E   0x19000000UL
#define DL_BITMAP_TFORM_F   0x1A000000UL
#define DL_BLEND_FUNC       0x0B000000UL
#define DL_BEGIN            0x1F000000UL
#define DL_CALL             0x1D000000UL
#define DL_CLEAR            0x26000000UL
#define DL_CELL             0x06000000UL
#define DL_CLEAR_RGB        0x02000000UL
#define DL_CLEAR_STENCIL    0x11000000UL
#define DL_CLEAR_TAG        0x12000000UL
#define DL_COLOR_A          0x0F000000UL
#define DL_COLOR_MASK       0x20000000UL
#define DL_COLOR_RGB        0x04000000UL
#define DL_DISPLAY          0x00000000UL
#define DL_END              0x21000000UL
#define DL_JUMP             0x1E000000UL

// BT81X Display List Commands Options
#define BITMAPS              1UL
#define CLR_COL              0x4
#define CLR_STN              0x2
#define CLR_TAG              0x1
#define DECR                 4UL
#define DECR_WRAP            7UL
#define DLSWAP_DONE          0UL
#define DLSWAP_FRAME         2UL
#define DLSWAP_LINE          1UL
#define DST_ALPHA            3UL
#define EDGE_STRIP_A         7UL
#define EDGE_STRIP_B         8UL
#define EDGE_STRIP_L         6UL
#define EDGE_STRIP_R         5UL
#define EQUAL                5UL
#define GEQUAL               4UL
#define GREATER              3UL
#define INCR                 3UL
#define INCR_WRAP            6UL
#define INT_CMDEMPTY         32UL
#define INT_CMDFLAG          64UL
#define INT_CONVCOMPLETE     128UL
#define INT_PLAYBACK         16UL
#define INT_SOUND            8UL
#define INT_SWAP             1UL
#define INT_TAG              4UL
#define INT_TOUCH            2UL
#define INVERT               5UL
#define KEEP                 1UL
#define L1                   1UL
#define L4                   2UL
#define L8                   3UL
#define LEQUAL               2UL
#define LESS                 1UL
#define LINEAR_SAMPLES       0UL
#define LINES                3UL
#define LINE_STRIP           4UL
#define NEAREST              0UL
#define NEVER                0UL
#define NOTEQUAL             6UL
#define ONE                  1UL
#define ONE_MINUS_DST_ALPHA  5UL
#define ONE_MINUS_SRC_ALPHA  4UL
#define OPT_CENTER           0x6000UL
#define OPT_CENTERX          0x0200UL
#define OPT_CENTERY          0x0400UL
#define OPT_FLAT             0x0100UL
#define OPT_MONO             1UL
#define OPT_NOBACK           0x1000UL
#define OPT_NODL             2UL
#define OPT_NOHANDS          0xC168UL
#define OPT_NOHM             0x4000UL
#define OPT_NOPOINTER        0x4000UL
#define OPT_NOSECS           0x8000UL
#define OPT_NOTICKS          0x2000UL
#define OPT_RIGHTX           0x0800UL
#define OPT_SIGNED           0x0100UL
#define PALETTED             8UL
#define POINTS               2UL
#define PLAYCOLOR            0x00A0A080
#define FTPOINTS             2UL
#define RECTS                9UL
#define REPEAT               1UL
#define REPLACE              2UL
#define RGB332               4UL
#define RGB565               7UL
#define SRC_ALPHA            2UL
#define TEXT8X8              9UL
#define TEXTVGA              10UL
#define TOUCHMODE_CONTINUOUS 3UL
#define TOUCHMODE_FRAME      2UL
#define TOUCHMODE_OFF        0UL
#define TOUCHMODE_ONESHOT    1UL
#define ULAW_SAMPLES         1UL
#define ZERO                 0UL

// BT81X Macros
#define RGB(r, g, b)         ((((r) << 16) | (g) << 8) | (b))
#define VERTEX2F(x,y) ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))
#define VERTEX2II(x,y,handle,cell) ((2UL<<30)|(((x)&511UL)<<21)|(((y)&511UL)<<12)|(((handle)&31UL)<<7)|(((cell)&127UL)<<0))
#define BITMAP_SOURCE(addr) ((1UL<<24)|(((addr)&1048575UL)<<0))
#define CLEAR_COLOR_RGB(red,green,blue) ((2UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define TAG(s) ((3UL<<24)|(((s)&255UL)<<0))
#define COLOR_RGB(red,green,blue) ((4UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define BITMAP_HANDLE(handle) ((5UL<<24)|(((handle)&31UL)<<0))
#define CELL(cell) ((6UL<<24)|(((cell)&127UL)<<0))
#define BITMAP_LAYOUT(format,linestride,height) ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0))
#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0))
#define ALPHA_FUNC(func,ref) ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
#define STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define BLEND_FUNC(src,dst) ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0))
#define STENCIL_OP(sfail,spass) ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0))
#define POINT_SIZE(size) ((13UL<<24)|(((size)&8191UL)<<0))
#define LINE_WIDTH(width) ((14UL<<24)|(((width)&4095UL)<<0))
#define CLEAR_COLOR_A(alpha) ((15UL<<24)|(((alpha)&255UL)<<0))
#define COLOR_A(alpha) ((16UL<<24)|(((alpha)&255UL)<<0))
#define CLEAR_STENCIL(s) ((17UL<<24)|(((s)&255UL)<<0))
#define CLEAR_TAG(s) ((18UL<<24)|(((s)&255UL)<<0))
#define STENCIL_MASK(mask) ((19UL<<24)|(((mask)&255UL)<<0))
#define TAG_MASK(mask) ((20UL<<24)|(((mask)&1UL)<<0))
#define BITMAP_TRANSFORM_A(a) ((21UL<<24)|(((a)&131071UL)<<0))
#define BITMAP_TRANSFORM_B(b) ((22UL<<24)|(((b)&131071UL)<<0))
#define BITMAP_TRANSFORM_C(c) ((23UL<<24)|(((c)&16777215UL)<<0))
#define BITMAP_TRANSFORM_D(d) ((24UL<<24)|(((d)&131071UL)<<0))
#define BITMAP_TRANSFORM_E(e) ((25UL<<24)|(((e)&131071UL)<<0))
#define BITMAP_TRANSFORM_F(f) ((26UL<<24)|(((f)&16777215UL)<<0))
#define SCISSOR_XY(x,y) ((27UL<<24)|(((x)&511UL)<<9)|(((y)&511UL)<<0))
#define SCISSOR_SIZE(width,height) ((28UL<<24)|(((width)&1023UL)<<10)|(((height)&1023UL)<<0))
#define CALL(dest) ((29UL<<24)|(((dest)&65535UL)<<0))
#define JUMP(dest) ((30UL<<24)|(((dest)&65535UL)<<0))
#define BEGIN(prim) ((31UL<<24)|(((prim)&15UL)<<0))
#define COLOR_MASK(r,g,b,a) ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0))
#define CLEAR(c,s,t) ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0))
#define END() ((33UL<<24))
#define SAVE_CONTEXT() ((34UL<<24))
#define RESTORE_CONTEXT() ((35UL<<24))
#define RETURN() ((36UL<<24))
#define MACRO(m) ((37UL<<24)|(((m)&1UL)<<0))
#define DISPLAY() ((0UL<<24))







// Resolution Definitions
#ifdef EVE_RESOLUTION_WXGA
	#define USR_HCYCLE		1280
	#define USR_HOFFSET		368
	#define USR_HSYNC0		72
	#define	USR_HSYNC1		152
	#define USR_VCYCLE		750
	#define USR_VOFFSET		30
	#define USR_VSYNC0		3
	#define USR_VSYNC1		27
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		1280
	#define USR_VSIZE		720
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif

#ifdef EVE_RESOLUTION_XGA
	#define USR_HCYCLE		1280
	#define USR_HOFFSET		256
	#define USR_HSYNC0		40
	#define	USR_HSYNC1		128
	#define USR_VCYCLE		804
	#define USR_VOFFSET		36
	#define USR_VSYNC0		3
	#define USR_VSYNC1		33
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		1024
	#define USR_VSIZE		768
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif

#ifdef EVE_RESOLUTION_WSVGA
	#define USR_HCYCLE		1280
	#define USR_HOFFSET		256
	#define USR_HSYNC0		40
	#define	USR_HSYNC1		128
	#define USR_VCYCLE		674
	#define USR_VOFFSET		74
	#define USR_VSYNC0		13
	#define USR_VSYNC1		61
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		1024
	#define USR_VSIZE		600
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif

#ifdef EVE_RESOLUTION_SVGA
	#define USR_HCYCLE		1048
	#define USR_HOFFSET		248
	#define USR_HSYNC0		40
	#define	USR_HSYNC1		152
	#define USR_VCYCLE		640
	#define USR_VOFFSET		40
	#define USR_VSYNC0		13
	#define USR_VSYNC1		27
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		800
	#define USR_VSIZE		600
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif

#ifdef EVE_RESOLUTION_WVGA
	#define USR_HCYCLE		928
	#define USR_HOFFSET		88
	#define USR_HSYNC0		0
	#define	USR_HSYNC1		48
	#define USR_VCYCLE		525
	#define USR_VOFFSET		32
	#define USR_VSYNC0		0
	#define USR_VSYNC1		3
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		800
	#define USR_VSIZE		480
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif

#ifdef EVE_RESOLUTION_VGA
	#define USR_HCYCLE		800
	#define USR_HOFFSET		160
	#define USR_HSYNC0		16
	#define	USR_HSYNC1		112
	#define USR_VCYCLE		525
	#define USR_VOFFSET		45
	#define USR_VSYNC0		10
	#define USR_VSYNC1		35
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		640
	#define USR_VSIZE		480
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif

#ifdef EVE_RESOLUTION_WQVGA
	#define USR_HCYCLE		548
	#define USR_HOFFSET		43
	#define USR_HSYNC0		6
	#define	USR_HSYNC1		32
	#define USR_VCYCLE		292
	#define USR_VOFFSET		12
	#define USR_VSYNC0		3
	#define USR_VSYNC1		7
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		480
	#define USR_VSIZE		272
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif

#ifdef EVE_RESOLUTION_QVGA
	#define USR_HCYCLE		408
	#define USR_HOFFSET		68
	#define USR_HSYNC0		0
	#define	USR_HSYNC1		10
	#define USR_VCYCLE		262
	#define USR_VOFFSET		18
	#define USR_VSYNC0		0
	#define USR_VSYNC1		2
	#define USR_SWIZZLE		0
	#define USR_PCLK_POL	1
	#define USR_HSIZE		320
	#define USR_VSIZE		240
	#define USR_CSPREAD		0
	#define USR_DITHER		0
#endif
