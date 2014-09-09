FLAGS:=-Wall -march=atom -mtune=atom -ffast-math -pipe -Os -fstack-protector --param=ssp-buffer-size=4 -fexcess-precision=fast -DDATE="\"`date +'%Y'`\""
SRCS=string.o file.o output.o lists.o
TARGET=launcher

all: $(TARGET)

launcher: $(SRCS) main.c
	gcc $(FLAGS) $(SRCS) -lX11 -o $(TARGET) main.c

clean:
	rm -f *.d *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/bin/$(TARGET)
	chmod go+rx /usr/bin/$(TARGET)
