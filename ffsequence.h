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

#include "OpenImageIO/imageio.h"
OIIO_NAMESPACE_USING


#define FF_FIRST_FRAME                                      1

//#define FF_NO_FRAME                                        -1
//#define FF_NO_STREAM                                       -1
#define FF_NO_DIMENSION                                    -1

/******************************************************************************
 * Forward Declarations
 ******************************************************************************/
class ffSequence;

/******************************************************************************
 * ffDefault
 ******************************************************************************/
class ffDefault
{
public:
    enum Defaults
    {
        NoFrame =                                              -1,
        NoStream =                                             -1,
        NoDimension =                                          -1,
        FirstFrame =                                            1
    };
};

/******************************************************************************
 * ffInterpolator
 ******************************************************************************/
class ffInterpolator
{
public:
    enum Type
    {
        Nearest, Linear, Cubic, Prefilter
    };
};

/******************************************************************************
 * ffAVPacket
 ******************************************************************************/
class ffAVPacket : public AVPacket
{
public:
    ffAVPacket():AVPacket() {}
};

/******************************************************************************
 * ffError
 ******************************************************************************/
class ffError : public std::runtime_error
{
public:
    enum FFError
    {
        ERROR_NO_VIDEO_STREAM,
        ERROR_BAD_FORMAT,
        ERROR_NO_DECODER,
        ERROR_ALLOC_ERROR,
        ERROR_BAD_FILENAME,
        ERROR_NULL_FILENAME,
        ERROR_BAD_TRIM,
        ERROR_BAD_FRAME
    };

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

/******************************************************************************
 * ffTrim
 ******************************************************************************/
class ffTrim
{
public:
    long                                    m_in;
    long                                    m_out;

    ffTrim(long, long);
};

/******************************************************************************
 * ffSize
 ******************************************************************************/
class ffSize
{
public:
    long                                    m_width;
    long                                    m_height;

    ffSize(long, long);
};

/******************************************************************************
 * ffSizeRatio
 ******************************************************************************/
class ffSizeRatio
{
public:
    ffSizeRatio(ffSize, ffSize);
    float                                   m_widthRatio;
    float                                   m_heightRatio;
};

/******************************************************************************
 * ffRawFrame
 ******************************************************************************/
class ffRawFrame
{
private:

public:
    enum Plane
    {
        Y =                         0,
        Cb =                        1,
        Cr =                        2
    };

    unsigned char                  *m_pY;
    unsigned char                  *m_pCb;
    unsigned char                  *m_pCr;

    ffRawFrame(AVFrame*);
    ~ffRawFrame();

    void scalePlane(ffRawFrame::Plane, ffSize, ffSizeRatio,
                    ffInterpolator::Type);
};

/******************************************************************************
 * ffRawFrameFloat
 ******************************************************************************/
class ffRawFrameFloat
{
private:
public:
    float                          *m_pfY;
    float                          *m_pfCb;
    float                          *m_pfCr;

    ffRawFrameFloat(long);
    ~ffRawFrameFloat();
};

/******************************************************************************
 * ffViewer
 ******************************************************************************/
class ffViewer
{
public:
    enum ViewerPlane
    {
        Y =                                 ffRawFrame::Y,
        Cb =                                ffRawFrame::Cb,
        Cr =                                ffRawFrame::Cr,
        RGB
    };
};

/******************************************************************************
 * ffExportDetails
 ******************************************************************************/
class ffExportDetails
{
public:
    enum ExportPlane
    {
        RGB =                               0,
        YCbCr =                             1,
        Raw =                               2
    };

private:
    ffSize                                  m_exportSize;
    ffInterpolator::Type                    m_YInterp;
    ffInterpolator::Type                    m_CbInterp;
    ffInterpolator::Type                    m_CrInterp;
    ffTrim                                  m_trim;
    ExportPlane                             m_exportPlanes;

public:
    ffExportDetails(void);
    void deinit(void);

    ffSize getExportSize(void);
    ffInterpolator::Type getYInterp(void);
    ffInterpolator::Type getCbInterp(void);
    ffInterpolator::Type getCrInterp(void);
    ffTrim getTrim(void);
    ExportPlane getExportPlane(void);

    void setExportSize(ffSize);
    void setYInterp(ffInterpolator::Type);
    void setCbInterp(ffInterpolator::Type);
    void setCrInterp(ffInterpolator::Type);
    void setTrim(long, long);
    void setExportPlane(ExportPlane);
};

/******************************************************************************
 * ffSequence
 ******************************************************************************/
class ffSequence
{
public:
    enum ffSequenceState
    {
        isLoading,
        isValid,
        isInvalid,
        justLoading,
        justOpened,
        justClosed,
        justErrored
    };

private:
    AVFormatContext                        *m_pFormatCtx;
    AVCodecContext                         *m_pCodecCtx;
    AVCodec                                *m_pCodec;

    ffExportDetails                        *m_pExportDetails;

    long                                    m_totalFrames;
    long                                    m_currentFrame;
    ffSize                                  m_lumaSize;
    ffSize                                  m_chromaSize;

    int                                     m_stream;
    static bool                             m_isInitialized;
    std::vector<ffRawFrame*>                m_frames;
    std::vector<ffRawFrameFloat*>           m_framesFloat;
    ffSequenceState                         m_state;

    std::string                             m_fileURI;

    void initialize(void);
    void pushRawFrame(AVFrame*);
    void freeRawFrames(void);
    void cleanup(void);
public:
    ffSequence(void);
    ~ffSequence();

    void readFile(char *fileName);
    void writeFile(char *, long, long);
    void closeFile(void);

    ffRawFrame* getRawFrame(long);
    ffRawFrame* setCurrentFrame(long, void *);
    long getCurrentFrame(void);
    long getTotalFrames(void);
    void setExportTrim(long, long, void *);
    void setExportTrimIn(long, void *);
    void setExportTrimOut(long, void *);
    void setExportPlane(ffExportDetails::ExportPlane, void *);
    void setExportDimensions(long, long, void*);
    void resetExportTrim(void *);
    void resetExportTrimIn(void *);
    void resetExportTrimOut(void *);

    ffTrim getExportTrim(void);
    ffExportDetails::ExportPlane getExportPlane(void);
    ffSize getExportDimensions(void);

    ffSize getLumaSize(void);
    ffSize getChromaSize(void);
    ffSequenceState getState(void);
    std::string getFileURI(void);

    // The following virtual functions are provided for GUI applications to
    // provide a simple way to keep the UI in synchronicity with the object.
    virtual void onProgressStart(void);
    virtual void onProgress(double);
    virtual void onProgressEnd(void);
    virtual void onJustLoading(void);
    virtual void onJustOpened(void);
    virtual void onJustClosed(void);
    virtual void onJustErrored(void);
    // The following are events generated via set* functions. Beware, all
    // objects that set should include themselves in the sender and assert
    // that correct action is taken to avoid recursive loops.
    virtual void onExportTrimChanged(long, long, void *);
    virtual void onExportPlaneChanged(ffExportDetails::ExportPlane, void *);
    virtual void onExportDimensionsChanged(long, long, void*);
    virtual void onFrameChanged(long, void *);
};
#endif // FFSEQUENCE_H
