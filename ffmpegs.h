#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
#include <libavutil/avutil.h>
}

typedef struct {
    const char *filename;
    int width = 0;
    int height = 0;
    AVPixelFormat format;
} RawVideoFile;

typedef struct {
    char *pixels = nullptr;
    int width = 0;
    int height = 0;
    AVPixelFormat format;
} RawVideoFrame;

class FFmpegs
{
public:
    FFmpegs();

    static void convertRawVideo(const RawVideoFrame &in, RawVideoFrame &out);
    static void convertRawVideo(const RawVideoFile &in, RawVideoFile &out);
};

#endif // FFMPEGS_H
