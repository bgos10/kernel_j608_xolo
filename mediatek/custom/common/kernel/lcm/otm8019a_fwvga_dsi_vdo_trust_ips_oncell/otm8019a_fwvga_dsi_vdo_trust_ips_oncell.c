#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

// ---------------------------------------------------------------------------
//RGK add
// ---------------------------------------------------------------------------
#include <cust_adc.h>		// zhoulidong  add for lcm detect
#define MIN_VOLTAGE (700)	// +++rgk bug-id:null modify by yangjuwei 20140326
#define MAX_VOLTAGE (900)	// +++rgk bug-id:null modify by yangjuwei 20140326

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

#define LCM_ID	0x8019	//+++rgk bug-id:null modify by yangjuwei 20140326


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

unsigned static int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util ;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        	lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   		lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE	0

// zhoulidong  add for lcm detect ,read adc voltage
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

static LCM_setting_table_V3 lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

 
{0x15,0x00,1,{0x00}},
{0x39,0xFF,3,{0x80,0x19,0x01}},
{0x15,0x00,1,{0x80}},
{0x39,0xFF,2,{0x80,0x19}},
{0x15,0x00,1,{0xB4}},
{0x15,0xC0,1,{0x00}},  //
{0x15,0x00,1,{0x81}},
{0x15,0xC5,1,{0x66}},
{0x15,0x00,1,{0x82}},
{0x15,0xC5,1,{0xA3}},
{0x15,0x00,1,{0x90}},
{0x39,0xC5,2,{0x96,0x87}},
{0x15,0x00,1,{0x00}},
{0x39,0xD8,2,{0x62,0x62}},
{0x15,0x00,1,{0x00}},
{0x15,0xD9,1,{0x36}},
{0x15,0x00,1,{0x81}},
{0x15,0xC1,1,{0x66}},
{0x15,0x00,1,{0xA1}},
{0x15,0xC1,1,{0x08}},
{0x15,0x00,1,{0x89}},
{0x15,0xC4,1,{0x00}},
{0x15,0x00,1,{0xA2}},
{0x39,0xC0,3,{0x02,0x1B,0x02}},
{0x15,0x00,1,{0x81}},
{0x15,0xC4,1,{0x83}},
{0x15,0x00,1,{0x92}},
{0x15,0xC5,1,{0x01}},
{0x15,0x00,1,{0xB1}},
{0x15,0xC5,1,{0xA9}},
{0x15,0x00,1,{0xA0}},
{0x15,0xC1,1,{0xEA}},
{0x15,0x00,1,{0xA6}},
{0x15,0xC1,1,{0x01}},
{0x15,0x00,1,{0xC0}},
{0x15,0xC5,1,{0x00}},
{0x15,0x00,1,{0x8B}},
{0x15,0xB0,1,{0x40}},
{0x15,0x00,1,{0xB2}},
{0x39,0xF5,4,{0x15,0x00,0x15,0x00}},
{0x15,0x00,1,{0xB6}},
{0x15,0xF5,1,{0x06}},
{0x15,0x00,1,{0x93}},
{0x15,0xC5,1,{0x03}},
{0x15,0x00,1,{0x92}},
{0x15,0xB3,1,{0x45}},
{0x15,0x00,1,{0x90}},
{0x15,0xB3,1,{0x02}},
{0x15,0x00,1,{0x80}},
{0x15,0xC4,1,{0x30}},
{0x15,0x00,1,{0x8A}},
{0x15,0xC4,1,{0x40}},
{0x15,0x00,1,{0x90}},
{0x15,0xB3,1,{0x02}},
{0x15,0x00,1,{0x92}},
{0x15,0xB3,1,{0x45}},
{0x15,0x00,1,{0x80}},
{0x39,0xC0,9,{0x00,0x58,0x00,0x15,0x15,0x00,0x58,0x15,0x15}},
{0x15,0x00,1,{0x90}},
{0x39,0xC0,6,{0x00,0x56,0x00,0x00,0x00,0x03}},
{0x15,0x00,1,{0xA6}},
{0x39,0xC1,3,{0x01,0x00,0x00}},
{0x15,0x00,1,{0x80}},
{0x39,0xCE,12,{0x8B,0x03,0x00,0x8A,0x03,0x00,0x89,0x03,0x00,0x88,0x03,0x00}},
{0x15,0x00,1,{0xA0}},
{0x39,0xCE,14,{0x38,0x07,0x03,0x54,0x00,0x00,0x00,0x38,0x06,0x03,0x55,0x00,0x00,0x00}},
{0x15,0x00,1,{0xB0}},
{0x39,0xCE,14,{0x38,0x05,0x03,0x56,0x00,0x00,0x00,0x38,0x04,0x03,0x57,0x00,0x00,0x00}},
{0x15,0x00,1,{0xC0}},
{0x39,0xCE,14,{0x38,0x03,0x03,0x58,0x00,0x00,0x00,0x38,0x02,0x03,0x59,0x00,0x00,0x00}},
{0x15,0x00,1,{0xD0}},
{0x39,0xCE,14,{0x38,0x01,0x03,0x5A,0x00,0x00,0x00,0x38,0x00,0x03,0x5B,0x00,0x00,0x00}},
{0x15,0x00,1,{0xC0}},
{0x39,0xCF,10,{0x02,0x02,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00}},
{0x15,0x00,1,{0x80}},
{0x39,0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0x90}},
{0x39,0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0xA0}},
{0x15,0xCB,1,{0x00}},
{0x15,0x00,1,{0xA5}},
{0x39,0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0xB0}},
{0x39,0xCB,6,{0x00,0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0xC0}},
{0x39,0xCB,15,{0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0xD0}},
{0x15,0xCB,1,{0x00}},
{0x15,0x00,1,{0xd5}},
{0x39,0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01}},
{0x15,0x00,1,{0xE0}},
{0x39,0xCB,6,{0x01,0x01,0x01,0x01,0x01,0x01}},
{0x15,0x00,1,{0x80}},
{0x39,0xCC,10,{0x26,0x25,0x21,0x22,0x0C,0x0A,0x10,0x0E,0x02,0x04}},
{0x15,0x00,1,{0x90}},
{0x39,0xCC,6,{0x00,0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0x9A}},
{0x39,0xCC,5,{0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0xA0}},
{0x39,0xCC,11,{0x00,0x03,0x01,0x0d,0x0f,0x09,0x0b,0x22,0x21,0x25,0x26}},
{0x15,0x00,1,{0xB0}},
{0x39,0xCC,10,{0x25,0x26,0x21,0x22,0x10,0x0a,0x0c,0x0e,0x04,0x02}},
{0x15,0x00,1,{0xC0}},
{0x39,0xCC,6,{0x00,0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0xCA}},
{0x39,0xCC,5,{0x00,0x00,0x00,0x00,0x00}},
{0x15,0x00,1,{0xD0}},
{0x39,0xCC,11,{0x00,0x01,0x03,0x0d,0x0b,0x09,0x0f,0x22,0x21,0x26,0x25}},
{0x15,0x00,1,{0x90}},
{0x39,0xC5,4,{0x4E,0x87,0x01,0x03}},
{0x15,0x00,1,{0x82}},
{0x15,0xC5,1,{0xB0}},     
{0x15,0x00,1,{0x80}},
{0x39,0xC1,2,{0x03,0x33}},
{0x15,0x00,1,{0x90}},
{0x39,0xC0,6,{0x00,0x15,0x00,0x00,0x00,0x03}},
{0x15,0x00,1,{0x00}}, 
{0x39,0xE1,20,{0x00,0x0a,0x13,0x1d,0x2c,0x38,0x38,0x64,0x55,0x70,0x92,0x79,0x86,0x5d,0x55,0x4b,0x35,0x25,0x1c,0x04}}, 
{0x15,0x00,1,{0x00}}, 
{0x39,0xE2,20,{0x00,0x09,0x14,0x1e,0x2c,0x37,0x38,0x65,0x56,0x70,0x92,0x79,0x86,0x5d,0x55,0x4b,0x35,0x25,0x1c,0x04}}, 
{0x15,0x00,1,{0x00}},
{0x15,0xFF,3,{0xFF,0xFF,0xFF}},
{0x05,0x11,0,{}}, 
//DELAYMS(120}},
{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
{0x05,0x29,0,{}}, 
//DELAYMS(10}},
{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},

};


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
	params->dsi.LANE_NUM				= LCM_TWO_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Video mode setting		
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
	params->dsi.vertical_sync_active				= 6;// 3    2
	params->dsi.vertical_backporch					= 14;// 20   1
	params->dsi.vertical_frontporch					= 20; // 1  12
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 10;// 50  2
	params->dsi.horizontal_backporch				= 90;
	params->dsi.horizontal_frontporch				= 90;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	//params->dsi.LPX=8; 

	// Bit rate calculation
	//1 Every lane speed
	//params->dsi.pll_select=1;
	//params->dsi.PLL_CLOCK  = LCM_DSI_6589_PLL_CLOCK_377;
	params->dsi.PLL_CLOCK=245;
	params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
	params->dsi.pll_div2=0;		// div2=0,1,2,3;div1_real=1,2,4,4	
