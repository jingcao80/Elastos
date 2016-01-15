
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODEC_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODEC_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CMediaCodec.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ISurface;
using Elastos::IO::IByteBuffer;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodec)
    , public Object
    , public IMediaCodec
{
private:
    class EventHandler
        : public Handler
    {
    public:
        EventHandler(
            /* [in] */ CMediaCodec* host,
            /* [in] */ ILooper* looper)
            : Handler(looper)
            , mCodec(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleCallback(
            /* [in] */ IMessage* msg);

    private:
        CMediaCodec* mCodec;
    };

    // used to track dequeued buffers
    class BufferMap
        : public Object
    {
    private:
        // various returned representations of the codec buffer
        class CodecBuffer
            : public Object
        {
        public:
            CodecBuffer(
                /* [in] */ BufferMap* host)
                : mHost(host)
            {}

            CARAPI Free();

            CARAPI SetImage(
                /* [in] */ IImage* image);

            CARAPI SetByteBuffer(
                /* [in] */ IByteBuffer* buffer);

        private:
            BufferMap* mHost;
            AutoPtr<IImage> mImage;
            AutoPtr<IByteBuffer> mByteBuffer;
        };

    public:
        BufferMap(
            /* [in] */ CMediaCodec* host)
            : mHost(host)
        {}

        CARAPI Remove(
            /* [in] */ Int32 index);

        CARAPI Put(
            /* [in] */ Int32 index,
            /* [in] */ IByteBuffer* newBuffer);

        CARAPI Put(
            /* [in] */ Int32 index,
            /* [in] */ IImage* newImage);

        CARAPI Clear();

    private:
        AutoPtr<IMap> mMap;

        CMediaCodec* mHost;
    };

public:
    CMediaCodec();

    virtual ~CMediaCodec();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Boolean nameIsType,
        /* [in] */ Boolean encoder);

    /**
     * Returns the codec to its initial (Initialized) state.
     *
     * Call this if an {@link MediaCodec.CodecException#isRecoverable unrecoverable}
     * error has occured to reset the codec to its initial state after creation.
     *
     * @throws CodecException if an unrecoverable error has occured and the codec
     * could not be reset.
     * @throws IllegalStateException if in the Uninitialized state.
     */
    CARAPI Reset();

    /**
     * Make sure you call this when you're done to free up any opened
     * component instance instead of relying on the garbage collector
     * to do this for you at some point in the future.
     */
    CARAPI ReleaseResources();

    /**
     * Configures a component.
     *
     * @param format The format of the input data (decoder) or the desired
     *               format of the output data (encoder).
     * @param surface Specify a surface on which to render the output of this
     *                decoder.
     * @param crypto  Specify a crypto object to facilitate secure decryption
     *                of the media data.
     * @param flags   Specify {@link #CONFIGURE_FLAG_ENCODE} to configure the
     *                component as an encoder.
     * @throws IllegalArgumentException if the surface has been released (or is invalid),
     * or the format is unacceptable (e.g. missing a mandatory key),
     * or the flags are not set properly
     * (e.g. missing {@link #CONFIGURE_FLAG_ENCODE} for an encoder).
     * @throws IllegalStateException if not in the Initialized state.
     */
    CARAPI Configure(
        /* [in] */ IMediaFormat* format,
        /* [in] */ ISurface* surface,
        /* [in] */ IMediaCrypto* crypto,
        /* [in] */ Int32 flags);

    /**
     * Requests a Surface to use as the input to an encoder, in place of input buffers.  This
     * may only be called after {@link #configure} and before {@link #start}.
     * <p>
     * The application is responsible for calling release() on the Surface when
     * done.
     * <p>
     * The Surface must be rendered with a hardware-accelerated API, such as OpenGL ES.
     * {@link android.view.Surface#lockCanvas(android.graphics.Rect)} may fail or produce
     * unexpected results.
     * @throws IllegalStateException if not in the Configured state.
     */
    CARAPI CreateInputSurface(
        /* [out] */ ISurface** result);

    /**
     * After successfully configuring the component, call {@code start}.
     * <p>
     * Call {@code start} also if the codec is configured in asynchronous mode,
     * and it has just been flushed, to resume requesting input buffers.
     * @throws IllegalStateException if not in the Configured state
     *         or just after {@link #flush} for a codec that is configured
     *         in asynchronous mode.
     * @throws MediaCodec.CodecException upon codec error. Note that some codec errors
     * for start may be attributed to future method calls.
     */
    CARAPI Start();

    /**
     * Finish the decode/encode session, note that the codec instance
     * remains active and ready to be {@link #start}ed again.
     * To ensure that it is available to other client call {@link #release}
     * and don't just rely on garbage collection to eventually do this for you.
     * @throws IllegalStateException if in the Uninitialized state.
     */
    CARAPI Stop();

    /**
     * Flush both input and output ports of the component, all indices
     * previously returned in calls to {@link #dequeueInputBuffer} and
     * {@link #dequeueOutputBuffer} become invalid.
     * <p>
     * If codec is configured in asynchronous mode, call {@link #start}
     * after {@code flush} has returned to resume codec operations. The
     * codec will not request input buffers until this has happened.
     * <p>
     * If codec is configured in synchronous mode, codec will resume
     * automatically if an input surface was created.  Otherwise, it
     * will resume when {@link #dequeueInputBuffer} is called.
     *
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI Flush();

    /**
     * After filling a range of the input buffer at the specified index
     * submit it to the component. Once an input buffer is queued to
     * the codec, it MUST NOT be used until it is later retrieved by
     * {@link #getInputBuffer} in response to a {@link #dequeueInputBuffer}
     * return value or a {@link Callback#onInputBufferAvailable}
     * callback.
     * <p>
     * Many decoders require the actual compressed data stream to be
     * preceded by "codec specific data", i.e. setup data used to initialize
     * the codec such as PPS/SPS in the case of AVC video or code tables
     * in the case of vorbis audio.
     * The class {@link android.media.MediaExtractor} provides codec
     * specific data as part of
     * the returned track format in entries named "csd-0", "csd-1" ...
     * <p>
     * These buffers can be submitted directly after {@link #start} or
     * {@link #flush} by specifying the flag {@link
     * #BUFFER_FLAG_CODEC_CONFIG}.  However, if you configure the
     * codec with a {@link MediaFormat} containing these keys, they
     * will be automatically submitted by MediaCodec directly after
     * start.  Therefore, the use of {@link
     * #BUFFER_FLAG_CODEC_CONFIG} flag is discouraged and is
     * recommended only for advanced users.
     * <p>
     * To indicate that this is the piece of input data (or rather that
     * no more input data follows unless the decoder is subsequently flushed)
     * specify the flag {@link #BUFFER_FLAG_END_OF_STREAM}.
     *
     * @param index The index of a client-owned input buffer previously returned
     *              in a call to {@link #dequeueInputBuffer}.
     * @param offset The byte offset into the input buffer at which the data starts.
     * @param size The number of bytes of valid input data.
     * @param presentationTimeUs The presentation timestamp in microseconds for this
     *                           buffer. This is normally the media time at which this
     *                           buffer should be presented (rendered).
     * @param flags A bitmask of flags
     *              {@link #BUFFER_FLAG_CODEC_CONFIG} and {@link #BUFFER_FLAG_END_OF_STREAM}.
     *              While not prohibited, most codecs do not use the
     *              {@link #BUFFER_FLAG_KEY_FRAME} flag for input buffers.
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     * @throws CryptoException if a crypto object has been specified in
     *         {@link #configure}
     */
    CARAPI QueueInputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Int64 presentationTimeUs,
        /* [in] */ Int32 flags);

    /**
     * Similar to {@link #queueInputBuffer} but submits a buffer that is
     * potentially encrypted.
     * @param index The index of a client-owned input buffer previously returned
     *              in a call to {@link #dequeueInputBuffer}.
     * @param offset The byte offset into the input buffer at which the data starts.
     * @param info Metadata required to facilitate decryption, the object can be
     *             reused immediately after this call returns.
     * @param presentationTimeUs The presentation timestamp in microseconds for this
     *                           buffer. This is normally the media time at which this
     *                           buffer should be presented (rendered).
     * @param flags A bitmask of flags
     *              {@link #BUFFER_FLAG_CODEC_CONFIG} and {@link #BUFFER_FLAG_END_OF_STREAM}.
     *              While not prohibited, most codecs do not use the
     *              {@link #BUFFER_FLAG_KEY_FRAME} flag for input buffers.
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     * @throws CryptoException if an error occurs while attempting to decrypt the buffer.
     *              An error code associated with the exception helps identify the
     *              reason for the failure.
     */
    CARAPI QueueSecureInputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ IMediaCodecCryptoInfo* info,
        /* [in] */ Int64 presentationTimeUs,
        /* [in] */ Int32 flags);

    /**
     * Returns the index of an input buffer to be filled with valid data
     * or -1 if no such buffer is currently available.
     * This method will return immediately if timeoutUs == 0, wait indefinitely
     * for the availability of an input buffer if timeoutUs &lt; 0 or wait up
     * to "timeoutUs" microseconds if timeoutUs &gt; 0.
     * @param timeoutUs The timeout in microseconds, a negative timeout indicates "infinite".
     * @throws IllegalStateException if not in the Executing state,
     *         or codec is configured in asynchronous mode.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI DequeueInputBuffer(
        /* [in] */ Int64 timeoutUs,
        /* [out] */ Int32* result);

    /**
     * Dequeue an output buffer, block at most "timeoutUs" microseconds.
     * Returns the index of an output buffer that has been successfully
     * decoded or one of the INFO_* constants below.
     * @param info Will be filled with buffer meta data.
     * @param timeoutUs The timeout in microseconds, a negative timeout indicates "infinite".
     * @throws IllegalStateException if not in the Executing state,
     *         or codec is configured in asynchronous mode.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI DequeueOutputBuffer(
        /* [in] */ IMediaCodecBufferInfo* info,
        /* [in] */ Int64 timeoutUs,
        /* [out] */ Int32* result);

    /**
     * If you are done with a buffer, use this call to return the buffer to
     * the codec. If you previously specified a surface when configuring this
     * video decoder you can optionally render the buffer.
     *
     * Once an output buffer is released to the codec, it MUST NOT
     * be used until it is later retrieved by {@link #getOutputBuffer} in response
     * to a {@link #dequeueOutputBuffer} return value or a
     * {@link Callback#onOutputBufferAvailable} callback.
     *
     * @param index The index of a client-owned output buffer previously returned
     *              from a call to {@link #dequeueOutputBuffer}.
     * @param render If a valid surface was specified when configuring the codec,
     *               passing true renders this output buffer to the surface.
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI ReleaseOutputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Boolean render);

    /**
     * If you are done with a buffer, use this call to update its surface timestamp
     * and return it to the codec to render it on the output surface. If you
     * have not specified an output surface when configuring this video codec,
     * this call will simply return the buffer to the codec.<p>
     *
     * The timestamp may have special meaning depending on the destination surface.
     *
     * <table>
     * <tr><th>SurfaceView specifics</th></tr>
     * <tr><td>
     * If you render your buffer on a {@link android.view.SurfaceView},
     * you can use the timestamp to render the buffer at a specific time (at the
     * VSYNC at or after the buffer timestamp).  For this to work, the timestamp
     * needs to be <i>reasonably close</i> to the current {@link System#nanoTime}.
     * Currently, this is set as within one (1) second. A few notes:
     *
     * <ul>
     * <li>the buffer will not be returned to the codec until the timestamp
     * has passed and the buffer is no longer used by the {@link android.view.Surface}.
     * <li>buffers are processed sequentially, so you may block subsequent buffers to
     * be displayed on the {@link android.view.Surface}.  This is important if you
     * want to react to user action, e.g. stop the video or seek.
     * <li>if multiple buffers are sent to the {@link android.view.Surface} to be
     * rendered at the same VSYNC, the last one will be shown, and the other ones
     * will be dropped.
     * <li>if the timestamp is <em>not</em> "reasonably close" to the current system
     * time, the {@link android.view.Surface} will ignore the timestamp, and
     * display the buffer at the earliest feasible time.  In this mode it will not
     * drop frames.
     * <li>for best performance and quality, call this method when you are about
     * two VSYNCs' time before the desired render time.  For 60Hz displays, this is
     * about 33 msec.
     * </ul>
     * </td></tr>
     * </table>
     *
     * Once an output buffer is released to the codec, it MUST NOT
     * be used until it is later retrieved by {@link #getOutputBuffer} in response
     * to a {@link #dequeueOutputBuffer} return value or a
     * {@link Callback#onOutputBufferAvailable} callback.
     *
     * @param index The index of a client-owned output buffer previously returned
     *              from a call to {@link #dequeueOutputBuffer}.
     * @param renderTimestampNs The timestamp to associate with this buffer when
     *              it is sent to the Surface.
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI ReleaseOutputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Int64 renderTimestampNs);

    /**
     * Signals end-of-stream on input.  Equivalent to submitting an empty buffer with
     * {@link #BUFFER_FLAG_END_OF_STREAM} set.  This may only be used with
     * encoders receiving input from a Surface created by {@link #createInputSurface}.
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI SignalEndOfInputStream();

    /**
     * Call this after dequeueOutputBuffer signals a format change by returning
     * {@link #INFO_OUTPUT_FORMAT_CHANGED}.
     * You can also call this after {@link #configure} returns
     * successfully to get the output format initially configured
     * for the codec.  Do this to determine what optional
     * configuration parameters were supported by the codec.
     *
     * @throws IllegalStateException if not in the Executing or
     *                               Configured state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetOutputFormat(
        /* [out] */ IMediaFormat** result);

    /**
     * Call this after {@link #configure} returns successfully to
     * get the input format accepted by the codec. Do this to
     * determine what optional configuration parameters were
     * supported by the codec.
     *
     * @throws IllegalStateException if not in the Executing or
     *                               Configured state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetInputFormat(
        /* [out] */ IMediaFormat** result);

    /**
     * Returns the output format for a specific output buffer.
     *
     * @param index The index of a client-owned input buffer previously
     *              returned from a call to {@link #dequeueInputBuffer}.
     *
     * @return the format for the output buffer, or null if the index
     * is not a dequeued output buffer.
     */
    CARAPI GetOutputFormat(
        /* [in] */ Int32 index,
        /* [out] */ IMediaFormat** result);

    /**
     * Retrieve the set of input buffers.  Call this after start()
     * returns. After calling this method, any ByteBuffers
     * previously returned by an earlier call to this method MUST no
     * longer be used.
     *
     * @deprecated Use the new {@link #getInputBuffer} method instead
     * each time an input buffer is dequeued.
     *
     * <b>Note:</b>As of API 21, dequeued input buffers are
     * automatically {@link java.nio.Buffer#clear cleared}.
     *
     * @throws IllegalStateException if not in the Executing state,
     *         or codec is configured in asynchronous mode.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetInputBuffers(
        /* [out, callee] */ ArrayOf<IByteBuffer*>** result);

    /**
     * Retrieve the set of output buffers.  Call this after start()
     * returns and whenever dequeueOutputBuffer signals an output
     * buffer change by returning {@link
     * #INFO_OUTPUT_BUFFERS_CHANGED}. After calling this method, any
     * ByteBuffers previously returned by an earlier call to this
     * method MUST no longer be used.
     *
     * @deprecated Use the new {@link #getOutputBuffer} method instead
     * each time an output buffer is dequeued.  This method is not
     * supported if codec is configured in asynchronous mode.
     *
     * <b>Note:</b>As of API 21, the position and limit of output
     * buffers that are dequeued will be set to the valid data
     * range.
     *
     * @throws IllegalStateException if not in the Executing state,
     *         or codec is configured in asynchronous mode.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetOutputBuffers(
        /* [out, callee] */ ArrayOf<IByteBuffer*>** result);

    /**
     * Returns a {@link java.nio.Buffer#clear cleared}, writable ByteBuffer
     * object for a dequeued input buffer index to contain the input data.
     *
     * After calling this method any ByteBuffer or Image object
     * previously returned for the same input index MUST no longer
     * be used.
     *
     * @param index The index of a client-owned input buffer previously
     *              returned from a call to {@link #dequeueInputBuffer},
     *              or received via an onInputBufferAvailable callback.
     *
     * @return the input buffer, or null if the index is not a dequeued
     * input buffer, or if the codec is configured for surface input.
     *
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetInputBuffer(
        /* [in] */ Int32 index,
        /* [out] */ IByteBuffer** result);

    /**
     * Returns a writable Image object for a dequeued input buffer
     * index to contain the raw input video frame.
     *
     * After calling this method any ByteBuffer or Image object
     * previously returned for the same input index MUST no longer
     * be used.
     *
     * @param index The index of a client-owned input buffer previously
     *              returned from a call to {@link #dequeueInputBuffer},
     *              or received via an onInputBufferAvailable callback.
     *
     * @return the input image, or null if the index is not a
     * dequeued input buffer, or not a ByteBuffer that contains a
     * raw image.
     *
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetInputImage(
        /* [in] */ Int32 index,
        /* [out] */ IImage** result);

    /**
     * Returns a read-only ByteBuffer for a dequeued output buffer
     * index. The position and limit of the returned buffer are set
     * to the valid output data.
     *
     * After calling this method, any ByteBuffer or Image object
     * previously returned for the same output index MUST no longer
     * be used.
     *
     * @param index The index of a client-owned output buffer previously
     *              returned from a call to {@link #dequeueOutputBuffer},
     *              or received via an onOutputBufferAvailable callback.
     *
     * @return the output buffer, or null if the index is not a dequeued
     * output buffer, or the codec is configured with an output surface.
     *
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetOutputBuffer(
        /* [in] */ Int32 index,
        /* [out] */ IByteBuffer** result);

    /**
     * Returns a read-only Image object for a dequeued output buffer
     * index that contains the raw video frame.
     *
     * After calling this method, any ByteBuffer or Image object previously
     * returned for the same output index MUST no longer be used.
     *
     * @param index The index of a client-owned output buffer previously
     *              returned from a call to {@link #dequeueOutputBuffer},
     *              or received via an onOutputBufferAvailable callback.
     *
     * @return the output image, or null if the index is not a
     * dequeued output buffer, not a raw video frame, or if the codec
     * was configured with an output surface.
     *
     * @throws IllegalStateException if not in the Executing state.
     * @throws MediaCodec.CodecException upon codec error.
     */
    CARAPI GetOutputImage(
        /* [in] */ Int32 index,
        /* [out] */ IImage** result);

    /**
     * If a surface has been specified in a previous call to {@link #configure}
     * specifies the scaling mode to use. The default is "scale to fit".
     * @throws IllegalArgumentException if mode is not recognized.
     * @throws IllegalStateException if in the Uninitialized state.
     */
    CARAPI SetVideoScalingMode(
        /* [in] */ Int32 mode);

    /**
     * Get the component name. If the codec was created by createDecoderByType
     * or createEncoderByType, what component is chosen is not known beforehand.
     * @throws IllegalStateException if in the Uninitialized state.
     */
    CARAPI GetName(
        /* [out] */ String* result);

    /**
     * Communicate additional parameter changes to the component instance.
     * @throws IllegalStateException if in the Uninitialized state.
     */
    CARAPI SetParameters(
        /* [in] */ IBundle* params);

    /**
     * Sets an asynchronous callback for actionable MediaCodec events.
     *
     * If the client intends to use the component in asynchronous mode,
     * a valid callback should be provided before {@link #configure} is called.
     *
     * When asynchronous callback is enabled, the client should not call
     * {@link #getInputBuffers}, {@link #getOutputBuffers},
     * {@link #dequeueInputBuffer(long)} or {@link #dequeueOutputBuffer(BufferInfo, long)}.
     * <p>
     * Also, {@link #flush} behaves differently in asynchronous mode.  After calling
     * {@code flush}, you must call {@link #start} to "resume" receiving input buffers,
     * even if an input surface was created.
     *
     * @param cb The callback that will run.
     */
    CARAPI SetCallback(
        /* [in] */ IMediaCodecCallback* cb);

    /**
     * Get the codec info. If the codec was created by createDecoderByType
     * or createEncoderByType, what component is chosen is not known beforehand,
     * and thus the caller does not have the MediaCodecInfo.
     * @throws IllegalStateException if in the Uninitialized state.
     */
    CARAPI GetCodecInfo(
        /* [out] */ IMediaCodecInfo** result);

    /**
     * Instantiate a decoder supporting input data of the given mime type.
     *
     * The following is a partial list of defined mime types and their semantics:
     * <ul>
     * <li>"video/x-vnd.on2.vp8" - VP8 video (i.e. video in .webm)
     * <li>"video/x-vnd.on2.vp9" - VP9 video (i.e. video in .webm)
     * <li>"video/avc" - H.264/AVC video
     * <li>"video/hevc" - H.265/HEVC video
     * <li>"video/mp4v-es" - MPEG4 video
     * <li>"video/3gpp" - H.263 video
     * <li>"audio/3gpp" - AMR narrowband audio
     * <li>"audio/amr-wb" - AMR wideband audio
     * <li>"audio/mpeg" - MPEG1/2 audio layer III
     * <li>"audio/mp4a-latm" - AAC audio (note, this is raw AAC packets, not packaged in LATM!)
     * <li>"audio/vorbis" - vorbis audio
     * <li>"audio/g711-alaw" - G.711 alaw audio
     * <li>"audio/g711-mlaw" - G.711 ulaw audio
     * </ul>
     *
     * @param type The mime type of the input data.
     * @throws IOException if the codec cannot be created.
     * @throws IllegalArgumentException if type is not a valid mime type.
     * @throws NullPointerException if type is null.
     */
    static CARAPI CreateDecoderByType(
        /* [in] */ const String& type,
        /* [out] */ IMediaCodec** result);

    /**
     * Instantiate an encoder supporting output data of the given mime type.
     * @param type The desired mime type of the output data.
     * @throws IOException if the codec cannot be created.
     * @throws IllegalArgumentException if type is not a valid mime type.
     * @throws NullPointerException if type is null.
     */
    static CARAPI CreateEncoderByType(
        /* [in] */ const String& type,
        /* [out] */ IMediaCodec** result);

    /**
     * If you know the exact name of the component you want to instantiate
     * use this method to instantiate it. Use with caution.
     * Likely to be used with information obtained from {@link android.media.MediaCodecList}
     * @param name The name of the codec to be instantiated.
     * @throws IOException if the codec cannot be created.
     * @throws IllegalArgumentException if name is not valid.
     * @throws NullPointerException if name is null.
     */
    static CARAPI CreateByCodecName(
        /* [in] */ const String& name,
        /* [out] */ IMediaCodec** result);

