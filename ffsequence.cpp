/*******************************************************************************
YCbCr Lab
A tool to aid filmmakers / artists to manipulate YCbCr files to
maximize quality.
Copyright (C) 2013 Troy James Sobotka, troy.sobotka@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "ffsequence.h"
#include <stdexcept>
#include <math.h>

/******************************************************************************
 * ffTrim
 ******************************************************************************/
ffTrim::ffTrim(long in, long out) : m_in(in), m_out(out)
{

}

/******************************************************************************
 * ffSizeRatio
 ******************************************************************************/
ffSize::ffSize(long w, long h) : m_width(w), m_height(h)
{

}

/******************************************************************************
 * ffSizeRatio
 ******************************************************************************/
ffSizeRatio::ffSizeRatio(ffSize src, ffSize dst) :
    m_widthRatio(0), m_heightRatio(0)
{
    m_widthRatio = (float)src.m_width / (float)dst.m_width;
    m_heightRatio = (float)src.m_height / dst.m_height;
}

/******************************************************************************
 * ffExportDetails
 ******************************************************************************/
ffExportDetails::ffExportDetails(void) :
    m_exportSize(ffDefault::NoDimension,ffDefault::NoDimension),
    m_YInterp(ffInterpolator::Nearest),
    m_CbInterp(ffInterpolator::Nearest),
    m_CrInterp(ffInterpolator::Nearest),
    m_trim(ffDefault::NoFrame,ffDefault::NoFrame),
    m_exportPlanes(ffExportDetails::RGB)
{
}

void ffExportDetails::deinit(void)
{
    m_exportSize.m_width = 0;
    m_exportSize.m_height = 0;
    m_YInterp = ffInterpolator::Nearest;
    m_CbInterp = ffInterpolator::Nearest;
    m_CrInterp = ffInterpolator::Nearest;
    m_trim.m_in = ffDefault::NoFrame;
    m_trim.m_out = ffDefault::NoFrame;
}

ffSize ffExportDetails::getExportSize(void)
{
    return m_exportSize;
}

ffInterpolator::Type ffExportDetails::getYInterp(void)
{
    return m_YInterp;
}

ffInterpolator::Type ffExportDetails::getCbInterp(void)
{
    return m_CbInterp;
}

ffInterpolator::Type ffExportDetails::getCrInterp(void)
{
    return m_CrInterp;
}

ffTrim ffExportDetails::getTrim(void)
{
    return m_trim;
}

ffExportDetails::ExportPlane ffExportDetails::getExportPlane()
{
    return m_exportPlanes;
}

void ffExportDetails::setExportSize(ffSize size)
{
    m_exportSize.m_width = size.m_width;
    m_exportSize.m_height = size.m_height;
}

void ffExportDetails::setYInterp(ffInterpolator::Type yType)
{
    m_YInterp = yType;
}

void ffExportDetails::setCbInterp(ffInterpolator::Type cbType)
{
    m_CbInterp = cbType;
}

void ffExportDetails::setCrInterp(ffInterpolator::Type crType)
{
    m_CrInterp = crType;
}

void ffExportDetails::setTrim(long in, long out)
{
    m_trim.m_in = in;
    m_trim.m_out = out;
}

void ffExportDetails::setExportPlane(ffExportDetails::ExportPlane plane)
{
    m_exportPlanes = plane;
}

/******************************************************************************
 * ffRawFrame
 ******************************************************************************/
ffRawFrame::ffRawFrame(AVFrame* pFrame) :
    m_pY(NULL), m_pCb(NULL), m_pCr(NULL)
{
    int y_width         = pFrame->width;
    int y_height        = pFrame->height;
    int c_hshift        = av_pix_fmt_descriptors[pFrame->format].log2_chroma_h;
    int c_wshift        = av_pix_fmt_descriptors[pFrame->format].log2_chroma_w;

    // Instead of making assumptions, and likely keeping this code more
    // flexibile, we do the correct approach according to FFMPEG and bit shift
    // the luma width and height.
    int c_width         = y_width >> c_hshift;
    int c_height        = y_height >> c_wshift;

    int y_stride        = pFrame->linesize[ffRawFrame::Y];
    int cb_stride       = pFrame->linesize[ffRawFrame::Cb];
    int cr_stride       = pFrame->linesize[ffRawFrame::Cr];

    long len_y          = y_width * y_height;
    long len_cb         = c_width * c_height;
    long len_cr         = c_width * c_height;

    unsigned char* pY   = pFrame->data[ffRawFrame::Y];
    unsigned char* pCb  = pFrame->data[ffRawFrame::Cb];
    unsigned char* pCr  = pFrame->data[ffRawFrame::Cr];

    // TODO: Check for null pointers and throw relevant exceptions.
    // Initialize the Y buffer.
    m_pY    = new unsigned char[len_y];
    for (long y = 0; y < y_height; y++)
        for (long x = 0; x < y_width; x++)
            m_pY[(y * y_width) + x] = pY[(y * y_stride) + x];

    // Initialize the Cb buffer.
    m_pCb   = new unsigned char[len_cb];
    for (long y = 0; y < c_height; y++)
        for (long x = 0; x < c_width; x++)
            m_pCb[(y * c_width) + x] = pCb[(y * cb_stride) + x];

    // Initialize the Cr buffer.
    m_pCr   = new unsigned char[len_cr];
    for (long y = 0; y < c_height; y++)
        for (long x = 0; x < c_width; x++)
            m_pCr[(y * c_width) + x] = pCr[(y * cr_stride) + x];
}

