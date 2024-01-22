/*------------------------------------------------------------------
 * cli.c -- command line interpreter
 *
 * Copyright (c) 2009 by B. Berry
 * All rights resevered. 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
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
#include <inttypes.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>   
#include <ctype.h>   
#include <menu_builder.h>
#include "cli.h"

/** 
 * cli -- command line interpreter
 *  
 * DESCRIPTION
 *    command line interpreter (cli)  provides a directory
 *    hierarchy of commands similar in concept to the linux
 *    directory structure.  There exists the concept of a root
 *    directory and subdirectory.  Subdirectories can be created
 *    under root or under other subdirectories.  Commands can
 *    be created under root or any subdirectory.
 *
 *    When the user enters a command, the matching command 
 *    structure is located from the registered commands
 *    and the associated call-back function is invoked to 
 *    execute the command.
 * 
 *    When the user enters a directory name to change directories,
 *    the cli will change to that directory and display the
 *    contents.  This new directory becomes the present
 *    working directory (pwd).
 *
 *    User call backs are invoked with the (argc, *argv[]) parameters.
 *    argv[0] is the command string while argv[1,2,...,ARGC_MAX] are
 *    command specific parameters.  
 *
 *    There are several built-in commands for directory navigation:
 *    h    help  
 *    ~    moves to the top directory, root
 *    ..   moves the directory up one level
 *    ld   lists both commands and directories
 *    !    (bang) global command execution  
 *    ls   global list of commands 
 *
 *    Optional built in directories 
 *      cli_config_dir - Commands to manage configuration.  
 *      cli_show_dir - Commands to show internals: configuration, data, etc.  
 *      cli_clear_dir - Commands to clear counters, runtime data, etc. 
 *      cli_opertion_dir - Commands to manage the runtime operation. 
 *      cli_debug_dir - Commands to facilitate debugging. 
 * 
 * PUBLIC FUNCTIONS  
 *     cli_mkcmd       - add a command to a directory
 *     cli_mkdir       - add a subdirectory to directory
 *     cli_engine      - command processing engine  
 *     cli_init        - initialization
 * 
 * 
 */

/*
 * #define CLI_DEBUG  ( 1 )
 */
//#define CLI_DEBUG  ( 1 )

#ifdef CLI_DEBUG  
#define DEBUG_PRINTF(myargs...)   printf(myargs) 
#else
#define DEBUG_PRINTF(myargs...)  ((void)0)
#endif


/*
 * The maximum number of entries displayed on a single line
 * is ENTRIES_PER_LINE. If there are more than ENTRIES_PER_LINE,
 * a newline is injected to avoid line wrap.  Useful for small
 * screens. 
 */
#define ENTRIES_PER_LINE   ( 3 )

//#define PROMPT_MAX_LEVEL   ( 4 )

#define boolean_t u_int8_t
#define FALSE 0
#define TRUE  1

/*
 * navigation commands
 */
#define CLI_H             	"h"       	/* h => help */
#define CLI_QUESTION     	"?"       	/* help */
#define CLI_UP_ONE        	".."      	/* move up one directory level */
#define CLI_ROOT_DIR      	"~"       	/* move to root directory  */
#define CLI_GLOBAL_LIST   	"?"      	/* global command list   */
#define SUPERUSER		  	"SU08!29@66#" /* Switch to Super User as root */
#define ADMINUSER			"Adm08!29@66#" /* Switch to Administrator User */
#define MONUSER				"Mon08!29@66#"	/* Switch to Monitor User */
/*
 * root directory structure
 */
cli_record_t cli_root_element;

/*
 * pointer to present working directory (pwd)
 */
cli_record_t *current_directory;


static struct t_nodal_record  *list;

//extern ipglb_card_t card;
extern char sys_name[32];
char cli_prompt[CLI_PROMPT_LEN];

char device_prompt[48] = "SPM";

menu_prio uprio = mon;

