
#ifndef __ELASTOS_DROID_MEDIA_CAUDIORECORD_H__
#define __ELASTOS_DROID_MEDIA_CAUDIORECORD_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Media_CAudioRecord.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Privacy::IPrivacySettingsManager;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioRecord)
    , public Object
    , public IAudioRecord
{
private:
    //---------------------------------------------------------
    // Inner classes
    //---------------------------------------------------------
    /**
     * Helper class to handle the forwarding of native events to the appropriate listener
     * (potentially) handled in a different thread
     */
    class NativeEventHandler
        : public Handler
    {
    public:
        NativeEventHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CAudioRecord* owner)
            : Handler(looper)
            , mOwner(owner)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CAudioRecord* mOwner;
    };

public:
    CAudioRecord();

    virtual ~CAudioRecord();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    //---------------------------------------------------------
    // Constructor, Finalize
    //--------------------
    /**
     * Class constructor.
     * Though some invalid parameters will result in an {@link IllegalArgumentException} exception,
     * other errors do not.  Thus you should call {@link #getState()} immediately after construction
     * to confirm that the object is usable.
     * @param audioSource the recording source (also referred to as capture preset).
     *    See {@link MediaRecorder.AudioSource} for the capture preset definitions.
     * @param sampleRateInHz the sample rate expressed in Hertz. 44100Hz is currently the only
     *   rate that is guaranteed to work on all devices, but other rates such as 22050,
     *   16000, and 11025 may work on some devices.
     * @param channelConfig describes the configuration of the audio channels.
     *   See {@link AudioFormat#CHANNEL_IN_MONO} and
     *   {@link AudioFormat#CHANNEL_IN_STEREO}.  {@link AudioFormat#CHANNEL_IN_MONO} is guaranteed
     *   to work on all devices.
     * @param audioFormat the format in which the audio data is represented.
     *   See {@link AudioFormat#ENCODING_PCM_16BIT} and
     *   {@link AudioFormat#ENCODING_PCM_8BIT}
     * @param bufferSizeInBytes the total size (in bytes) of the buffer where audio data is written
     *   to during the recording. New audio data can be read from this buffer in smaller chunks
     *   than this size. See {@link #getMinBufferSize(int, int, int)} to determine the minimum
     *   required buffer size for the successful creation of an AudioRecord instance. Using values
     *   smaller than getMinBufferSize() will result in an initialization failure.
     * @throws java.lang.IllegalArgumentException
     */
    CARAPI constructor( // throws IllegalArgumentException
        /* [in] */ Int32 audioSource,
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 channelConfig,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 bufferSizeInBytes);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * Class constructor with {@link AudioAttributes} and {@link AudioFormat}.
     * @param attributes a non-null {@link AudioAttributes} instance. Use
     *     {@link AudioAttributes.Builder#setCapturePreset(int)} for configuring the capture
     *     preset for this instance.
     * @param format a non-null {@link AudioFormat} instance describing the format of the data
     *     that will be recorded through this AudioRecord. See {@link AudioFormat.Builder} for
     *     configuring the audio format parameters such as encoding, channel mask and sample rate.
     * @param bufferSizeInBytes the total size (in bytes) of the buffer where audio data is written
     *   to during the recording. New audio data can be read from this buffer in smaller chunks
     *   than this size. See {@link #getMinBufferSize(int, int, int)} to determine the minimum
     *   required buffer size for the successful creation of an AudioRecord instance. Using values
     *   smaller than getMinBufferSize() will result in an initialization failure.
     * @param sessionId ID of audio session the AudioRecord must be attached to, or
     *   {@link AudioManager#AUDIO_SESSION_ID_GENERATE} if the session isn't known at construction
     *   time. See also {@link AudioManager#generateAudioSessionId()} to obtain a session ID before
     *   construction.
     * @throws IllegalArgumentException
     */
    CARAPI constructor(
        /* [in] */ IAudioAttributes* attributes,
        /* [in] */ IAudioFormat* format,
        /* [in] */ Int32 bufferSizeInBytes,
        /* [in] */ Int32 sessionId);

    /**
     * Releases the native AudioRecord resources.
     * The object can no longer be used and the reference should be set to NULL
     * after a call to release()
     */
    CARAPI ReleaseResources();

    //---------------------------------------------------------
    // Getters
    //---------------------------------------------------------
    /**
     * Returns the configured audio data sample rate in Hz
     */
    CARAPI GetSampleRate(
        /* [out] */ Int32* result);

    /**
     * Returns the audio recording source.
     * @see MediaRecorder.AudioSource
     */
    CARAPI GetAudioSource(
        /* [out] */ Int32* result);

    /**
     * Returns the configured audio data format. See {@link AudioFormat#ENCODING_PCM_16BIT}
     * and {@link AudioFormat#ENCODING_PCM_8BIT}.
     */
    CARAPI GetAudioFormat(
        /* [out] */ Int32* result);

    /**
     * Returns the configured channel configuration.
     * See {@link AudioFormat#CHANNEL_IN_MONO}
     * and {@link AudioFormat#CHANNEL_IN_STEREO}.
     */
    CARAPI GetChannelConfiguration(
        /* [out] */ Int32* result);

    /**
     * Returns the configured number of channels.
     */
    CARAPI GetChannelCount(
        /* [out] */ Int32* result);

    /**
     * Returns the state of the AudioRecord instance. This is useful after the
     * AudioRecord instance has been created to check if it was initialized
     * properly. This ensures that the appropriate hardware resources have been
     * acquired.
     * @see AudioRecord#STATE_INITIALIZED
     * @see AudioRecord#STATE_UNINITIALIZED
     */
    CARAPI GetState(
        /* [out] */ Int32* result);

    /**
     * Returns the recording state of the AudioRecord instance.
     * @see AudioRecord#RECORDSTATE_STOPPED
     * @see AudioRecord#RECORDSTATE_RECORDING
     */
    CARAPI GetRecordingState(
        /* [out] */ Int32* result);

    /**
     * Returns the notification marker position expressed in frames.
     */
    CARAPI GetNotificationMarkerPosition(
        /* [out] */ Int32* result);

    /**
     * Returns the notification update period expressed in frames.
     */
    CARAPI GetPositionNotificationPeriod(
        /* [out] */ Int32* result);

    /**
     * Returns the audio session ID.
     *
     * @return the ID of the audio session this AudioRecord belongs to.
     */
    CARAPI GetAudioSessionId(
        /* [out] */ Int32* result);

    //---------------------------------------------------------
    // Transport control methods
    //---------------------------------------------------------
    /**
     * Starts recording from the AudioRecord instance.
     * @throws IllegalStateException
     */
    CARAPI StartRecording();

    /**
     * Starts recording from the AudioRecord instance when the specified synchronization event
     * occurs on the specified audio session.
     * @throws IllegalStateException
     * @param syncEvent event that triggers the capture.
     * @see MediaSyncEvent
     */
    CARAPI StartRecording(
        /* [in] */ IMediaSyncEvent* syncEvent);

    /**
     * Stops recording.
     * @throws IllegalStateException
     */
    CARAPI Stop();

    //---------------------------------------------------------
    // Audio data supply
    //---------------------------------------------------------
    /**
     * Reads audio data from the audio hardware for recording into a buffer.
     * @param audioData the array to which the recorded audio data is written.
     * @param offsetInBytes index in audioData from which the data is written expressed in bytes.
     * @param sizeInBytes the number of requested bytes.
     * @return the number of bytes that were read or or {@link #ERROR_INVALID_OPERATION}
     *    if the object wasn't properly initialized, or {@link #ERROR_BAD_VALUE} if
     *    the parameters don't resolve to valid data and indexes.
     *    The number of bytes will not exceed sizeInBytes.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* audioData,
        /* [in] */ Int32 offsetInBytes,
        /* [in] */ Int32 sizeInBytes,
        /* [out] */ Int32* result);

    /**
     * Reads audio data from the audio hardware for recording into a buffer.
     * @param audioData the array to which the recorded audio data is written.
     * @param offsetInShorts index in audioData from which the data is written expressed in shorts.
     * @param sizeInShorts the number of requested shorts.
     * @return the number of shorts that were read or or {@link #ERROR_INVALID_OPERATION}
     *    if the object wasn't properly initialized, or {@link #ERROR_BAD_VALUE} if
     *    the parameters don't resolve to valid data and indexes.
     *    The number of shorts will not exceed sizeInShorts.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Int16>* audioData,
        /* [in] */ Int32 offsetInShorts,
        /* [in] */ Int32 sizeInShorts,
        /* [out] */ Int32* result);

    /**
     * Reads audio data from the audio hardware for recording into a direct buffer. If this buffer
     * is not a direct buffer, this method will always return 0.
     * @param audioBuffer the direct buffer to which the recorded audio data is written.
     * @param sizeInBytes the number of requested bytes.
     * @return the number of bytes that were read or or {@link #ERROR_INVALID_OPERATION}
     *    if the object wasn't properly initialized, or {@link #ERROR_BAD_VALUE} if
     *    the parameters don't resolve to valid data and indexes.
     *    The number of bytes will not exceed sizeInBytes.
     */
    CARAPI Read(
        /* [in] */ IByteBuffer* audioBuffer,
        /* [in] */ Int32 sizeInBytes,
        /* [out] */ Int32* result);

    //---------------------------------------------------------
    // Initialization / configuration
    //---------------------------------------------------------
    /**
     * Sets the listener the AudioRecord notifies when a previously set marker is reached or
     * for each periodic record head position update.
     * @param listener
     */
    CARAPI SetRecordPositionUpdateListener(
        /* [in] */ IAudioRecordOnRecordPositionUpdateListener* listener);

    /**
     * Sets the listener the AudioRecord notifies when a previously set marker is reached or
     * for each periodic record head position update.
     * Use this method to receive AudioRecord events in the Handler associated with another
     * thread than the one in which you created the AudioTrack instance.
     * @param listener
     * @param handler the Handler that will receive the event notification messages.
     */
    CARAPI SetRecordPositionUpdateListener(
        /* [in] */ IAudioRecordOnRecordPositionUpdateListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Sets the marker position at which the listener is called, if set with
     * {@link #setRecordPositionUpdateListener(OnRecordPositionUpdateListener)} or
     * {@link #setRecordPositionUpdateListener(OnRecordPositionUpdateListener, Handler)}.
     * @param markerInFrames marker position expressed in frames
     * @return error code or success, see {@link #SUCCESS}, {@link #ERROR_BAD_VALUE},
     *  {@link #ERROR_INVALID_OPERATION}
     */
    CARAPI SetNotificationMarkerPosition(
        /* [in] */ Int32 markerInFrames,
        /* [out] */ Int32* result);

    /**
     * Sets the period at which the listener is called, if set with
     * {@link #setRecordPositionUpdateListener(OnRecordPositionUpdateListener)} or
     * {@link #setRecordPositionUpdateListener(OnRecordPositionUpdateListener, Handler)}.
     * @param periodInFrames update period expressed in frames
     * @return error code or success, see {@link #SUCCESS}, {@link #ERROR_INVALID_OPERATION}
     */
    CARAPI SetPositionNotificationPeriod(
        /* [in] */ Int32 periodInFrames,
        /* [out] */ Int32* result);

    /**
     * Returns the minimum buffer size required for the successful creation of an AudioRecord
     * object.
     * Note that this size doesn't guarantee a smooth recording under load, and higher values
     * should be chosen according to the expected frequency at which the AudioRecord instance
     * will be polled for new data.
     * @param sampleRateInHz the sample rate expressed in Hertz.
     * @param channelConfig describes the configuration of the audio channels.
     *   See {@link AudioFormat#CHANNEL_IN_MONO} and
     *   {@link AudioFormat#CHANNEL_IN_STEREO}
     * @param audioFormat the format in which the audio data is represented.
     *   See {@link AudioFormat#ENCODING_PCM_16BIT}.
     * @return {@link #ERROR_BAD_VALUE} if the recording parameters are not supported by the
     *  hardware, or an invalid parameter was passed,
     *  or {@link #ERROR} if the implementation was unable to query the hardware for its
     *  output properties,
     *   or the minimum buffer size expressed in bytes.
     * @see #AudioRecord(int, int, int, int, int) for more information on valid
     *   configuration values.
     */
    static CARAPI GetMinBufferSize(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 channelConfig,
        /* [in] */ Int32 audioFormat,
        /* [out] */ Int32* result);

protected:
    //@Override
    CARAPI_(void) Finalize();

private:
    // Convenience method for the constructor's parameter checks.
    // This is where constructor IllegalArgumentException-s are thrown
    // postconditions:
    //    mRecordSource is valid
    //    mChannelCount is valid
    //    mChannels is valid
    //    mAudioFormat is valid
    //    mSampleRate is valid
    CARAPI AudioParamCheck(
        /* [in] */ Int32 audioSource,
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 audioFormat);

    // Convenience method for the contructor's audio buffer size check.
    // preconditions:
    //    mChannelCount is valid
    //    mAudioFormat is AudioFormat.ENCODING_PCM_8BIT OR AudioFormat.ENCODING_PCM_16BIT
    // postcondition:
    //    mNativeBufferSizeInBytes is valid (multiple of frame size, positive)
    CARAPI AudioBuffSizeCheck(
        /* [in] */ Int32 audioBufferSize);

    CARAPI_(void) HandleFullVolumeRec(
        /* [in] */ Boolean starting);

    // Convenience method for the constructor's parameter checks.
    // This, getChannelMaskFromLegacyConfig and audioBuffSizeCheck are where constructor
    // IllegalArgumentException-s are thrown
    static CARAPI GetChannelMaskFromLegacyConfig(
        /* [in] */ Int32 inChannelConfig,
        /* [in] */ Boolean allowLegacyConfig,
        /* [out] */ Int32* result);

    //---------------------------------------------------------
    // Java methods called from the native side
    //---------------------------------------------------------
    //@SuppressWarnings("unused")
    static CARAPI_(void) PostEventFromNative(
        /* [in] */ IInterface* audiorecord_ref,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    //---------------------------------------------------------
    // Native methods called from the Java side
    //---------------------------------------------------------
    static CARAPI_(void) recorderCallback(Int32 event, void* user, void *info);

    CARAPI_(Int32) NativeSetup(
        /* [in] */ IWeakReference* audiorecord_this,
        /* [in] */ IAudioAttributes* attributes,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 buffSizeInBytes,
        /* [in] */ ArrayOf<Int32>* sessionId);

    CARAPI_(void) NativeFinalize();

    CARAPI_(void) NativeRelease();

    CARAPI NativeStart(
        /* [in] */ Int32 syncEvent,
        /* [in] */ Int32 sessionId,
        /* [out] */ Int32* result);

    CARAPI NativeStop();

    CARAPI_(Int32) Native_read_in_byte_array(
        /* [in] */ ArrayOf<Byte>* audioData,
        /* [in] */ Int32 offsetInBytes,
        /* [in] */ Int32 sizeInBytes);

    CARAPI_(Int32) Native_read_in_short_array(
        /* [in] */ ArrayOf<Int16>* audioData,
        /* [in] */ Int32 offsetInShorts,
        /* [in] */ Int32 sizeInShorts);

    CARAPI_(Int32) Native_read_in_direct_buffer(
        /* [in] */ IInterface* buffer,
        /* [in] */ Int32 sizeInBytes);

    CARAPI Native_set_marker_pos(
        /* [in] */ Int32 marker,
        /* [out] */ Int32* result);

    CARAPI Native_get_marker_pos(
        /* [out] */ Int32* result);

    CARAPI Native_set_pos_update_period(
        /* [in] */ Int32 updatePeriod,
        /* [out] */ Int32* result);

    CARAPI Native_get_pos_update_period(
        /* [out] */ Int32* result);

    static CARAPI_(Int32) Native_get_min_buff_size(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 channelCount,
        /* [in] */ Int32 audioFormat);

    CARAPI Native_check_permission(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    //---------------------------------------------------------
    // Utility methods
    //---------------------------------------------------------
    static CARAPI_(void) Logd(
        /* [in] */ const String& msg);

    static CARAPI_(void) Loge(
        /* [in] */ const String& msg);

    CARAPI IsAudioRecordAllowed(
        /* [out] */ Boolean* result);

private:
    // Events:
    // to keep in sync with frameworks/base/include/media/AudioRecord.h
    /**
     * Event id denotes when record head has reached a previously set marker.
     */
    static const Int32 NATIVE_EVENT_MARKER;

    /**
     * Event id denotes when previously set update period has elapsed during recording.
     */
    static const Int32 NATIVE_EVENT_NEW_POS;

    static const String TAG;

    //---------------------------------------------------------
    // Used exclusively by native code
    //---------------------------------------------------------
    /**
     * Accessed by native methods: provides access to C++ AudioRecord object
     */
    //@SuppressWarnings("unused")
    Int64 mNativeRecorderInJavaObj;

    /**
     * Accessed by native methods: provides access to the callback data.
     */
    //@SuppressWarnings("unused")
    Int64 mNativeCallbackCookie;


    //---------------------------------------------------------
    // Member variables
    //---------------------------------------------------------
    /**
     * The audio data sampling rate in Hz.
     */
    Int32 mSampleRate;

    /**
     * The number of input audio channels (1 is mono, 2 is stereo)
     */
    Int32 mChannelCount;

    /**
     * The audio channel mask
     */
    Int32 mChannelMask;

    /**
     * The encoding of the audio samples.
     * @see AudioFormat#ENCODING_PCM_8BIT
     * @see AudioFormat#ENCODING_PCM_16BIT
     */
    Int32 mAudioFormat;

    /**
     * Where the audio data is recorded from.
     */
    Int32 mRecordSource;

    /**
     * Indicates the state of the AudioRecord instance.
     */
    Int32 mState;

    /**
     * Indicates the recording state of the AudioRecord instance.
     */
    Int32 mRecordingState;

    /**
     * Lock to make sure mRecordingState updates are reflecting the actual state of the object.
     */
    Object mRecordingStateLock;

    /**
     * The listener the AudioRecord notifies when the record position reaches a marker
     * or for periodic updates during the progression of the record head.
     *  @see #setRecordPositionUpdateListener(OnRecordPositionUpdateListener)
     *  @see #setRecordPositionUpdateListener(OnRecordPositionUpdateListener, Handler)
     */
    AutoPtr<IAudioRecordOnRecordPositionUpdateListener> mPositionListener;

    /**
     * Lock to protect position listener updates against event notifications
     */
    Object mPositionListenerLock;

    /**
     * Handler for marker events coming from the native code
     */
    AutoPtr<NativeEventHandler> mEventHandler;

    /**
     * Looper associated with the thread that creates the AudioRecord instance
     */
    AutoPtr<ILooper> mInitializationLooper;

    /**
     * Size of the native audio buffer.
     */
    Int32 mNativeBufferSizeInBytes;

    /**
     * Audio session ID
     */
    Int32 mSessionId;

    /**
     * AudioAttributes
     */
    AutoPtr<IAudioAttributes> mAudioAttributes;
    Boolean mIsSubmixFullVolume;

    AutoPtr<IBinder> mICallBack;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIORECORD_H__
