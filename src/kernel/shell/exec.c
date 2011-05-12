/*
 * kernel/shell/exec.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <version.h>
#include <vsprintf.h>
#include <xboot/printk.h>
#include <xboot/list.h>
#include <fs/fsapi.h>
#include <shell/readline.h>
#include <shell/env.h>
#include <shell/parser.h>
#include <shell/exec.h>
#include <command/command.h>

/*
 * exec the command line
 */
void exec_cmdline(const x_s8 * cmdline)
{
    struct command * cmd;
    x_s32 n;
    char **args;
    char *p, *buf, *pos;
    x_s32 ret;

    if(!cmdline)
    	return;

    p = buf = malloc(strlen((const char *)cmdline) + 2);
    if(!p)
    	return;

    strcpy(p, (const char *)cmdline);
    strcat(p, " ");

    while(*p)
    {
    	if(parser((const x_s8 *)p, &n, (x_s8 ***)&args, (x_s8 **)&pos))
    	{
    		if(n > 0)
    		{
    			cmd = command_search((char*)args[0]);
    			if(cmd)
    			{
    				ret = cmd->func(n, (const x_s8 **)args);
    				if(ret != 0)
    				{
    					/*
    					 * if having other command which waitting be exec, abort.
    					 */
    			    	if(pos)
    			    	{
    			    		printk(" when exec \'%s\' command return an error code (%ld).\r\n", args[0], ret);
    			    		free(args[0]);
    			    		free(args);
    			    		break;
    			    	}
    				}
    			}
    			else
    			{
    				printk(" could not found \'%s\' command \r\n", args[0]);
    				printk(" if you want to kown available commands, type 'help'.\r\n");

					/*
					 * if having other command which waitting be exec, abort.
					 */
			    	if(pos)
			    	{
			    		free(args[0]);
			    		free(args);
			    		break;
			    	}
    			}
    		}

    		free(args[0]);
    		free(args);
    	}

    	if(!pos)
    		*p = 0;
    	else
    		p = pos;
    }

    free(buf);
}