static void
menu_help ()
{
    char tmpStr[64];
    sprintf(tmpStr, "\r\nh    \t\tFor help");
    printThis(tmpStr);
    sprintf(tmpStr, "\r\n~    \t\tReturns to the top or root command directory");
    printThis(tmpStr);
    sprintf(tmpStr, "\r\n..   \t\tMoves from the current directory up one directory");
    printThis(tmpStr);
    sprintf(tmpStr, "\r\n?    \t\tShow options or command help\r\n");
    printThis(tmpStr);
    return;
}

/*
 * This function will keep track of the prompt structure
 */
void update_prompt(void) {
	int myindex = 0;
	char dir_chain[4][48];
	cli_record_t *ptr_directory;
	if (strcmp((char *)sys_name, "\0")) {
		strcpy(device_prompt, (char *)sys_name);
	}
	if (current_directory == &cli_root_element) {
		strcpy (cli_prompt, device_prompt); 
	}
	else {
	/* If not root directory, go the next directory level */
		ptr_directory = current_directory;
		strcpy (&dir_chain[myindex][0], ptr_directory->name);
		while (ptr_directory->parent_dir != &cli_root_element) {
			myindex++;
			ptr_directory = ptr_directory->parent_dir;
			strcpy (&dir_chain[myindex][0], ptr_directory->name);
		}
		myindex++;
		/* Do it one more time to get the root directory */
		ptr_directory = ptr_directory->parent_dir;
		strcpy (&dir_chain[myindex][0], ptr_directory->name);
		/* Start copying to prompt */
		strcpy (cli_prompt, &dir_chain[myindex][0]);
		/* if in directories, concatenate to the prompt string */
		while (myindex) {
			strcat(cli_prompt, "\\");
			strcat(cli_prompt, &dir_chain[myindex-1][0]);
			myindex--;
		}
	}
	switch (uprio) {
	case su:
		strcat(cli_prompt, "#");
		break;
	case adm:
		strcat(cli_prompt, "@");
		break;
	default:
		strcat(cli_prompt, ">>");
		break;
	}
}

static bool
str_exact (char *p1, char *p2, size_t n)
{
    while (*p1) {
       if (*p1 == *p2) {
           p1++;
           p2++;
       } else {
           return (FALSE);
       }
    }

    if (*p1 == *p2) {
        return (TRUE);
    } else {
        return (FALSE);
    }
}




/*
 * NAME 
 *    cli_bind_entry
 * 
 * SYNOPSIS
 *    #include "cli.h" 
 *    static RC_CLI_t
 *    cli_bind_entry(char *name,
 *            cli_funcvar_t fv,
 *            cli_record_t *dir_rec,
 *            cli_record_t *cmd_rec)
 *
 * DESCRIPTION
 *    This is an internal function to add a command/directory 
 *    into the specified chain.  To add an entry into the 
 *    root directory, specify NULL for the directory pointer.
 *
 * INPUT PARAMETERS
 *    name           the name of the user command.
 *    desc			 user command description
 *    priority		 Command priority based on user level
 *
 *    fv             a pointer to the target function to be invoked
 *                   when the command is entered.  If NULL then
 *                   the name is considered to be a directory.
 *
 *    dir_rec        a pointer to the directory record
 *
 *    cmd_rec        a pointer to the command record to be added
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 * 
 */
