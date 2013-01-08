
TOOLPATH = i586-mingw32msvc-
CC = $(TOOLPATH)cc

CFLAGS = -mno-cygwin -Wall -Werror -O2 -D_WINDOWS \
		 -D_NO_OLDNAMES -DNO_MULTIMON -DNO_HTMLHELP \
		 -DWINVER=0x0501 -D_WIN32_WINNT=0x0501


LIBS = -lws2_32  # WinSockets

# to debug, comment out next line
DEBUG_MODE =
PROD_MODE = -mwindows

# MODE = $(PRODMODE)
MODE = $(DEBUG_MODE)

# probably not needed
# RCFLAGS = $(RCINC) --define WIN32=1 --define _WIN32=1 --define WINVER=0x0400
# RCFLAGS += $(patsubst -D%,--define %,$(VER))
# CFLAGS += -D_WIN32_IE=0x0500
# CFLAGS += -D_WIN32_WINDOWS=0x0410

listenurl.exe: trayicon.o listenurl.o network.o
	$(CC) $(MODE) -o $@ $^ $(LIBS)

netdemo.o: network.h
listenurl.o: trayicon.h network.h
trayicon.o: trayicon.h
network.o: network.h port-def.h

clean:
	rm -f *.o *.exe *.res.o