#if (LCM_DSI_CMD_MODE)
	params->dsi.fbk_div =7;
#else
	params->dsi.fbk_div =7;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
#endif
	//params->dsi.compatibility_for_nvk = 1; // this parameter would be set to 1 if DriverIC is NTK's and when force match DSI clock for NTK's
}


static void lcm_init(void)
{
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	dsi_set_cmdq_V3(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);
    
}


static  LCM_setting_table_V3 lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x05, 0x28, 0, {}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},

	// Sleep Mode On
	{0x05, 0x10, 0, {}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
};

static void lcm_suspend(void)
{

	dsi_set_cmdq_V3(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting)/sizeof(lcm_deep_sleep_mode_in_setting[0]), 1);
 
	
	SET_RESET_PIN(1);	
	SET_RESET_PIN(0);
	MDELAY(20); // 1ms
	
	SET_RESET_PIN(1);
	MDELAY(120);      
}

static void lcm_resume(void)
{
	
	lcm_init();

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

        SET_RESET_PIN(1);  //NOTE:should reset LCM firstly
    	SET_RESET_PIN(0);
    	MDELAY(1);
    	SET_RESET_PIN(1);
    	MDELAY(150);
	array[0] = 0x00053700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
//	id = read_reg(0xF4);
	read_reg_v2(0xA1, buffer, 5);
	id = ((buffer[2] << 8) | buffer[3]); //we only need ID

    	return (LCM_ID == id)?1:0;
}