ffRawFrame::~ffRawFrame()
{
    delete[] m_pY;
    delete[] m_pCb;
    delete[] m_pCr;
}

// The following will take a given plane and scale it to the given
// dimensions using the given interpolator. For any given ffSequence,
// only one scaled float version will exist.
void ffRawFrame::scalePlane(ffRawFrame::Plane /*plane*/, ffSize /*dst*/,
                            ffSizeRatio /*ratio*/, ffInterpolator::Type interp)
{
    switch (interp)
    {
        case ffInterpolator::Nearest:
            break;
        case ffInterpolator::Linear:
            break;
        case ffInterpolator::Cubic:
            break;
        case ffInterpolator::Prefilter:
            break;
    }
}

/******************************************************************************
 * ffRawFrameFloat
 ******************************************************************************/
ffRawFrameFloat::ffRawFrameFloat(long len) :
    m_pfY(NULL),
    m_pfCb(NULL),
    m_pfCr(NULL)
{
    m_pfY = new float[len];
    m_pfCb = new float[len];
    m_pfCr = new float[len];
}

ffRawFrameFloat::~ffRawFrameFloat()
{
    delete[] m_pfY;
    delete[] m_pfCb;
    delete[] m_pfCr;
}

/******************************************************************************
 * ffSequence
 ******************************************************************************/
bool ffSequence::m_isInitialized;

ffSequence::ffSequence(void):
    m_pFormatCtx(NULL), m_pCodecCtx(NULL), m_pCodec(NULL),
    m_pExportDetails(NULL),
    m_totalFrames(ffDefault::NoFrame), m_currentFrame(ffDefault::NoFrame),
    m_lumaSize(0,0),
    m_chromaSize(0,0), m_stream(ffDefault::NoStream),
    m_state(isInvalid)
{
    if (!m_isInitialized)
        initialize();
    m_pExportDetails = new ffExportDetails;
}

ffSequence::~ffSequence()
{
    cleanup();
}

void ffSequence::initialize()
{
    av_register_all();
    m_isInitialized = true;
}

void ffSequence::pushRawFrame(AVFrame *pAVFrame)
{
    ffRawFrame* pRawFrame = new ffRawFrame(pAVFrame);
    m_frames.push_back(pRawFrame);
    onProgress((double)m_frames.size() / (double)m_totalFrames);
}

void ffSequence::cleanup(void)
{
    freeRawFrames();
    if (m_pCodecCtx)
    {
        avcodec_close(m_pCodecCtx);
        m_pCodecCtx = NULL;
    }

    if (m_pFormatCtx)
    {
        avformat_close_input(&m_pFormatCtx);
        m_pFormatCtx = NULL;
    }

    // The rest of the API relies on the various variables and makes
    // assumptions they are applicable. Zero them out.
    m_totalFrames = ffDefault::NoFrame;
    m_currentFrame = ffDefault::NoFrame;
    m_lumaSize = ffSize(FF_NO_DIMENSION, FF_NO_DIMENSION);
    m_chromaSize = ffSize(FF_NO_DIMENSION, FF_NO_DIMENSION);
    m_stream = ffDefault::NoStream;
    m_state = isInvalid;
    m_pExportDetails->deinit();
}

void ffSequence::freeRawFrames(void)
{
    while(m_frames.size() > 0)
    {
        delete m_frames.back();
        m_frames.pop_back();
    }
}

