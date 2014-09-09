/*
 * common_output.c - define some easy fuctions
 * Copyright © 2014 - Niels Neumann  <vatriani.nn@googlemail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#ifndef __STRING_H__
#include "string.h"
#endif

/**
 * __DATE__ support over Makefile CFLAGS=-DDATE="\"`date +'%Y'`\""
 */
#ifndef __DATE__
	#define __DATE__ "unknown"
#endif

/**
 * set some usable color codes
 */
#define CSI_RED     "\x1b[31;1m"
#define CSI_GREEN   "\x1b[32;1m"
#define CSI_YELLOW  "\x1b[33;1m"
#define CSI_BLUE    "\x1b[34;1m"
#define CSI_MAGENTA "\x1b[35;1m"
#define CSI_CYAN    "\x1b[36;1m"
#define CSI_RESET   "\x1b[0m"

ssize_t write(int,void*,size_t);

static inline void outplain(char *tmp) {write(1,tmp,strmlen(tmp));}
static inline void outplainerr(char *tmp) {write(2,tmp,strmlen(tmp));}

#ifdef COLOR
void styledOutput(char *type, char *mesg, char *color);
#else
void styledOutput(char *type, char *mesg);
#endif

void outerr(char *tmp);
void outdeb(char *tmp);
void outstat(char *tmp);
void showHelp(char *, char *);
void showVersion(char*, char*);

#endif