static RC_CLI_t
cli_bind_entry (char *name, char *desc, menu_prio prio,
             cli_funcvar_t fv,
             cli_record_t *dir_rec,
             cli_record_t *cmd_rec)
{
    cli_record_t  *p2list;
    cli_record_t  *p2parent_directory;

    if (name == NULL) {
        return (RC_CLI_NULL_NAME);
    }

    if (cmd_rec == NULL) {
        return (RC_CLI_NULL_CMD);
    }

    if (dir_rec == NULL) {
        p2list = &cli_root_element;
    } else {
        p2list = dir_rec;
    }

    p2parent_directory = p2list;


    /*
     * can not re-use a record
     */
    if (cmd_rec == p2list) {
        return (RC_CLI_RECORD_CORRUPT);
    }

    /*
     * check if the directory is empty to add the
     * first entry
     */
    if (p2list->link2subdir == NULL) {
        p2list->link2subdir = cmd_rec;

        DEBUG_PRINTF("\"%s\" is the First entry in directory [%s]\n",
                  name, p2list->name );

    } else {
        /*
         * Directory is not empty, so walk the list to
         * check for duplicates and append the entry.
         */

        p2list = p2list->link2subdir;

        while (p2list->next_entry != NULL)  {
            /*
             * can not re-use a record
             */
            if (cmd_rec == p2list) {
                return (RC_CLI_RECORD_CORRUPT);
            }

            /*
             * do not allow duplicate names within the same subdirectory
             */
            if (str_exact(p2list->name, name, CLI_ENTRY_LEN)) {
                return (RC_CLI_DUPLICATE);
            }

            p2list = p2list->next_entry;
        }

        /*
         * Now we can append the new entry.
         */
        p2list->next_entry = cmd_rec;

        DEBUG_PRINTF("Appended \"%s\" in directory [%s]\n",
                name, dir_rec->name);
      }

    /*
     * setup the new entry record
     */
    strncpy(cmd_rec->name, name, CLI_ENTRY_LEN-1);
    strncpy(cmd_rec->desc, desc, CLI_ENTRY_DESC_LEN-1);
    cmd_rec->name[CLI_ENTRY_LEN-1] = '\0';
    cmd_rec->desc[CLI_ENTRY_DESC_LEN-1] = '\0';
    cmd_rec->priority	  = prio;
    cmd_rec->parent_dir   = p2parent_directory;
    cmd_rec->next_entry   = NULL;
    cmd_rec->link2subdir  = NULL;
    cmd_rec->fv           = fv;

    if (cmd_rec->fv) {
        cmd_rec->cmd_list     = list;
        list                  = cmd_rec;
    }
    
    return (RC_CLI_OK);
}


/** 
 * NAME
 *    cli_mkcmd
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    RC_CLI_t
 *    cli_mkcmd(char  *name, char *desc,
 *          cli_funcvar_t fv,
 *          cli_record_t *dir_rec,
 *          cli_record_t *cmd_rec)
 *
 * DESCRIPTION
 *    Add the new command into the specified directory.
 *    To add a command into the root directory,
 *    specify NULL for the directory pointer.
 *
 * INPUT PARAMETERS
 *    name            pointer to the command name.
 *	  desc			  pointer to the command description string
 *
 *    fv              pointer to the target function to be invoked
 *                    when the command is entered.
 *
 *    dir_rec         pointer to the directory record where the
 *                    new command is to be added.  Use NULL for root.
 *
 *    cmd_rec         pointer to the new command record.
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 *
 * EXAMPLES  
 *    The examples below add three commands to my_directory.
 *
 *      rcode = cli_mkcmd ( "command_one",   "command_desc_one", funct_one,
 *                            &my_directory_record, &cmd_one_record );
 *      rcode = cli_mkcmd ( "command_two",   "command_desc_two", funct_two,
 *                            &my_directory_record, &cmd_two_record );
 *      rcode = cli_mkcmd ( "command_three", "command_desc_three",funct_three,
 *                            &my_directory_record, &cmd_three_record );
 * 
 */
RC_CLI_t
cli_mkcmd (char *name, char *desc, menu_prio prio,
       cli_funcvar_t fv,
       cli_record_t *dir_rec,
       cli_record_t *cmd_rec)
{
    RC_CLI_t rc;

    if (fv == NULL) {
        return (RC_CLI_NULL_FV);
    }

    /*
     * bind into ROOT if directory not specified
     */
    if (dir_rec == NULL) {
        dir_rec = &cli_root_element;
    }

    /*
     * you must bind only to a directory
     */
    if (dir_rec->fv != NULL) {
       return (RC_CLI_DIR_INVALID);
    }

    rc = cli_bind_entry(name, desc, prio, fv, dir_rec, cmd_rec);
    return (rc);
}


