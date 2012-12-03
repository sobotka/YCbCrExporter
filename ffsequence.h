#ifndef FFSEQUENCE_H
#define FFSEQUENCE_H

#define AV_LOG_VERBOSE
// Needed by FFMPEG to avoid "error: ‘UINT64_C’ was not declared in this scope"
#define                           __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

#include <stdexcept>
#include <vector>

// The following correspond to the data channels. Defined here for readability.
#define FF_Y                                                0
#define FF_CB                                               1
#define FF_CR                                               2

#define FF_FIRST_FRAME                                      1
#define FF_NO_FRAME                                        -1
#define FF_NO_STREAM                                       -1
#define FF_NO_DIMENSION                                    -1

// Custom error codes generated from within this library.
#define FFERROR_NO_VIDEO_STREAM                             100
#define FFERROR_BAD_FORMAT                                  101
#define FFERROR_NO_DECODER                                  102
#define FFERROR_ALLOC_ERROR                                 103
#define FFERROR_BAD_FILENAME                                104

class ffAVPacket : public AVPacket
{
public:
    ffAVPacket():AVPacket() {}
};

class ffError : public std::runtime_error
{
private:
    int                                 m_ffError;
public:
    ffError(const std::string& what_arg, int error) :
        runtime_error(what_arg), m_ffError(error) {}
    int getError(void) { return m_ffError; }
};

class ffmpegError : public ffError
{
public:
    ffmpegError(const std::string& what_arg, int error) :
        ffError(what_arg, error) {}
};

class ffSize
{
public:
    ffSize(long w, long h): m_width(w), m_height(h) { }
    long                                    m_width;
    long                                    m_height;
};

class ffRawFrame
{
private:

public:
    unsigned char                  *m_pY;
    unsigned char                  *m_pCb;
    unsigned char                  *m_pCr;

    ffRawFrame(AVFrame*);
    ~ffRawFrame();
};

class ffSequence
{
private:
    AVFormatContext                        *m_pFormatCtx;
    AVCodecContext                         *m_pCodecCtx;
    AVCodec                                *m_pCodec;

    long                                    m_totalFrames;
    long                                    m_currentFrame;
    ffSize                                  m_lumaSize;
    ffSize                                  m_chromaSize;
    int                                     m_stream;
    static bool                             m_isInitialized;
    std::vector<ffRawFrame*>                m_frames;
    bool                                    m_isValid;

    void initialize(void);
    void pushRawFrame(AVFrame*);
    void freeRawFrames(void);
    void cleanup(void);
public:
    ffSequence(void);
    ~ffSequence();

    void openFile(char*);
    ffRawFrame* getRawFrame(long);
    ffRawFrame* setCurrentFrame(long);
    long getCurrentFrame(void);
    long getTotalFrames(void);
    ffSize getLumaSize(void);
    ffSize getChromaSize(void);
    bool isValid(void);
    void scalePlane(void);
};

class ffRawFrameFloatScaled
{
private:
public:
    float                          *m_pfY;
    float                          *m_pfCb;
    float                          *m_pfCr;

    ffRawFrameFloatScaled(ffRawFrame*, ffSequence*);
    ~ffRawFrameFloatScaled();
};
#endif // FFSEQUENCE_H
