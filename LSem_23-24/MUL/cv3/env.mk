CFLAGS_FFMPEG=$$(pkg-config --cflags libavcodec libavformat libavutil libswscale)
LDLIBS_FFMPEG=$$(pkg-config --libs libavcodec libavformat libavutil libswscale)

CFLAGS=-std=c99 -Wall -Wextra -O2 $(CFLAGS_FFMPEG)
CXXFLAGS=-std=c++11 -Wall -Wextra -O2 $(CFLAGS_FFMPEG)
LDLIBS=$(LDLIBS_FFMPEG)