/** 
 * NAME 
 *     cli_mkdir
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    RC_CLI_t
 *    cli_mkdir(char *name, char *desc, cli_record_t *dir_rec,
 *              cli_record_t *sub_dir)
 *
 * DESCRIPTION
 *    Add a new directory into the specified directory.
 *    To add a new directory into the ROOT directory,
 *    specify NULL for the directory pointer.
 *
 * INPUT PARAMETERS
 *    name            pointer to the direcory name.
 *    desc			  pointer description to the name command
 *    dir_rec         pointer to the directory record where the
 *                    new subdir is to be added.  Use NULL for root.
 *
 *    cmd_rec         pointer to the new directory record.
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 *
 * EXAMPLES
 *    The example below adds a_directory (subdirectory) to the 
 *    root directory.  This is a visible entry.
 *
 *       rc = cli_mkdir( "a_directory", "a_description", priority, NULL,
 *                       &a_directory_record);
 *
 *
 *    The example below adds a_sub_directory (subdirectory) to the
 *    previously created directory 'a_directory_record'.
 *
 *       rc = cli_mkdir( "a_sub_directory", "a_description", priority,
 *       				 &a_directory_record,
 *                       &a_subdir_record);
 * 
 */
RC_CLI_t
cli_mkdir (char *name, char *desc, menu_prio prio, cli_record_t *dir_rec,
                       	   	   	   cli_record_t *sub_dir)
{
    RC_CLI_t rc;

    /*
     * bind into ROOT if the directory is not specified
     */
    if (dir_rec == NULL) {
        dir_rec = &cli_root_element;
    }

    /*
     * must bind only to an existing directory
     */
    if (dir_rec->fv != NULL) {
        return (RC_CLI_DIR_INVALID);
    }

    rc = cli_bind_entry(name, desc, prio, NULL, dir_rec, sub_dir);
    return (rc);
}

/*
 * NAME
 *    list_directory
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    static void
 *    list_directory(void)
 *
 * DESCRIPTION
 *    Display the contents of the directory, both commands and
 *    subdirectories. Directories are enclosed in brackets, [..].
 *
 * INPUT PARAMETERS
 *    none 
 *
 * RETURN VALUE
 *    void
 * 
 */
static void  //ld
list_directory ()
{
   cli_record_t *cmd_tmp;
   int rx_tunable = 0;
   int tx_tunable = 0;
   int tx_power_control = 0;
   char tmpStr[64];
    /*
     * first list all directories
     */
    cmd_tmp = current_directory->link2subdir;
    while (cmd_tmp != NULL) {
        /* fv == NULL means it is a directory */
        if ((cmd_tmp->fv == NULL) && (uprio >= cmd_tmp->priority)) {
            sprintf (tmpStr, "%-12s", cmd_tmp->name);
            printThis(tmpStr);
            sprintf(tmpStr, "\t%s\r\n", cmd_tmp->desc);
            printThis(tmpStr);
        }
        cmd_tmp = cmd_tmp->next_entry;
    }

    cmd_tmp = current_directory->link2subdir;
    while (cmd_tmp != NULL) {
    	/*
    	 * Apply filters here.  No need in the hybrid code, just
    	 * remove all the if statements and keep one of the 2 printfs
    	 * Use ifdef to the file usable in both projects.
    	 */
        if ((cmd_tmp->fv != NULL)  && (uprio >= cmd_tmp->priority)) {
        	/* If none of these cmds print it */
        	if (!(!strcmp(cmd_tmp->name, "pout") || !strcmp(cmd_tmp->name, "rxfrq") || !strcmp(cmd_tmp->name, "txfrq"))) {
                      sprintf (tmpStr, "%-12s", cmd_tmp->name);
                      printThis(tmpStr);
                      sprintf(tmpStr, "\t%s\r\n", cmd_tmp->desc);
                      printThis(tmpStr);
                      cmd_tmp = cmd_tmp->next_entry;
                      continue;
			}
        	/* Handle special cases for the sub-commands */
			if ((!strcmp(cmd_tmp->name, "rxfrq")) && (rx_tunable)) {
				sprintf (tmpStr, "%-12s", cmd_tmp->name);
                                printThis(tmpStr);
                                sprintf(tmpStr, "\t%s\r\n", cmd_tmp->desc);
                                printThis(tmpStr);
				cmd_tmp = cmd_tmp->next_entry;
				continue;
			}
			if ((!strcmp(cmd_tmp->name, "txfrq")) && (tx_tunable)) {
                            sprintf (tmpStr, "%-12s", cmd_tmp->name);
                            printThis(tmpStr);
                            sprintf(tmpStr, "\t%s\r\n", cmd_tmp->desc);
                            printThis(tmpStr);
                            cmd_tmp = cmd_tmp->next_entry;
                            continue;
			}
			if ((!strcmp(cmd_tmp->name, "pout")) && (tx_power_control)) {
                            sprintf (tmpStr, "%-12s", cmd_tmp->name);
                            printThis(tmpStr);
                            sprintf(tmpStr, "\t%s\r\n", cmd_tmp->desc);
                            printThis(tmpStr);
			}

        }
        cmd_tmp = cmd_tmp->next_entry;
    }
    return;
}