void ffSequence::readFile(char *fileName)
{
    try
    {
        cleanup();
        m_state = isLoading;
        onJustLoading();

        int retValue = -1;

        retValue = avformat_open_input(&m_pFormatCtx, fileName, NULL, NULL);
        if (retValue < 0)
            throw ffmpegError("avformat_open_input() < 0", retValue);

        retValue = avformat_find_stream_info(m_pFormatCtx, NULL);
        if (retValue < 0)
            throw ffmpegError("avformat_find_stream_info() < 0", retValue);

        for(unsigned int i = 0; i < m_pFormatCtx->nb_streams; i++)
          if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                // TODO: Provide for multiple video streams via vector or like.
                m_stream = i;
                break;
            }
        if (m_stream < 0)
            throw ffImportError("m_stream < 0", ffError::ERROR_NO_VIDEO_STREAM);

        m_pCodecCtx = m_pFormatCtx->streams[m_stream]->codec;
        if (m_pCodecCtx->pix_fmt != PIX_FMT_YUVJ420P)
            throw ffImportError("m_pCodecCtx->pix_fmt != PIX_FMT_YUVJ420P",
                          ffError::ERROR_BAD_FORMAT);

        m_lumaSize = ffSize(m_pCodecCtx->width, m_pCodecCtx->height);
        // While we could simply assume that all 4:2:0 content is half width
        // and half height, there may be some discrepancy between the actual
        // stored width and height for the chroma channels. To avoid this, we
        // correctly bitshift according to the chroma shift values.
        m_chromaSize = ffSize(
                    m_lumaSize.m_width >>
                    av_pix_fmt_descriptors[m_pCodecCtx->pix_fmt].log2_chroma_w,
                    m_lumaSize.m_height >>
                    av_pix_fmt_descriptors[m_pCodecCtx->pix_fmt].log2_chroma_h);

        m_totalFrames = m_pFormatCtx->streams[m_stream]->nb_frames;

        m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
        if (m_pCodec == NULL)
            throw ffImportError("m_pCodec == NULL", ffError::ERROR_NO_DECODER);

        retValue = avcodec_open2(m_pCodecCtx, m_pCodec, NULL);
        if (retValue < 0)
            throw ffmpegError("avcodec_open2() < 0", retValue);

        m_frames.reserve(m_totalFrames);

        AVFrame *pTempFrame = avcodec_alloc_frame();
        if (pTempFrame == NULL)
            throw ffImportError("avcodec_alloc_frame() == NULL",
                          ffError::ERROR_ALLOC_ERROR);

        ffAVPacket    tempPacket;
        int         got_picture = -1;

        onProgressStart();
        // Primary loop to iterate over the frames, allocate our ffRawFrames,
        // and push them into the vector storage.
        while (retValue = av_read_frame(m_pFormatCtx, &tempPacket),
               retValue == 0)
        {
            if (tempPacket.stream_index == m_stream)
            {
                int innerError = 0;
                innerError = avcodec_decode_video2(m_pCodecCtx, pTempFrame,
                                                   &got_picture, &tempPacket);
                if (innerError < 0)
                    throw ffmpegError("avcodec_decode_video2()", innerError);

                if (got_picture)
                    pushRawFrame(pTempFrame);
            }

            av_free_packet(&tempPacket);
        }
        if ((retValue != AVERROR_EOF) && (retValue != 0))
            throw ffmpegError("av_read_frame()", retValue);

        // got_picture above will return 0 for every bi-directionally encoded
        // frame. We must flush the frames out that remain internally buffered.
        while (retValue =
               avcodec_decode_video2(m_pCodecCtx, pTempFrame,
                                     &got_picture, &tempPacket),
               ((retValue != AVERROR_EOF) && (retValue >= 0) &&
                (got_picture)))
            pushRawFrame(pTempFrame);

        av_free_packet(&tempPacket);

        if (pTempFrame)
            av_freep(pTempFrame);

        // Only if we make it this far is the ffSequence object valid.
        m_state = isValid;
        m_fileURI = fileName;
        setCurrentFrame(ffDefault::FirstFrame, this);
        setExportDimensions(m_lumaSize.m_width, m_lumaSize.m_height, this);
        setExportTrim(ffDefault::FirstFrame, getTotalFrames(), this);
        onProgressEnd();
        onJustOpened();
    }
    catch(ffmpegError ffmpegErr)
    {
        cleanup();
//        onJustErrored();
        throw;
    }
    catch(ffExportError ffErr)
    {
        cleanup();
//        onJustErrored();
        throw;
    }
}

void ffSequence::writeFile(char *fileName)
{
    try
    {
        ImageOutput *imageOutput = ImageOutput::create (fileName);
        if (imageOutput == NULL)
            throw ffExportError("imageOutput == NULL",
                                ffError::ERROR_BAD_FILENAME);
        ImageSpec imageSpec(m_lumaSize.m_width, m_lumaSize.m_height,
                            1, TypeDesc::UINT8);

        imageOutput->open(fileName, imageSpec);
        imageOutput->write_image(TypeDesc::UINT8,
                                 getRawFrame(getCurrentFrame())->m_pY);
        imageOutput->close();
        delete imageOutput;
    }
    catch (ffExportError eff)
    {
//        onJustErrored();
        throw;
    }
}

