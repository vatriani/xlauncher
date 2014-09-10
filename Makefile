FLAGS:=-pipe -fstack-protector -O5 -fdata-sections -ffunction-sections -Wl,--gc-sections --param=ssp-buffer-size=4 -fexcess-precision=fast -DDATE="\"`date +'%Y'`\""
SRCS=string.o file.o output.o lists.o
TARGET=xlauncher

all: $(TARGET)

xlauncher: $(SRCS) main.c
	gcc $(FLAGS) $(SRCS) -lX11 -o $(TARGET) main.c

clean:
	rm -f *.d *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/bin/$(TARGET)
	chmod go+rx /usr/bin/$(TARGET)
