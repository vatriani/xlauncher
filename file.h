/*
 * file.h - for file, config and cache handling
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

#ifndef __FILE_H__
#define __FILE_H__


#ifndef BUFSIZ
#define	BUFSIZ	SSIZE_MAX
#endif

#ifndef EOF
#define	EOF	(-1)
#endif

#define MAX_NAME NAME_MAX
#define MAX_PATH PATH_MAX
#ifndef	DIRSIZ
#define	DIRSIZ	14
#endif


typedef int FILEHANDLER;
extern __uid_t getuid (void) __THROW;


void getUserPath(char **path);
void createPathIfNotExists(char **path);
void getConfigHome(char **path);
void getConfigApp(char **path, char *app_name);
void getCacheHome(char **path);
void getCacheApp(char **path, char *app_name);
void getCachePath(char **path, char *app_name);
void openFileForWrite(FILEHANDLER *file, char* name);
int openFileForRead(FILEHANDLER *file, char* name);
void closeFile(FILEHANDLER *file);
void writeToFile(FILEHANDLER *file, char* bytes);
void writeNToFile(FILEHANDLER *file, char* bytes, unsigned int len);
char* readFromFile(FILEHANDLER *file);
void dirWalk(char *dir, void (*fcn)(char *));
void dirWalkR(char *dir, void (*fcn)(char *,int));

#endif