void ffSequence::closeFile(void)
{
    cleanup();
    onJustClosed();
}

ffRawFrame* ffSequence::getRawFrame(long frame)
{
    return m_frames.at(frame - 1);
}

// All frames internally are zero based, but everything above this layer
// of abstraction should talk in one based "reality" terms.
ffRawFrame* ffSequence::setCurrentFrame(long frame, void *sender)
{
    if (((frame - 1) != m_currentFrame) &&
        ((frame - 1) > ffDefault::NoFrame) &&
        ((frame - 1) <= getTotalFrames()))
    {
        m_currentFrame = (frame - 1);
        onFrameChanged(getCurrentFrame(), sender);
    }
    else
    {
        throw ffError("((frame - 1) != m_currentFrame) && ((frame - 1) > "
                      "ffDefault::NoFrame) && ((frame - 1) <= "
                      "getTotalFrames())", ffError::ERROR_BAD_FRAME);
    }

    return getRawFrame(getCurrentFrame());
}

long ffSequence::getCurrentFrame(void)
{
    return (m_currentFrame + 1);
}

long ffSequence::getTotalFrames(void)
{
    return m_frames.size();
}

void ffSequence::setExportTrim(long in, long out, void *sender)
{
    if ((getState() == isValid) &&
            ((in < ffDefault::FirstFrame) || (out > getTotalFrames())))
        throw ffError("(in > getCurrentFrame()) || (out < getCurrentFrame())",
                      ffError::ERROR_BAD_TRIM);
    else
    {
        if (!((in == getExportTrim().m_in) && (out == getExportTrim().m_out)))
        {
            m_pExportDetails->setTrim(in, out);
            onExportTrimChanged(getExportTrim().m_in,
                                getExportTrim().m_out, sender);
        }
    }
}

void ffSequence::setExportTrimIn(long in, void *sender)
{
    setExportTrim(in, getExportTrim().m_out, sender);
}

void ffSequence::setExportTrimOut(long out, void *sender)
{
    setExportTrim(getExportTrim().m_in, out, sender);
}

void ffSequence::resetExportTrim(void *sender)
{
    setExportTrim(ffDefault::FirstFrame, getTotalFrames(), sender);
}

void ffSequence::resetExportTrimIn(void *sender)
{
    setExportTrim(ffDefault::FirstFrame, getExportTrim().m_out, sender);
}

void ffSequence::resetExportTrimOut(void *sender)
{
    setExportTrim(getExportTrim().m_in, getTotalFrames(), sender);
}

void ffSequence::setExportPlane(ffExportDetails::ExportPlane plane,
                                void *sender)
{
    if (plane != getExportPlane())
    {
        m_pExportDetails->setExportPlane(plane);
        onExportPlaneChanged(getExportPlane(), sender);
    }
}

void ffSequence::setExportDimensions(long width, long height, void *sender)
{
    if (!((width == getExportDimensions().m_width) &&
          (height == getExportDimensions().m_height)))
    {
        m_pExportDetails->setExportSize(ffSize(width, height));
        onExportDimensionsChanged(getExportDimensions().m_width,
                                  getExportDimensions().m_height,
                                  sender);
    }
}

ffTrim ffSequence::getExportTrim(void)
{
    return m_pExportDetails->getTrim();
}

ffExportDetails::ExportPlane ffSequence::getExportPlane()
{
    return m_pExportDetails->getExportPlane();
}

ffSize ffSequence::getExportDimensions()
{
    return m_pExportDetails->getExportSize();
}

ffSize ffSequence::getLumaSize(void)
{
    return m_lumaSize;
}

ffSize ffSequence::getChromaSize(void)
{
    return m_chromaSize;
}

ffSequenceState ffSequence::getState(void)
{
    return m_state;
}

std::string ffSequence::getFileURI(void)
{
    return m_fileURI;
}

void ffSequence::onProgressStart(void)
{
    // Pass
}

void ffSequence::onProgress(double)
{
    // Pass
}

void ffSequence::onProgressEnd(void)
{
    // Pass
}

void ffSequence::onJustLoading(void)
{
    // Pass
}

void ffSequence::onJustOpened(void)
{
    // Pass
}

void ffSequence::onJustClosed(void)
{
    // Pass
}

//void ffSequence::onJustErrored(void)
//{
//    // Pass
//}

void ffSequence::onExportTrimChanged(long, long, void *)
{
    // Pass
}

void ffSequence::onExportPlaneChanged(ffExportDetails::ExportPlane, void *)
{
    // Pass
}

void ffSequence::onExportDimensionsChanged(long, long, void *)
{
    // Pass
}

void ffSequence::onFrameChanged(long, void *)
{
    // Pass
}
