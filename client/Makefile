CFLAGS += -g -DCLIENT -Wno-initializer-overrides -I/usr/X11R6/include -Iinclude
LDFLAGS += -lncurses -lX11 -L/usr/X11R6/lib -lGL -lGLU -lglut
exe := neverdarkc

obj-y := view.o tty.o main.o tee.o gl.o
obj-y += drivers/
