/*
 *  main.c - of xlauncher main routine
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


#include <X11/Xlib.h>
#include <X11/Xutil.h>


#include "string.h"
#include "file.h"
#include "lists.h"
#include "output.h"



static list_t* newList;
const char *applicationsPath = "/usr/share/applications/";


int system(const char*);
inline int CmpFunc(void* s,void* t) {return strmverscmp((char*)s,(char*)t);};
inline void mfreeChar(void* s) {freeChar((char**)&s);};



XFontStruct *getFirstFoundEntry(Display *d, char *str) {
	int counter;
	char **list;
	XFontStruct *font;

	list = XListFonts(d, str, 1,&counter);
	font = XLoadQueryFont(d, list[0]);
	XFreeFontNames(list);
	return font;
}

inline int getWindowWidth(Display *d, Window *w) {
	int old_x;
	int old_y;
	unsigned int old_width;
	unsigned int old_height;
	unsigned int old_border_width;
	unsigned int old_depth;
	Window *root_window = NULL;

	XGetGeometry(d, *w, root_window, &old_x, &old_y, &old_width, &old_height, &old_border_width, &old_depth);
	return old_width;
}

static void handleFile(char* name, int unused) {
	char* progname = NULL;
	char* tmpFilename = NULL;
	FILEHANDLER actualFile;

	strmcat(&tmpFilename, (char*)applicationsPath);
	strmcat(&tmpFilename, name);

	if(openFileForRead(&actualFile, tmpFilename)) {
		char* out = readFromFile(&actualFile);

		if(out) {
			char* start = strmstr(out,"Exec=");

			if(strmlen(start) >= 6) {
				start += 5;
				*strmstr(start,"\n") = 0;
				strmcat(&progname, start);
				insert_list_element(newList, newList, progname);
			}
			freeChar(&out);
		}
		closeFile(&actualFile);
	}
	freeChar(&tmpFilename);
}

inline unsigned int searchInList(char *name, list_t* result) {
	int counter = 0;
	list_t* iterator = newList;

	while((iterator = iterator->next)) {
		if(!strmncmp(iterator->val, name, strmlen(name))) {
			insert_list_element(result, result, iterator->val);
			++counter;
		}
	}
	return counter;
}


int main(int argv, char **argc) {
	Display *display;				// $DISPLAY .x:1
	Window window;					// WINDOW
	XEvent event;					// EVENTHANDLING
	int screen;						// $DISPLAY .1:x
	XFontStruct *font;				// FIRST FOUND FONT
	char *msg = NULL;				// INPUT FROM KEYBOARD
	int loop = 1;					// EVENTLOOPBREAKER
	int window_width = 100;			// DEFAULT WINDOW SIZE
	int window_height = 11;			// DEFAULT WINDOW HEIGHT
	char *actualResult = NULL;		// ACTUAL PROGRAMM
	unsigned int resultOffset = 0;	// ACTUAL LIST OFFSET


//--- initial X11 system and draw simple window
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		outerr("Cannot open display\n");
		return 1;
	}

	screen = DefaultScreen(display);
	font = getFirstFoundEntry(display,"a*");

	XTextItem first;
	first.chars = msg;
	first.nchars = strmlen(msg);
	first.delta = 0;
	first.font = font->fid;

	window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, window_width, window_height, 1, BlackPixel(display, screen), WhitePixel(display, screen));
	XSelectInput(display, window, ExposureMask | StructureNotifyMask | KeyReleaseMask | KeymapStateMask | LeaveWindowMask | FocusChangeMask);
	XMapWindow(display, window);

//--- initial programm list
	newList = new_list();
	dirWalkR((char*)applicationsPath, handleFile);


//--- event loop
	while (loop) {
		int new_width = 0;

		XNextEvent(display, &event);
		switch(event.type) {
			case KeymapNotify:
				XRefreshKeyboardMapping(&event.xmapping);
				break;
			case Expose:
				new_width = XTextWidth(font, msg, first.nchars) + 1;
				break;
			case KeyRelease:
				{
					char string[25];
					int len;
					KeySym keysym;

					mallocate(string, 0, 25);

					len = XLookupString(&event.xkey, string, 25, &keysym, NULL);
					if (len > 0){
						if (keysym == XK_Return) {
							if(strmlen(msg) > 0) {
								loop = 0;
								if(actualResult!=NULL) {
									char *spacer = strmchr(actualResult, ' ');

									if(spacer)
										*spacer = 0;

									system(actualResult);
								}
							}
							break;
						}
						else if (keysym == XK_Escape) {
							loop = 0;
							break;
						}
						else if (keysym == XK_BackSpace) {
							int lastchar = strmlen(msg);

							if(lastchar != 0) {
								msg[lastchar - 1] = 0;
								--first.nchars;
							}
							new_width = XTextWidth(font, msg, first.nchars) + 1;
							XClearWindow(display, window);
						}
						else if (keysym == XK_Up) {
							++resultOffset;
							new_width = 1;
						}
						else if (keysym == XK_Down) {
							if(resultOffset != 0) {
								--resultOffset;
								new_width = 1;
							}
						}
						else {
							strmcat(&msg, string);
							first.chars = msg;
							first.nchars = strmlen(msg);
							resultOffset = 0;
							new_width = XTextWidth(font, msg, first.nchars) + 1;
						}
					}
				}
				break;
		}


// --- Handling input
		if(new_width != 0) {
			if(strmlen(msg) > 0) {
				int resultCounter;

				list_t* results;
				results = new_list();
				resultCounter = searchInList(msg, results);

				bubblesort_list(results,CmpFunc);

				if(resultCounter > 0) {
					list_t* iterator = results;
					actualResult = results->next->val;

					if(new_width>=100)
						XResizeWindow(display, window, new_width, window_height+12*resultCounter);
					else
						XResizeWindow(display, window, window_width, window_height+12*resultCounter);

///--- drawing search results
					XClearWindow(display, window);
					int counter = 1;
					while((iterator = iterator->next)) {
						first.chars = iterator->val;
						first.nchars = strmlen(iterator->val);

						if(first.nchars > strmlen(msg))
							XDrawText(display, window, DefaultGC(display, screen), 1, counter*11+11, &first, 1);
						++counter;
					}
				}
				else {
					actualResult = NULL;
					if(new_width >= 100)
						XResizeWindow(display, window, new_width, window_height);
					else
						XResizeWindow(display, window, window_width, window_height);
				}
				clear_list(results,NULL);
			}
			else {
					actualResult = NULL;
					if(new_width >= 100)
						XResizeWindow(display, window, new_width, window_height);
					else
						XResizeWindow(display, window, window_width, window_height);
			}

			first.chars = msg;
			first.nchars = strmlen(msg);
			XDrawText(display, window, DefaultGC(display, screen), 1, 10, &first, 1);
		}
	}

//--- CLEANUP
	clear_list(newList, mfreeChar);
	XFreeFont(display, font);
	XCloseDisplay(display);
	return 0;
}
