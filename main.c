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
#include <X11/Xresource.h>
#include <X11/Xos.h>
#include <X11/keysymdef.h>

#include "string.h"
#include "file.h"
#include "lists.h"
#include "output.h"


static list_t* newList;
const char *applicationsPath = "/usr/share/applications/";

typedef struct programm_t{
	char* exec;
	char* name;
} programm;

int system(const char*);
inline int CmpFunc(void* s,void* t) {return strmverscmp(((programm*)s)->name, ((programm*)t)->name);};


inline void mfree(void* s) {
	freeChar((char**)&((programm*)s)->exec);
	freeChar((char**)&((programm*)s)->name);
	free(s);
};



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

	openFileForRead(&actualFile, tmpFilename);

	if(actualFile) {
		char* out = readFromFile(&actualFile);

		if(out) {
			programm *entry;
			entry = malloc(sizeof(programm));
			entry->exec = NULL;
			entry->name = NULL;

			char* exec = strmstr(out,"Exec=");
			char* name = strmstr(out,"Name=");

			if(strmlen(exec) > 6 && strmlen(name) > 6) {
				if(!entry)
					outerr("couldN't malloc\n");

				exec += 5;
				char *end = strmchr(exec, '\n');

				if(end)
					*end = 0;

				name += 5;
				end = strmchr(name, '\n');

				if(end)
					*end = 0;

				strmcat(&(entry->exec), exec);
				strmcat(&(entry->name), name);

				insert_list_element(newList, newList, (void*)entry);
			}
			freeChar(&out);
		}
		closeFile(&actualFile);
	}
	freeChar(&tmpFilename);
}

inline unsigned int searchInList(char *name, list_t* result) {
	register int counter = 0;
	list_t* iterator = newList;
	programm *prog = NULL;

	while((iterator = iterator->next)) {
		prog = ((programm*)iterator->val);

		if(!strmncmp(strmlwr(prog->name), name, strmlen(name))) {
			insert_list_element(result, result, (void*)prog);
			++counter;
		}
	}
	return counter;
}


int main(int argv, char **argc) {
	static Display *display;				// $DISPLAY .x:1
	static Window window;					// WINDOW
	XEvent event;							// EVENTHANDLING
	int screen;								// $DISPLAY .1:x
	XFontStruct *font;						// FIRST FOUND FONT
	char *msg = NULL;						// INPUT FROM KEYBOARD
	register int loop = 1;					// EVENTLOOPBREAKER
	int window_width = 100;					// DEFAULT WINDOW SIZE
	int window_height = 11;					// DEFAULT WINDOW HEIGHT
	programm *actualResult = NULL;			// ACTUAL PROGRAMM
	static unsigned int resultOffset = 0;	// ACTUAL LIST OFFSET
	XWMHints *wm_hints;
	XClassHint *class_hints;
	XTextProperty windowName, iconName;
	Pixmap icon_pixmap;
	const char *window_name = "xlauncher";
	const char *icon_name = "basicwin";


//--- initial X11 system and draw simple window
	display = XOpenDisplay(NULL);
	if (!display) {
		outerr("Cannot open display\n");
		return 1;
	}

	screen = DefaultScreen(display);
	font = getFirstFoundEntry(display,"a*");

	if (!(wm_hints = XAllocWMHints())) return 1;
	if (!(class_hints = XAllocClassHint())) return 1;

	XStringListToTextProperty((char**)&window_name, 1, &windowName);
	XStringListToTextProperty((char**)&icon_name, 1, &iconName);

	wm_hints->initial_state = NormalState;
	wm_hints->input = True;
	wm_hints->flags = StateHint | InputHint;

	class_hints->res_name = (char*)window_name;
	class_hints->res_class = (char*)icon_name;

	XTextItem first;
	first.chars = msg;
	first.nchars = strmlen(msg);
	first.delta = 0;
	first.font = font->fid;

	int blackColor = BlackPixel(display, screen);
	int whiteColor = WhitePixel(display, screen);

	window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, window_width, window_height, 1, blackColor, whiteColor);
	XSelectInput(display, window, ExposureMask | KeyPressMask | FocusChangeMask);

	XSetWMHints(display,window,wm_hints);
	XSetWMName(display,window,&windowName);

	XMapWindow(display, window);

//--- initial programm list
	newList = new_list();
	dirWalkR((char*)applicationsPath, handleFile);
//--- event loop
	while (loop) {
		int new_width = 0;

		XNextEvent(display, &event);
		switch(event.type) {
			case Expose:
				new_width = XTextWidth(font, msg, first.nchars) + 1;
				break;
			case FocusOut:
				loop = 0;
				break;
			case KeyPress:
				{
					KeySym keysym;
					int index;

					keysym = XLookupKeysym(&event.xkey, index);
					if (keysym == XK_Return) {
						if(strmlen(msg) > 0) {
							loop = 0;
							if(actualResult) {
								char *spacer = strmchr(actualResult->exec, '%');

								if(spacer)
									*spacer = 0;
								strmcat(&actualResult->exec, " &");
								system(actualResult->exec);
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
						if(resultOffset!=0) {
							--resultOffset;
							new_width = 1;
						}
					}
					else if (keysym == XK_Down) {
						++resultOffset;
						new_width = 1;
					}
					else {
						char *newstr = XKeysymToString(keysym);
						if(strmlen(newstr)==1) {
							strmcat(&msg, newstr);
							first.chars = msg;
							first.nchars = strmlen(msg);
							resultOffset = 0;
							new_width = XTextWidth(font, msg, first.nchars) + 1;
						}
					}
				}
				break;
		}


// --- Handling input and drawing
		if(new_width != 0) {
			if(strmlen(msg) > 0) {
				int resultCounter;

				list_t* results;
				results = new_list();
				resultCounter = searchInList(msg, results);

				if(resultOffset >= resultCounter)
						resultOffset = resultCounter - 1;

				bubblesort_list(results,CmpFunc);

				if(resultCounter > 0) {
					list_t* iterator = results;
					actualResult = ((programm*)results->next->val);

///--- drawing search results
					XClearWindow(display, window);
					register int counter = 1;
					while((iterator = iterator->next)) {
						first.chars = ((programm*)iterator->val)->name;
						first.nchars = strmlen(((programm*)iterator->val)->name);

						if(resultOffset == counter - 1) {
							XDrawRectangle(display, window, DefaultGC(display, screen), 0, resultOffset*11 + 11, window_width, 11);
							actualResult=((programm*)iterator->val);
						}

						if((XTextWidth(font, msg, first.nchars) + 1) > new_width)
							new_width = XTextWidth(font, msg, first.nchars) + 1;

						if(first.nchars >= strmlen(msg))
							XDrawText(display, window, DefaultGC(display, screen), 1, counter*11+11, &first, 1);
						++counter;
					}

					if(new_width >= 100)
						XResizeWindow(display, window, new_width, window_height + 11 * resultCounter);
					else
						XResizeWindow(display, window, window_width, window_height + 11 * resultCounter);
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
		XFlush(display);
	}

//--- CLEANUP
	clear_list(newList, mfree);
	XFreeFont(display, font);
	XCloseDisplay(display);
	return 0;
}
