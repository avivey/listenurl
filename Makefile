
TOOLPATH = i586-mingw32msvc-
CC = $(TOOLPATH)gcc

CFLAGS = -mno-cygwin -Wall -O2 -D_WINDOWS -DDEBUG -DWIN32S_COMPAT \
		 -D_NO_OLDNAMES -DNO_MULTIMON -DNO_HTMLHELP

# probably not needed
# RCFLAGS = $(RCINC) --define WIN32=1 --define _WIN32=1 --define WINVER=0x0400
# RCFLAGS += $(patsubst -D%,--define %,$(VER))
# CFLAGS += -D_WIN32_IE=0x0500
# CFLAGS += -DWINVER=0x0500 -D_WIN32_WINDOWS=0x0410 -D_WIN32_WINNT=0x0500

trayicon.exe: listenurl.exe
	cp listenurl.exe $@
listenurl.exe: trayicon.o listenurl.o
	$(CC) -mwindows -Wl -o $@ $^

trayicon.o: trayicon.h
listenurl.o: trayicon.h

clean:
	rm -f *.o *.exe *.res.o *.gch
