xlauncher
=========

a very simple programm launcher for linux, based only on low-lewel glib and X11-lib
it doesn't runs in background

# compile:
make

# run:
./xlauncher

- type the application name in the window and this application will be displayed from /usr/share/applications/*.desktop
- hit enter for execute the application
- hit escape quits xlauncher
- click outside window exit launcher

# configuration example:
for openbox in rc.xml file

adding line for hotkey
```xml
<keybind key="W-space"><action name="Execute"><command>xlauncher</command></action></keybind>
```

adding for no window decoration and place on (0|0) on screen
```xml
<application title="xlauncher">
	<decor>no</decor>
	<focus>yes</focus>
	<position force="yes">
		<x>0</x>
		<y>0</y>
	</position>
</application>
```
