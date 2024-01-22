#include "cli_thread.h"
#include "cli.h"
#include "menu_builder.h"

                /* CLI Thread entry function */
                void cli_thread_entry(void)
                     {
                    /* TODO: add your own code here */
                    while(1)
                    {
                        cli_init_data_t my_data;
                        strcpy(my_data.prefix, device_prompt);

                        cli_init(&my_data);
                    
                        cli_set_prompt(device_prompt);
                        tx_thread_sleep(1);
                    }
                }