// zhoulidong  add for lcm detect (start)
static unsigned int rgk_lcm_compare_id(void)
{
 	int data[4] = {0,0,0,0};
	int res = 0;
	int rawdata = 0;
	int lcm_vol = 0;

#ifdef AUXADC_LCM_VOLTAGE_CHANNEL
	res = IMM_GetOneChannelValue(AUXADC_LCM_VOLTAGE_CHANNEL,data,&rawdata);
	if(res < 0)
	{ 
	#ifdef BUILD_LK
		printf("[adc_uboot]: get data error\n");
	#endif
		return 0;
	}
#endif

	lcm_vol = data[0]*1000+data[1]*10;

	#ifdef BUILD_LK
	printf("[adc_uboot]: lcm_vol= %d\n",lcm_vol);
	#endif
	if (lcm_vol>=MIN_VOLTAGE &&lcm_vol <= MAX_VOLTAGE&&lcm_compare_id())
	{
		return 1;
	}
	return 0;
}



// zhoulidong add for eds(start)
static unsigned int lcm_esd_check(void)
{
	#ifdef BUILD_LK
		//printf("lcm_esd_check()\n");
	#else
		//printk("lcm_esd_check()\n");
	#endif 
 #ifndef BUILD_LK
	char  buffer[3];
	int   array[4];

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x0a, buffer, 1);
	if(buffer[0]==0x9c)
	{
		//#ifdef BUILD_LK
		//printf("%s %d\n FALSE", __func__, __LINE__);
		//#else
		//printk("%s %d\n FALSE", __func__, __LINE__);
		//#endif
		return FALSE;
	}
	else
	{	
		//#ifdef BUILD_LK
		//printf("%s %d\n FALSE", __func__, __LINE__);
		//#else
		//printk("%s %d\n FALSE", __func__, __LINE__);
		//#endif		 
		return TRUE;
	}
 #endif

}

static unsigned int lcm_esd_recover(void)
{
	lcm_init();
	
	#ifndef BUILD_LK
	printk("lcm_esd_recover  ili9806e_dsi_vdo_dijin_fwvga_ips_j511\n");
	#endif
	return TRUE;
}




LCM_DRIVER otm8019a_fwvga_dsi_vdo_trust_ips_oncell_lcm_drv = 
{
    .name			= "otm8019a_fwvga_dsi_vdo_trust_ips_oncell",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = rgk_lcm_compare_id,
//	.esd_check = lcm_esd_check,
//	.esd_recover = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
