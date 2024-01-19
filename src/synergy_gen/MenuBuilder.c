/*
 * MenuBuilder.c
 *
 *  Created on: 12/17/2023
 *      Author: Hazem Kabbara
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "cli.h"
#include "menu_builder.h"
#include "blinky_thread.h"
#include "spm_fsm.h"
   
#define NUM	6
/*
 * extern variables
 */
extern int32_t temp1;
//extern ipglb_system_t _system;
extern char cli_prompt[CLI_PROMPT_LEN];
extern cli_record_t cli_root_element;
extern cli_record_t *current_directory;
//extern board_leds_t LedGYR;
//extern char save;
extern char* sysdescription_str;
//extern int16_t sysdescription_len;
extern float  t_alarms[NUM];

extern int32_t	 t_Tcase[NUM];
//extern SystemInfo CurrentSystemInfo;
extern int32_t tcase;

extern uint8_t flagDCDCVSet;
extern uint32_t	Alarm[2];
//extern char HwNumber[HW_NUM_LEN];
//extern char SwNumber[SW_NUM_LEN];
//extern char SerialNumber[SR_NUM_LEN];
//extern char Pid[PID_LEN];
extern char reboot[5]; //use enum subsystem type

extern uint32_t reboot_reset;
extern uint32_t uILD, T_DC_DC, T_3V3;
extern uint32_t uptime_s;

typedef enum sub_modules
{  UCM,
   GSA,
   GDN1,
   GDN2,
   VIM
}submodules;

submodules my_submodules;
/*
 * Public directories that applications can register with.
 * Enable define for these root directories.  Once defined,
 * apps can register in this directory structure.  If not,
 * applications register to the root directory.
 */
#ifdef CLI_ROOT_DIRS
/* Root Directory */
static cli_record_t cli_gpio_dir;
static cli_record_t cli_adc_dir;
static cli_record_t cli_dac_dir;
static cli_record_t cli_fan_dir;
static cli_record_t cli_can_dir;
static cli_record_t cli_power_dir;
static cli_record_t cli_alarms_cmd;
static cli_record_t cli_sysname_cmd;
static cli_record_t cli_version_cmd;
static cli_record_t cli_reboot_cmd;
static cli_record_t cli_show_dir;
static cli_record_t cli_state_cmd;
static cli_record_t cli_led1_cmd;
static cli_record_t cli_led2_cmd;
static cli_record_t cli_led3_cmd;

/* GPIO  Sub-commands function calls */
static cli_record_t cli_gpioshow_dir;
static cli_record_t cli_gpioshowall_cmd;
static cli_record_t cli_gpioset_cmd;
static cli_record_t cli_gpioclear_cmd;
static cli_record_t cli_gpioread_cmd;

/* ADC  Sub-commands function calls */
static cli_record_t cli_adcshow_dir;
static cli_record_t cli_adcshowall_cmd;

/* DAC  Sub-commands function calls */
//static cli_record_t cli_dacshow_dir;
//static cli_record_t cli_dacshowall_cmd;
static cli_record_t cli_dacset_cmd;
static cli_record_t cli_dacread_cmd;

/* Power  Sub-commands function calls */
static cli_record_t cli_subsystem_cmd;

#endif

static int
is_number (char *input_string){
	int count = strlen(input_string)+1; /* one for Null termination */
	int minus_cnt = 0;
	int dot_cnt = 0;
	char str[sizeof(input_string)+1]; //Hazem was [count]
	memcpy(str, input_string, count);
	str[count] = '\0'; /* terminate by Null */
	int index = 0;
	while(str[index] != '\0'){
		/* 1st check if number is negative */
		if (str[index] == '-') {
			minus_cnt++;
			index++;
			continue;
		}
		/* Should follow by either a number or '.' for decima l*/
		if (isdigit(str[index])) {
			index++;
			continue;
		}
		if (str[index] == '.'){
			dot_cnt++;
			index++;
			continue;
		}
		/* If not minus nor a dot then return not a number */
		else if (!isdigit(str[index])){
			return 0;
		}
		index++;
	}
	/* if we count more than a minus sign or one
	 * decimal point, then not a number
	 */
	if ((dot_cnt > 1) || (minus_cnt > 1)){
		return 0;
	}
	/* If none of the above, then it is a good number */
	else {
		return 1;
	}
}

static void
cli_reboot (uint32_t argc, char *argv[]) {
	if (!strcmp(argv[1], "?")) {
		printf ("\r\nSystemresetc command takes no argument\r\n");
	}
	else {
		printf ("\r\nYou typed reboot\r\n");
	}
}

static void
cli_version (uint32_t argc, char *argv[]){
	printf("\r\nDisplay software version\r\n");
}

static void
cli_gpioshowall (uint32_t argc, char *argv[]){
	printf("\r\nGPIO show all\r\n");
}

static void
cli_gpioset (uint32_t argc, char *argv[]){
	printf("\r\nGPIO set\r\n");
}

static void
cli_gpioclear (uint32_t argc, char *argv[]){
	printf("\r\nGPIO clear\r\n");
}

static void
cli_gpioread (uint32_t argc, char *argv[]){
	printf("\r\nGPIO read\r\n");
}

