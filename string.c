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

#include "string.h"
#include <malloc.h>

int isspace(int);


void *memmset(void *s, int c, register size_t n) {
	while(n--)
		*(unsigned char*)s++ = (unsigned char)c;

	return s;
}



void *memcpy(void *dest, const void *src, register size_t n) {
	while (n--)
		*(char*)dest++ = *(const char*)src++;

	return dest;
}


void mallocate(char *array, char c,register int size) {
	while(--size)
		array[size] = c;
}




void freeChar(char** tmp) {
	if(*tmp != NULL) {
		free(*tmp);
		*tmp = NULL;
	}
}




int strmlen(char *s) {
	register int n = 0;

	if(s == NULL)
		return 0;

	for (; *s != 0; ++s)
		++n;
	return n;
}


char* strmdup(const char* s) {
	char *dest = NULL;

	dest = (char*)malloc(strmlen((char*)s)+1);

	if(dest == NULL)
		return NULL;

	memcpy(dest, s, strmlen((char*)s)+1);
	return dest;
}



int strmcmp(char *s, char *t) {
	register int i = 0;

	for (; s[i] == t[i]; ++i)
		if (s[i] == '\0')
			return 0;

	return s[i] - t[i];
}



int strmncmp(char *s, char *t, unsigned int size) {
	register int i = 0;
	--size;
	for (; s[i] == t[i] && i < size; ++i)
		if (s[i] == '\0' || i == size)
			return 0;
	return s[i] - t[i];
}



char* strmstr(char* s, char* t) {
	int tlen = strmlen(t);
	unsigned int difflen = strmlen(s) - tlen;
	register int i = 0;

	for (; i < difflen; ++i)
		if (strmncmp(t, s+i, tlen) == 0)
			return s+i;
	return 0;
}



/**
 * extends strcat and combined with strcpy with malloc needed space
 * needs to be free() befor terminated programm
 */
void strmcat(char **base_str,char *append_str) {
	char *tmp = NULL;
	int const applen = strmlen(append_str);
	int const baselen = strmlen(*base_str);

	if(*base_str == NULL) {
		*base_str = (char*)malloc(applen + 1);
		if(*base_str != NULL)
			memcpy(*base_str, append_str, applen + 1);
	} else {
		tmp = *base_str;
		*base_str = malloc(baselen + applen + 1);
		if(*base_str!=NULL) {
			memcpy(*base_str, tmp, baselen);
			memcpy(*base_str + strmlen(tmp), append_str, applen+1);
		}
	}
	freeChar(&tmp);
}



/**
 * replace in a string the replace_hint with insert_word
 * need to be free() befor terminating
 */
int strmreplace(char **string, char *replace_hint, char *insert_word) {
	int oldLen;
	int newLen;
	char *tmp[3];
	char *pointer;

	pointer = strmstr(*string, replace_hint);

	if(pointer == NULL)
		return -1;

	oldLen = strmlen(*string)+1;
	newLen = oldLen-strmlen(replace_hint)+strmlen(insert_word)+1;

	tmp[0] = malloc(oldLen);
	tmp[1] = malloc(oldLen);
	tmp[2] = malloc(oldLen + strmlen(insert_word));
	memcpy(tmp[0], *string, oldLen);
	*string = NULL;

	pointer = strmstr(tmp[0], replace_hint);
	pointer += strmlen(replace_hint);
	memcpy(tmp[2], pointer, strmlen(pointer)+1);

	pointer = strmstr(tmp[0], replace_hint);
	*pointer = '\0';
	memcpy(tmp[1], tmp[0], strmlen(tmp[0])+1);

	*string = malloc(newLen+1);
	pointer = *string;
	memcpy(pointer, tmp[1], strmlen(tmp[1]));
	pointer += strmlen(tmp[1]);
	memcpy(pointer, insert_word, strmlen(insert_word));
	pointer += strmlen(insert_word);
	memcpy(pointer, tmp[2], strmlen(tmp[2])+1);

	freeChar(&tmp[0]);
	freeChar(&tmp[1]);
	freeChar(&tmp[2]);
	return 0;
}



void strmrreplace(char **string, char *replace_hint, char *insert_word) {
	while( strmreplace(string,replace_hint,insert_word) == 0);
}

/**
 * removing tailing spaces and non printable chars
 */
void strmdelsp(char *s) {
	if(s != NULL) {
		char test[strmlen(s) +1];
		register unsigned int counter = 1;

		memcpy(test,s,strmlen(s)+1);
		for(;counter<(strmlen(test));++counter) {
			test[counter-1]=test[counter];
			if(test[counter]<32)
				test[--counter]=0;
		}
		memcpy(s,test,strmlen(test)+1);
	}
}



