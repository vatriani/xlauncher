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

#include "output.h"
#include <stdlib.h>


#ifdef COLOR
void styledOutput(char *type, char *mesg, char *color) {
 outplain("[");
 outplain(color);
 outplain(type);
 outplain("]");
 outplain(CSI_RESET);
 outplain(mesg);
 outplain("\n");
}
#else
void styledOutput(char *type, char *mesg) {
 outplain("[");
 outplain(type);
 outplain("]");
 outplain(mesg);
 outplain("\n");
}
#endif


// extended wrapper for write to stderr with colors
void outerr(char *tmp) {
#ifdef COLOR
 styledOutput("err",tmp,CSI_RED);
#else
 styledOutput("err",tmp);
#endif
}

void outfatal(char *tmp) {
 outerr(tmp);
 exit(1);
}

// extended wrapper for write to stdout as debug messages
#ifdef DEBUG
void outdeb(char *tmp) {
#ifdef COLOR
 styledoutput("deb",tmp,CSI_YELLOW);
#else
 styledoutput("deb",tmp);
#endif
}

void outedeb(char *msg,char *content) {
 outdeb(msg);
 outplain(" : ");
 outplain(content);
 outplain("\n");
}
#endif


void outstat(char *tmp) {
	outplain("  =>  ");
	outplain(tmp);
	outplain("\n");
}

/**
 * output programname -h for help message
 */
void showHelp(char *app_name, char *app_help) {
	char *buffer = NULL;

	strmcat(&buffer, "%1  for Linux, BSD and Windows\nuse: %1 %2\n\n");
	strmreplace(&buffer,"%1",app_name);
	strmreplace(&buffer,"%1",app_name);
	strmreplace(&buffer,"%2",app_help);
	outplain(buffer);
	free(buffer);
}

/**
 * output programname -v for version message
 */
void showVersion(char *app_name, char *app_version) {
	char *buffer = NULL;

	strmcat(&buffer, "%1 %2\nCopyright (C) %3 Niels Neumann  <vatriani.nn@googlemail.com\n\
License GPLv3+: GNU GPL Version 3 or later <http://gnu.org/licenses/gpl.html>.\
 \nThis is free software: you are free to change and redistribute it.\
 \nThere is NO WARRANTY, to the extent permitted by law.\n\n");
	strmreplace(&buffer,"%1",app_name);
	strmreplace(&buffer,"%2",app_version);
	strmreplace(&buffer,"%3",__DATE__);
	outplain(buffer);
	free(buffer);
}
