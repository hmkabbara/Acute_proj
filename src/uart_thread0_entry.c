#include <stdio.h>
#include "uart_thread0.h"
#include "cli.h"

#define  messageSize  64+2 //add CR/LF
#define  CR     0x0D
#define  LF     0x0C
#define  BS     0x08
#define  SP     0x20

#define OUTPUT_BUFFER_SIZE 64
char BSS[2] =  {SP, BS};
char RxMessage[messageSize];
char myChar[1] = {0};

static void clearBuffer (char *myBuf, int count);

void printThis (char *str)
{
  g_sf_comms0.p_api->write(g_sf_comms0.p_ctrl, (uint8_t *)str, strlen(str), 5);
}

// Redefine puts to handle printf output
int __write(int file, char *buffer, int count)
{ 
    // Write String to UART
    for (int i = 0; i < count; i++)
    {
        // Start Transmission
        ssp_err_t err = g_sf_comms0.p_api->write(g_sf_comms0.p_ctrl, (uint8_t *)(buffer + i), 1, 2);
        if (err != SSP_SUCCESS)
        {
            break;
        }
    }
    return 0;
}

/* UART Thread entry function */
void uart_thread0_entry(void)
{
    g_sf_comms0.p_api->open(g_sf_comms0.p_ctrl, g_sf_comms0.p_cfg);

    uint8_t y = 0;
    while(1)
    {
        while ((*myChar != CR) && (y < messageSize-2))
        {
            myChar[0] = 0;
            ssp_err_t err = g_sf_comms0.p_api->read(g_sf_comms0.p_ctrl, (uint8_t *)myChar, 1, 2);
            if (err == SSP_SUCCESS)
            {
                if (myChar[0] == BS) // accomodate for back space.
                {
                    if (y > 0)
                    {
                      printThis(BSS);
                      RxMessage[--y] = 0;
                    }
                }
                else
                {
                  RxMessage[y++] = myChar[0];
                }
            }
            tx_thread_sleep(1);
        }
        cli_engine((char *)RxMessage);
        clearBuffer(RxMessage, sizeof(RxMessage));
        y = 0;
        myChar[0] = 0;
    } 
}
static void clearBuffer (char *myBuf, int count)
{
    for (int i = 0; i < count; i++)
    {
        myBuf[i] = 0;
    }
}