char* strmstripws(char *str, size_t len) {
	register int i = 0;
	register int j = 0;
	char *newstr = NULL;
	int newLength = 0;

	for( ;isspace(str[i]); ++i);

	for(j=len-1; j > 0 && isspace(str[j]); --j) ;
	++j;

	newLength = j-i;
	newstr = (char *)calloc(newLength+1, sizeof(char));
	if(!newstr)
		return NULL;

	memcpy(newstr, str + i, newLength);
	newstr[newLength] = '\0';

	return newstr;
}



char* strmchr(char* str, char searchFor) {
	char* pointer = NULL;
	register int counter = 0;
	int strlen = strmlen(str);

	for(; counter <= strlen; ++counter)
		if(str[counter] == searchFor) {
			pointer = &str[counter];
			break;
		}

	return pointer;
}



char* strmrchr(char* str, char searchFor) {
	char* pointer = NULL;
	register int counter = strmlen(str);

	for(; counter >= 0; --counter)
		if(str[counter] == searchFor) {
			pointer = &str[counter];
			break;
		}

	return pointer;
}



/* states: S_N: normal, S_I: comparing integral part, S_F: comparing
           fractional parts, S_Z: idem but with leading Zeroes only */
#define S_N    0x0
#define S_I    0x4
#define S_F    0x8
#define S_Z    0xC

/* result_type: CMP: return diff; LEN: compare using len_diff/diff */
#define CMP    2
#define LEN    3

/* ISDIGIT differs from isdigit, as follows:
   - Its arg may be any int or unsigned int; it need not be an unsigned char.
   - It's guaranteed to evaluate its argument exactly once.
   - It's typically faster.
   POSIX says that only '0' through '9' are digits.  Prefer ISDIGIT to
   ISDIGIT_LOCALE unless it's important to use the locale's definition
   of `digit' even when the host does not conform to POSIX.  */
#define ISDIGIT(c) ((unsigned int) (c) - '0' <= 9)

/* Compare S1 and S2 as strings holding indices/version numbers,
   returning less than, equal to or greater than zero if S1 is less than,
   equal to or greater than S2 (for more info, see the texinfo doc).
*/

int strmverscmp (char *s1, char *s2)
{
  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;
  int state;
  int diff;

  /* Symbol(s)    0       [1-9]   others  (padding)
     Transition   (10) 0  (01) d  (00) x  (11) -   */
  static const unsigned int next_state[] =
  {
      /* state    x    d    0    - */
      /* S_N */  S_N, S_I, S_Z, S_N,
      /* S_I */  S_N, S_I, S_I, S_I,
      /* S_F */  S_N, S_F, S_F, S_F,
      /* S_Z */  S_N, S_F, S_Z, S_Z
  };

  static const int result_type[] =
  {
      /* state   x/x  x/d  x/0  x/-  d/x  d/d  d/0  d/-
                 0/x  0/d  0/0  0/-  -/x  -/d  -/0  -/- */

      /* S_N */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_I */  CMP, -1,  -1,  CMP,  1,  LEN, LEN, CMP,
                  1,  LEN, LEN, CMP, CMP, CMP, CMP, CMP,
      /* S_F */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_Z */  CMP,  1,   1,  CMP, -1,  CMP, CMP, CMP,
                 -1,  CMP, CMP, CMP
  };

  if (p1 == p2)
    return 0;

  c1 = *p1++;
  c2 = *p2++;
  /* Hint: '0' is a digit too.  */
  state = S_N | ((c1 == '0') + (ISDIGIT (c1) != 0));

  while ((diff = c1 - c2) == 0 && c1 != '\0')
    {
      state = next_state[state];
      c1 = *p1++;
      c2 = *p2++;
      state |= (c1 == '0') + (ISDIGIT (c1) != 0);
    }

  state = result_type[state << 2 | ((c2 == '0') + (ISDIGIT (c2) != 0))];

  switch (state)
    {
    case CMP:
      return diff;

    case LEN:
      while (ISDIGIT (*p1++))
	if (!ISDIGIT (*p2++))
	  return 1;

      return ISDIGIT (*p2) ? -1 : diff;

    default:
      return state;
    }
}

char* strmlwr(char*s) {
	register int counter = 0;
	unsigned int strlen = strmlen(s);
	char *ret = NULL;

	strmcat(&ret,s);

	for(;counter < strlen;++counter) {
		if(ret[counter]>='A' && ret[counter]<='Z')
			ret[counter]+=32;
	}
	return ret;
}


#endif
