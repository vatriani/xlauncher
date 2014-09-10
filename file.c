/*
 * file.c - for file, config and cache handling
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
#include "string.h"
#endif

#ifndef __OUTPUT_H__
#include "output.h"
#endif

#include "file.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <pwd.h>
#include <stdlib.h>
#include <fcntl.h>



void getUserPath(char **path) {
	struct passwd *userInformations;
	int len;

	userInformations = getpwuid(getuid());
	len = strmlen(userInformations->pw_dir);
	*path = malloc(++len);
	memcpy(*path, userInformations->pw_dir, len+1);
}



void createPathIfNotExists(char **path) {
	DIR *tmp;
	tmp = opendir(*path);
	if(tmp == NULL)
		mkdir(*path, 0700);
	else
		closedir(tmp);
}



void getConfigHome(char **path) {
	char *configHome = getenv("XDG_CONFIG_HOME");
	if(configHome)
		strmcat(path,configHome);
	else {
		getUserPath(path);
		strmcat(path,"/.config");
	}
}



void getConfigApp(char **path, char *app_name) {
	getConfigHome(path);
	if(*path) {
		strmcat(path, "/");
		strmcat(path, app_name);
	}
}



void getCacheHome(char **path) {
	char *cacheHome = getenv("XDG_CACHE_HOME");
	if(cacheHome)
		strmcat(path,cacheHome);
	else {
		getUserPath(path);
		strmcat(path,"/.cache");
	}
}



void getCacheApp(char **path, char *app_name) {
	getCacheHome(path);
	if(*path) {
		strmcat(path,"/");
		strmcat(path,app_name);
	}
}



void getCachePath(char **path, char *app_name) {
	getCacheApp(path,app_name);
	if(*path) {
		createPathIfNotExists(path);
	}
}



void openFileForWrite(FILEHANDLER *file, char* name) {
	*file = open(name,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);

	if(*file == 0)
		outerr("error by opening file");
}


int openFileForRead(FILEHANDLER *file, char* name) {
	*file = open(name, O_RDONLY);

	if(*file == 0) {
		outerr("error by opening file");
		return 0;
	}
	return 1;
}



void closeFile(FILEHANDLER *file) {
	if(close(*file) != 0 && !file)
		outerr("error by file closing\n");
}



void writeToFile(FILEHANDLER *file, char* bytes) {
	if(write(*file,bytes,strmlen(bytes)) <= 0)
		outerr("error by file writing\n");
}

void writeNToFile(FILEHANDLER *file, char* bytes, unsigned int len) {
	if(write(*file,bytes,len) <= 0)
		outerr("error by file writing\n");
}

char* readFromFile(FILEHANDLER *file) {
	char* returnString = NULL;
	char buffer[BUFSIZ+1];
	int actualSize;


	while((actualSize = read(*file, (void*)buffer, BUFSIZ))) {
		if(actualSize == -1) {
			outerr("couldn't read from file\n");
			return NULL;
		}
		buffer[actualSize ] = 0;
		strmcat(&returnString, buffer);
	}

	return returnString;
}



void dirWalk(char *dir, void (*fcn)(char *))
{
	char *name = NULL;
	struct dirent *dp;
	DIR *dfd;

	if ((dfd = opendir(dir)) == NULL) {
		outerr("dirWalk: can't open");
		return;
	}

	while ((dp = readdir(dfd)) != NULL) {
		if (strmcmp(dp->d_name, ".") == 0 || strmcmp(dp->d_name, "..") == 0)
			continue;

		if (strmlen(dir) + strmlen(dp->d_name)+1 > MAX_PATH)
			outerr("name too long\n");
		else {
			char* separator = strmrchr(dir,'/');

			if(&dir[strmlen(dir)-1] != separator){
				strmcat(&name, dir);
				strmcat(&name, "/");
			}

			strmcat(&name, dp->d_name);
			(*fcn)(name);
			freeChar(&name);
		}
	}
	closedir(dfd);
}



void dirWalkR(char *dir, void (*fcn)(char *,int))
{
	char *name = NULL;
	struct dirent *dp;
	DIR *dfd;

	if ((dfd = opendir(dir)) == NULL) {
		outerr("dirWalk: can't open");
		return;
	}

	while ((dp = readdir(dfd)) != NULL) {
		if (strmcmp(dp->d_name, ".") == 0 || strmcmp(dp->d_name, "..") == 0)
			continue;

		if (strmlen(dir)+strmlen(dp->d_name)+1 > MAX_PATH)
			outerr("name too long\n");
		else {
			char* separator = strmrchr(dir,'/');

			if(&dir[strmlen(dir)-1] != separator){
				strmcat(&name, dir);
				strmcat(&name, "/");
			}

			strmcat(&name, dp->d_name);

			if(dp->d_type == DT_DIR) {
				(*fcn)(name,0);
				dirWalkR(name, fcn);
			} else
				(*fcn)(name,1);

			freeChar(&name);
		}
	}
	closedir(dfd);
}