/*
 * NAME 
 *    cli_find_cmd
 * 
 * SYNOPSIS
 *    #include "cli.h" 
 *    static cli_record_t
 *    *cli_find_cmd(char  *name)
 *
 * DESCRIPTION
 *     An internal function to scan the current directory for the
 *     specified entry.
 *
 * INPUT PARAMETERS
 *     name       pointer to the string to look for (null terminated).
 *
 * RETURN VALUE
 *     NULL       string NOT found
 *     ptr        Pointer to entry record if a match was found
 *
 */
static cli_record_t
*cli_find_cmd (char  *name) 
{
    cli_record_t  *cmd_rec;

    DEBUG_PRINTF("\nAttempting to Find \"%s\" in directory [%s]\n",
            name, current_directory->name );

    /*
     * check for global command search  
     */
    if (*name == '!') {
        DEBUG_PRINTF("\nGlobal command search! \n");

        name++;
        cmd_rec = list;
        while (cmd_rec != NULL) {
            DEBUG_PRINTF("\nglobal comparing \"%s\" with entry \"%s\"\n",
                      name, cmd_rec->name);
            if (str_exact(cmd_rec->name, name, CLI_ENTRY_LEN)) {
                DEBUG_PRINTF("\nGlobal match found for -%s- \n", name);
                return (cmd_rec);
            }
            cmd_rec = cmd_rec->cmd_list;
        }
        return (NULL);
    }

    /*
     * now compare to existing command and directory entries
     */
    cmd_rec = current_directory->link2subdir;
    while (cmd_rec != NULL) {
        DEBUG_PRINTF("\ncomparing \"%s\" with entry \"%s\"\n", 
                      name, cmd_rec->name);
        if (str_exact(cmd_rec->name, name, strlen(cmd_rec->name))) {
            DEBUG_PRINTF("\nMatch found for -%s- \n", name);
            return (cmd_rec);
        }
        cmd_rec = cmd_rec->next_entry;
    }

    /*
     * check to see if the user wants to go back one directory
     */
    if (str_exact(name, CLI_UP_ONE, 2)) { 
        DEBUG_PRINTF("\nGo up one dir level \n");
        current_directory = current_directory->parent_dir;
        cli_prompt[strlen(sys_name)] = '\0';
        update_prompt();
        return (NULL);
    }

    /*
     * check to see if user wants to go to the root directory
     */
    if (str_exact(name, CLI_ROOT_DIR, 1)) { 
        DEBUG_PRINTF("\nGo to root \n");
        current_directory = &cli_root_element;
        cli_prompt[strlen(sys_name)] = '\0';
        update_prompt();
        return (NULL);
    }

    /*
     * check to see if user wants to list entries
     */

    if (str_exact(name, CLI_H, 1) ||  
        str_exact(name, CLI_GLOBAL_LIST, 2)) {
        menu_help();
        list_directory();
        cli_prompt[strlen(sys_name)] = '\0';
        update_prompt();
        return (NULL);
    }

    if (str_exact(name, SUPERUSER, 9)) {
        uprio = su;
        return (NULL);
    }

    if (str_exact(name, ADMINUSER, 9)) {
        uprio = adm;
        return (NULL);
    }

    if (str_exact(name, MONUSER, 7)) {
        uprio = mon;
        current_directory = &cli_root_element;
        cli_prompt[strlen(sys_name)] = '\0';
        update_prompt();
        return (NULL);
    }
    /* did not find a match */
    return (NULL);
}

