#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
#include <libavutil/avutil.h>
}

 struct RawVideoFile{
    const char *filename;
    int width = 0;
    int height = 0;
    AVPixelFormat format;
} ;

struct RawVideoFrame{
    char *pixels = nullptr;
    int width = 0;
    int height = 0;
    AVPixelFormat format;
} ;

class FFmpegs
{
public:
    FFmpegs();

    static void convertRawVideo(const RawVideoFrame &in, RawVideoFrame &out);
    static void convertRawVideo(const RawVideoFile &in, RawVideoFile &out);
};

#endif // FFMPEGS_H
