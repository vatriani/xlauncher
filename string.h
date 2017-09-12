/*
 * string.c - define some extended string functions
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

#ifndef __STRING_H__
#define __STRING_H__

#include <malloc.h>



extern void *nemmset(void*,int,size_t);
extern void *memcpy(void*,const void*,size_t);
void mallocate(char*,char*,int);
void freeChar(char**);

int strmlen(char*);
char* strmdup(const char*);
int strmcmp(char*, char*);
int strmncmp(char*,char*,unsigned int);
char *strmstr(char*,char*);
void strmcat(char**,char*);
int strmreplace(char**,char*,char*);
void strmrreplace(char**,char*,char*);
void strmdelsp(char*);
char* strmstripws(char*,size_t);
char* strmchr(char*,char);
char* strmrchr(char*,char);
int strmverscmp(char*,char*);
char* strmlwr(char*);

#endif