/**
 * 	digits_only
 *
 * 	SYNOPSYS
 *    int
 *    digits_only(char *input_string)
 *
 * DESCRIPTION
 *    Digits only
 *
 * INPUT PARAMETERS
 *    s - The input string will check if digits only.
 *        string should be null terminated.
 *
 *
 * RETURN VALUE
 *    1 for yes and 0 for not digits
 *
 */
static
int digits_only(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}

/**
 * 	check_build_in_cmds
 *
 * 	SYNOPSYS
 *    int
 *    check_build_in_cmds(char *input_string)
 *
 * DESCRIPTION
 *    Check the build in cmds
 *
 * INPUT PARAMETERS
 *    input_string - The input string will check if matches with the
 *    				 built in commands.
 *
 * RETURN VALUE
 *    1 for match and 0 for no match
 *
 */

static
int check_built_in_cmds(char *input_string){
	if(!(strcmp(input_string,"h")) || !(strcmp(input_string,"~")) || !(strcmp(input_string,"..")) ||
			!(strcmp(input_string,"?")) || !(strcmp(input_string,"SU08!29@66#")) ||
			!(strcmp(input_string,"Adm08!29@66#")) || !(strcmp(input_string,"Mon08!29@66#"))) {
	return 1;
	}
	else {
		return 0;
	}
}

/** 
 * NAME 
 *    cli_engine 
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    void
 *    cli_engine(char *input_string)
 *
 * DESCRIPTION
 *    The CLI engine.
 *
 * INPUT PARAMETERS
 *    input_string - The string as entered by the user.  The cli
 *                   processor will use the first token to match a
 *                   valid directory or command entry.  If the
 *                   entry matches with a directory, the current
 *                   directory is changed to that directory.
 *
 *                   If the entry is a command, the remainder of the
 *                   string is passed to the user function.  The user
 *                   function can then continue parsing token.
 *
 * RETURN VALUE
 *    None 
 *
 * :
 *    For example, a simple CLI process.
 *     for(;;) {
 *         gets(input_string);
 *         cli_engine(input_string);
 *     }
 * 
 */
