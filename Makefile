PROJECT = play-torrent
CXX = g++
CXXFLAGS =  -Wall `pkg-config --cflags libtorrent-rasterbar`
LDFLAGS = `pkg-config --libs libtorrent-rasterbar` -s
RM = rm -f

all: $(PROJECT)

$(PROJECT): torrent.cpp
        $(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
        $(RM) $(PROJECT)