private:
    CARAPI_(void) NativeReset();

    CARAPI_(void) NativeRelease();

    CARAPI_(void) NativeSetCallback(
        /* [in] */ IMediaCodecCallback* cb);

    CARAPI_(void) NativeConfigure(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [in] */ ISurface* surface,
        /* [in] */ IMediaCrypto* crypto,
        /* [in] */ Int32 flags);

    CARAPI_(void) NativeStart();

    CARAPI_(void) NativeStop();

    CARAPI_(void) NativeFlush();

    CARAPI_(void) NativeQueueInputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Int64 presentationTimeUs,
        /* [in] */ Int32 flags);

    CARAPI_(void) NativeQueueSecureInputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ IMediaCodecCryptoInfo* info,
        /* [in] */ Int64 presentationTimeUs,
        /* [in] */ Int32 flags);

    CARAPI_(Int32) NativeDequeueInputBuffer(
        /* [in] */ Int64 timeoutUs);

    CARAPI_(Int32) NativeDequeueOutputBuffer(
        /* [in] */ IMediaCodecBufferInfo* info,
        /* [in] */ Int64 timeoutUs);

    CARAPI_(void) NativeReleaseOutputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Boolean render,
        /* [in] */ Boolean updatePTS,
        /* [in] */ Int64 timeNs);

    CARAPI_(AutoPtr<IMap>) NativeGetFormatNative(
        /* [in] */ Boolean input);

    CARAPI_(AutoPtr<IMap>) NativeGetOutputFormatNative(
        /* [in] */ Int32 index);

    CARAPI_(void) InvalidateByteBuffer(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [in] */ Int32 index);

    CARAPI_(void) ValidateInputByteBuffer(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [in] */ Int32 index);

    CARAPI_(void) RevalidateByteBuffer(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [in] */ Int32 index);

    CARAPI_(void) ValidateOutputByteBuffer(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [in] */ Int32 index,
        /* [in] */ IMediaCodecBufferInfo* info);

    CARAPI_(void) InvalidateByteBuffers(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers);

    CARAPI_(void) FreeByteBuffer(
        /* [in] */ IByteBuffer* buffer);

    CARAPI_(void) FreeByteBuffers(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers);

    CARAPI_(void) FreeAllTrackedBuffers();

    CARAPI_(void) CacheBuffers(
        /* [in] */ Boolean input);

    CARAPI_(void) PostEventFromNative(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    CARAPI_(void) NativeSetParameters(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<IInterface*>* values);

    CARAPI_(AutoPtr<ArrayOf<IByteBuffer*> >) NativeGetBuffers(
        /* [in] */ Boolean input);

    CARAPI_(AutoPtr<IByteBuffer>) NativeGetBuffer(
        /* [in] */ Boolean input,
        /* [in] */ Int32 index);

    CARAPI_(AutoPtr<IImage>) NativeGetImage(
        /* [in] */ Boolean input,
        /* [in] */ Int32 index);

    static CARAPI_(void) NativeInit();

    CARAPI_(void) NativeSetup(
        /* [in] */ const String& name,
        /* [in] */ Boolean nameIsType,
        /* [in] */ Boolean encoder);

    CARAPI_(void) NativeFinalize();

private:
    AutoPtr<EventHandler> mEventHandler;
    AutoPtr<IMediaCodecCallback> mCallback;

    static const Int32 EVENT_CALLBACK;
    static const Int32 EVENT_SET_CALLBACK;
    static const Int32 CB_INPUT_AVAILABLE;
    static const Int32 CB_OUTPUT_AVAILABLE;
    static const Int32 CB_ERROR;
    static const Int32 CB_OUTPUT_FORMAT_CHANGE;

    AutoPtr<ArrayOf<IByteBuffer*> > mCachedInputBuffers;
    AutoPtr<ArrayOf<IByteBuffer*> > mCachedOutputBuffers;
    AutoPtr<BufferMap> mDequeuedInputBuffers;
    AutoPtr<BufferMap> mDequeuedOutputBuffers;
    Object mBufferLock;
    Int64 mNativeContext;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODEC_H__
