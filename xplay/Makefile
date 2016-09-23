.PHONY: portaudio

ifeq ($(OS),WINDOWS)
PLATFORM = WINDOWS
else
  ifeq ($(findstring Windows, $(OS)),Windows)
    PLATFORM = WINDOWS
  else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux) 
      PLATFORM = LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
      PLATFORM = DARWIN
    endif
  endif
endif

ifeq ($(PLATFORM),WINDOWS)
PORTAUDIO_CONFIG_FLAGS = --with-winapi=asio,wmme --with-asiodir=$(abspath ../../asiosdk)
EXTRA_LIBS = -lwinmm -lole32
EXTRA_CFLAGS = -static -static-libgcc
endif

ifeq ($(PLATFORM),DARWIN)
EXTRA_CFLAGS = -stdlib=libc++ -DOS_DARWIN=1
endif

portaudio:
	cd ../../portaudio; ./configure --prefix=$(abspath ../../xplay/xplay/libs/portaudio) $(PORTAUDIO_CONFIG_FLAGS)
	cd ../../portaudio; make
	cd ../../portaudio; make install

libsndfile:
	cd ../../libsndfile; ./configure --prefix=$(abspath ../../xplay/xplay/libs/libsndfile)
	cd ../../libsndfile; make
	cd ../../libsndfile; make install
		
bin/:
	mkdir -p bin/

xplay: | bin/
	g++ -ffast-math -g -O3 -std=c++11 -pthread $(EXTRA_CFLAGS)  -Ilibs/portaudio/include -Llibs/portaudio/lib -Ilibs/libsndfile/include -Llibs/libsndfile/lib  src/*.cpp -lm -lportaudio $(EXTRA_LIBS) -lsndfile -o bin/xplay

all: portaudio libsndfile xplay
	@echo "Build complete"