static void
cli_adcshowall (uint32_t argc, char *argv[]){
	printf("\r\nADC show all channels\r\n");
}

static void
cli_dacset (uint32_t argc, char *argv[]){
	printf("\r\nDAC set channel\r\n");
}

static void
cli_dacread (uint32_t argc, char *argv[]){
	printf("\r\nDAC read channel\r\n");
}

static void
cli_subsystem (uint32_t argc, char *argv[]){
	printf("\r\nPower subsystem\r\n");
}

static void
cli_alarms (uint32_t argc, char *argv[]){
  uint8_t *thisStr = "\r\nDisplay system alarms\r\n";
  printThis(thisStr);
}

static void
cli_sysname (uint32_t argc, char *argv[]){
    uint8_t *thisStr = "\r\nAcute SPM Module\r\n";
    printThis(thisStr);
}

static void
cli_led1 (uint32_t argc, char *argv[]){
    int value;
    bsp_leds_t leds;
    R_BSP_LedsGet(&leds);
    char thisStr[64];
    if ((!strcmp(argv[1], "?")) || (!strcmp(argv[1], "\0"))) {
        sprintf(thisStr, "\r\nOptions are 0 = OFF and 1 = ON \r\n ");
        printThis(thisStr);  
	return;
    }
    else {
	if(is_number(argv[1])) {
            value = atoi(argv[1]);
            if(value == 0) {
              g_ioport.p_api->pinWrite(leds.p_leds[0], IOPORT_LEVEL_HIGH);
              sprintf(thisStr, "\r\nLED1 is turned OFF \r\n");
              printThis(thisStr);
              return;
            }
            else if (value == 1) {
              g_ioport.p_api->pinWrite(leds.p_leds[0], IOPORT_LEVEL_LOW);
              sprintf(thisStr, "\r\nLED1 is turned ON \r\n");
              printThis(thisStr);
              return;
            }
            else {
              sprintf(thisStr, "\r\nInvalid argument \r\n");
              printThis(thisStr);
              return;
            }
	}
    }
    printf ("\r\nInvalid Argument!!\r\n");
}
                
static void
cli_led2 (uint32_t argc, char *argv[]){
}

static void
cli_led3 (uint32_t argc, char *argv[]){
}                

static void
cli_state (uint32_t argc, char *argv[]){
  char thisStr[64];
  spm_fsm_state_e sysState = spm_fsm_get_cur_state();
  sprintf(thisStr, "\r\nCurrent state is ");
  switch (sysState){
  case SPM_FSM_STATE_RESET:
    strcat (thisStr, "SPM_FSM_STATE_RESET \r\n");
    break;
  case SPM_FSM_STATE_POST:
    strcat (thisStr, "SPM_FSM_STATE_POST \r\n");
    break;
  case SPM_FSM_STATE_SELFTEST:
    strcat (thisStr, "SPM_FSM_STATE_SELFTEST \r\n");
    break;
  case SPM_FSM_STATE_IDLE:
    strcat (thisStr, "SPM_FSM_STATE_IDLE \r\n");
    break;
  case SPM_FSM_STATE_COUNT:
    strcat (thisStr, "SPM_FSM_STATE_COUNT \r\n");
    break;
  default:
    strcat (thisStr, "SPM_FSM_STATE_UNKNOWN \r\n");
    break;
  }
  printThis(thisStr);
  return;
}

char menu_build (void) {
#ifdef CLI_ROOT_DIRS
	RC_CLI_t rc;

	/* Main Menu Commands */
    rc = cli_mkdir("gpio", "GPIO management", mon, NULL, &cli_gpio_dir);
    rc = cli_mkdir("can", "CAN management", mon, NULL, &cli_can_dir);
    rc = cli_mkdir("adc", "ADC channel management", mon, NULL, &cli_adc_dir);
    rc = cli_mkdir("dac", "DAC channel management", mon, NULL, &cli_dac_dir);    
    rc = cli_mkdir("fan", "FAN channel management", mon, NULL, &cli_fan_dir);  
    rc = cli_mkdir("power", "Command to manage subsystem power", mon, NULL, &cli_power_dir);
    rc = cli_mkcmd("sysname", "Display system name", mon, &cli_sysname, NULL, &cli_sysname_cmd);
    rc = cli_mkcmd("alarms", "Display oustansding alarms", mon, &cli_alarms, NULL, &cli_alarms_cmd);
    rc = cli_mkdir("show", "Show intended state", mon, NULL, &cli_show_dir);
    
    /*  Show Sub Commands */
    rc = cli_mkcmd("state", "Show state machine current state", mon, &cli_state, &cli_show_dir, &cli_state_cmd);
    /*  GPIO Sub Commands */
    rc = cli_mkcmd("led1", "Manage LED1 (ON, OFF)", mon, &cli_led1, &cli_gpio_dir, &cli_led1_cmd);
    rc = cli_mkcmd("led2", "Manage LED2 (ON, OFF)", mon, &cli_led2, &cli_gpio_dir, &cli_led2_cmd);
    rc = cli_mkcmd("led3", "Manage LED3 (ON, OFF)", mon, &cli_led3, &cli_gpio_dir, &cli_led3_cmd);
 
    return (char)rc;

#endif
}