void
cli_engine (char  *input_string)
{
    cli_record_t *next_level;
    uint32_t i;
    char *p2str;
    uint16_t count;
    uint32_t argc;
    char *argv[ARGC_MAX];
    char tmpStr[64];

    if ((input_string == NULL) || (*input_string == '\0')) {
        DEBUG_PRINTF("\nEmpty string entered\n");
        update_prompt();
        printf("\r\n%s ", cli_prompt);
        cli_prompt[0] = '\0';
        return;
    }

    for (i=0; i<ARGC_MAX; i++) {
       argv[i] = NULL;
    }

    /*
     * Tokenize the user input into std C syntax
     * argv[0] - dir or command name
     * argv[1] - first parm
     *  ....
     */
    p2str = input_string;

    argv[0] = strtok(p2str, ", \t\n\r");

    if (argv[0] == '\0') {
        DEBUG_PRINTF("\nEmpty keyword\n");
        update_prompt();
        sprintf (tmpStr, "\r\n%s", cli_prompt);
        printThis(tmpStr);
        cli_prompt[0] = '\0';
        return;
    } 

    DEBUG_PRINTF("\n0 input token \"%s\"\n", argv[0]);

    /*
     * now pick off parameters.
     */
    argc = 1;
    i=1;
    while(1){
    	argv[i] = strtok(NULL, " \t\n\r");
    	if (argv[i] == NULL){
    		break;
    	}
    	i++;
    }
    for (i=1; i<ARGC_MAX; i++) {
        if (argv[i] == NULL) {
            break;
        } else {
            argc++;

            DEBUG_PRINTF("\n%u input token \"%s\"\n", i, argv[i]);
        }
    } 
    count = 0;

    /*
     * Search for the entry in current list
     */
    while (count < argc){
    	next_level = cli_find_cmd(argv[count]);
    	if (next_level) {
    		/*
    		 * Found a matching entry.
    		 * If there is a function associated with this entry,
    		 * then invoke it, else it is a directory.
    		 */
    		if ((next_level->fv != NULL)  && (uprio >= next_level->priority)) {
    			/*
    			 * call users registered function
    			 */
    			(next_level->fv)(argc, &argv[count]);
    			count++;
    			break;
    		} else {
    			/*
    			 * change to the subdirectory
    			 */
    			if (uprio >= next_level->priority) {
    				current_directory = next_level;
    			}
    			count++;
    		}
    	} else {
    		/* Did not find a match in the command list, check the built in commands */
			if (!check_built_in_cmds(argv[count])){
                                sprintf (tmpStr, "\r\nInvalid command \"%s\"\r\n\0", argv[0]);
                                printThis((char *)tmpStr);
				count++;
				break;
			}
			else {
				break;
			}
    	}
    }
    	update_prompt();
        sprintf (tmpStr, "\r\n%s\0", cli_prompt);
        printThis(tmpStr);
    	cli_prompt[0] = '\0';
    return;
}



/** 
 * NAME
 *    cli_set_prompt
 *
 * SYNOPSIS
 *    #include "cli.h"
 *    void
 *    cli_set_prompt(char *prompt)
 *
 * DESCRIPTION
 *    This function is used to set a prompt string.
 *
 * INPUT PARAMETERS
 *    prompt - pointer to the string prompt
 *
 * RETURN VALUE
 *    none
 * 
 */
void
cli_set_prompt (char *prompt)
{
    if (prompt) {
        strncpy(cli_prompt, prompt, CLI_PROMPT_LEN);
        cli_prompt[CLI_PROMPT_LEN-1] = '\0';
    } 

    return;
}



/** 
 * NAME 
 *    cli_init
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    RC_CLI_t
 *    cli_init (cli_init_data_t *init_data)
 *
 * DESCRIPTION
 *    This function initializes the CLI facility. All the 
 *    parameters listed in init_data must be provided.
 *
 * INPUT PARAMETERS
 *    init_data    pointer to initialization information
 *                 prefix - string to be displayed at the
 *                          root directory
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 * 
 */
RC_CLI_t
cli_init (cli_init_data_t *init_data)
{
    strncpy(cli_prompt, (char *)sys_name , CLI_PROMPT_LEN); //device_prompt
    cli_prompt[CLI_PROMPT_LEN-1] = '\0';

    /*
     * initialize the ROOT directory
     */
    strncpy(cli_root_element.name, (char *)sys_name , CLI_ENTRY_LEN); //init_data->prefix
    strncpy(cli_root_element.desc, init_data->prefix, CLI_ENTRY_DESC_LEN);
    cli_root_element.name[CLI_ENTRY_LEN-1] = '\0';
    cli_root_element.desc[CLI_ENTRY_DESC_LEN-1] = '\0';
    cli_root_element.priority 	  = mon;
    cli_root_element.fv           = NULL;
    cli_root_element.parent_dir   = &cli_root_element;
    cli_root_element.next_entry   = NULL;
    cli_root_element.link2subdir  = NULL;

    /* mark end of the command list */
    list = NULL;

    menu_build();
    current_directory = &cli_root_element;
    return (RC_CLI_OK);
}


