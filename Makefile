TOOLPATH = i586-mingw32msvc-
CC = $(TOOLPATH)cc

CFLAGS  = -Wall -Werror -O2
CFLAGS += -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -DUNICODE -D_UNICODE

LIBS = -lws2_32  # WinSockets

# remove -mwindows to have an stdout.
LINK_FLAGS = -mwindows

listenurl.exe: trayicon.o listenurl.o network.o
	$(CC) $(LINK_FLAGS) -o $@ $^ $(LIBS)

listenurl.o: trayicon.h network.h
trayicon.o: trayicon.h
network.o: network.h

clean:
	rm -f *.o *.exe
