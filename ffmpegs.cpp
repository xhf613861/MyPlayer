#include "ffmpegs.h"
#include <QDebug>
#include <QFile>

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#define ERR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof(errbuf))

#define END(func) \
    if (ret < 0) { \
        ERR_BUF(ret); \
        qDebug() << #func << " error " << errbuf; \
        goto end; \
    }

FFmpegs::FFmpegs()
{

}

void FFmpegs::convertRawVideo(const RawVideoFrame &in, RawVideoFrame &out)
{
    SwsContext *ctx = nullptr;

    // 输入、输出缓冲区（指向每一个平面的数据）
    uint8_t *inData[4], *outData[4];
    // 每一个平面的大小
    int inStrides[4], outStrides[4];
    int ret = -1;
    int inFramSize, outFramSize;

    ctx = sws_getContext(in.width, in.height, in.format,
                         out.width, out.height, out.format,
                         SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!ctx) {
        qDebug() << "sws_getContext error";
        goto end;
    }

    ret = av_image_alloc(inData, inStrides, in.width, in.height, in.format, 1);
    END(av_image_alloc);
    ret = av_image_alloc(outData, outStrides, out.width, out.height, out.format, 1);
    END(av_image_alloc);

    inFramSize = av_image_get_buffer_size(in.format, in.width, in.height, 1);
    outFramSize = av_image_get_buffer_size(out.format, out.width, out.height, 1);

    // 拷贝输入
    memcpy(inData[0], in.pixels, inFramSize);

    // 转换
    sws_scale(ctx, inData, inStrides, 0, in.height,
              outData, outStrides);
    // 写到输出文件
    out.pixels = (char*)malloc(outFramSize);
    memcpy(out.pixels, outData[0], outFramSize);

end:
    av_freep(&inData[0]);
    av_freep(&outData[0]);
    sws_freeContext(ctx);
}

void FFmpegs::convertRawVideo(const RawVideoFile &in, RawVideoFile &out)
{
    SwsContext *ctx = nullptr;

    // 输入、输出缓冲区（指向每一个平面的数据）
    uint8_t *inData[4], *outData[4];
    // 每一个平面的大小
    int inStrides[4], outStrides[4];
    int ret = -1;
    int inFramSize, outFramSize;

    QFile infile(in.filename);
    QFile outfile(out.filename);

    if (!infile.open(QFile::ReadOnly)) {
        qDebug() << "file open error in";
        goto end;
    }

    if (!outfile.open(QFile::WriteOnly)) {
        qDebug() << "file open error outfile";
        goto end;
    }

    ctx = sws_getContext(in.width, in.height, in.format,
                         out.width, out.height, out.format,
                         SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!ctx) {
        qDebug() << "sws_getContext error";
        goto end;
    }

    ret = av_image_alloc(inData, inStrides, in.width, in.height, in.format, 1);
    END(av_image_alloc);
    ret = av_image_alloc(outData, outStrides, out.width, out.height, out.format, 1);
    END(av_image_alloc);

    inFramSize = av_image_get_buffer_size(in.format, in.width, in.height, 1);
    outFramSize = av_image_get_buffer_size(out.format, out.width, out.height, 1);
    while (infile.read((char *)inData[0], inFramSize) == inFramSize) {
        sws_scale(ctx, inData, inStrides, 0, in.height,
                  outData, outStrides);
        outfile.write((char *)outData[0], outFramSize);
    }

end:
    infile.close();
    outfile.close();
    av_freep(&inData[0]);
    av_freep(&outData[0]);
    sws_freeContext(ctx);
}
