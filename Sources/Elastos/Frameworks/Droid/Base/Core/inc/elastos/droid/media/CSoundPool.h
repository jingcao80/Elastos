
#ifndef __ELASTOS_DROID_MEDIA_CSOUNDPOOL_H__
#define __ELASTOS_DROID_MEDIA_CSOUNDPOOL_H__

#include "_Elastos_Droid_Media_CSoundPool.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The SoundPool class manages and plays audio resources for applications.
 *
 * <p>A SoundPool is a collection of samples that can be loaded into memory
 * from a resource inside the APK or from a file in the file system. The
 * SoundPool library uses the MediaPlayer service to decode the audio
 * into a raw 16-bit PCM mono or stereo stream. This allows applications
 * to ship with compressed streams without having to suffer the CPU load
 * and latency of decompressing during playback.</p>
 *
 * <p>In addition to low-latency playback, SoundPool can also manage the number
 * of audio streams being rendered at once. When the SoundPool object is
 * constructed, the maxStreams parameter sets the maximum number of streams
 * that can be played at a time from this single SoundPool. SoundPool tracks
 * the number of active streams. If the maximum number of streams is exceeded,
 * SoundPool will automatically stop a previously playing stream based first
 * on priority and then by age within that priority. Limiting the maximum
 * number of streams helps to cap CPU loading and reducing the likelihood that
 * audio mixing will impact visuals or UI performance.</p>
 *
 * <p>Sounds can be looped by setting a non-zero loop value. A value of -1
 * causes the sound to loop forever. In this case, the application must
 * explicitly call the stop() function to stop the sound. Any other non-zero
 * value will cause the sound to repeat the specified number of times, e.g.
 * a value of 3 causes the sound to play a total of 4 times.</p>
 *
 * <p>The playback rate can also be changed. A playback rate of 1.0 causes
 * the sound to play at its original frequency (resampled, if necessary,
 * to the hardware output frequency). A playback rate of 2.0 causes the
 * sound to play at twice its original frequency, and a playback rate of
 * 0.5 causes it to play at half its original frequency. The playback
 * rate range is 0.5 to 2.0.</p>
 *
 * <p>Priority runs low to high, i.e. higher numbers are higher priority.
 * Priority is used when a call to play() would cause the number of active
 * streams to exceed the value established by the maxStreams parameter when
 * the SoundPool was created. In this case, the stream allocator will stop
 * the lowest priority stream. If there are multiple streams with the same
 * low priority, it will choose the oldest stream to stop. In the case
 * where the priority of the new stream is lower than all the active
 * streams, the new sound will not play and the play() function will return
 * a streamID of zero.</p>
 *
 * <p>Let's examine a typical use case: A game consists of several levels of
 * play. For each level, there is a set of unique sounds that are used only
 * by that level. In this case, the game logic should create a new SoundPool
 * object when the first level is loaded. The level data itself might contain
 * the list of sounds to be used by this level. The loading logic iterates
 * through the list of sounds calling the appropriate SoundPool.load()
 * function. This should typically be done early in the process to allow time
 * for decompressing the audio to raw PCM format before they are needed for
 * playback.</p>
 *
 * <p>Once the sounds are loaded and play has started, the application can
 * trigger sounds by calling SoundPool.play(). Playing streams can be
 * paused or resumed, and the application can also alter the pitch by
 * adjusting the playback rate in real-time for doppler or synthesis
 * effects.</p>
 *
 * <p>Note that since streams can be stopped due to resource constraints, the
 * streamID is a reference to a particular instance of a stream. If the stream
 * is stopped to allow a higher priority stream to play, the stream is no
 * longer be valid. However, the application is allowed to call methods on
 * the streamID without error. This may help simplify program logic since
 * the application need not concern itself with the stream lifecycle.</p>
 *
 * <p>In our example, when the player has completed the level, the game
 * logic should call SoundPool.release() to release all the native resources
 * in use and then set the SoundPool reference to null. If the player starts
 * another level, a new SoundPool is created, sounds are loaded, and play
 * resumes.</p>
 */
CarClass(CSoundPool)
    , public Object
    , public ISoundPool
{
public:
    /**
     * Builder class for {@link SoundPool} objects.
     */
    class Builder
        : public Object
        , public ISoundPoolBuilder
    {
        friend class CSoundPool;
    public:
        Builder();

        ~Builder();

        CAR_INTERFACE_DECL()

        CARAPI SetMaxStreams(
            /* [in] */ Int32 maxStreams);

        CARAPI SetAudioAttributes(
            /* [in] */ IAudioAttributes * attributes);

        CARAPI Build(
            /* [out] */ ISoundPool** result);

    private:
        Int32 mMaxStreams;
        AutoPtr<IAudioAttributes> mAudioAttributes;
    };

    class SoundPoolImpl
        : public Object
        , public ISoundPoolDelegate
    {
        friend class CSoundPool;
    private:
        class EventHandler
            : public Handler
        {
            friend class CSoundPool;
            friend class SoundPoolImpl;
        public:
            TO_STRING_IMPL("CSoundPool::EventHandler")

            EventHandler(
                /* [in] */ CSoundPool* soundPool,
                /* [in] */ SoundPoolImpl* host,
                /* [in] */ ILooper* looper);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            CSoundPool* mSoundPool;
            SoundPoolImpl*  mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        SoundPoolImpl();

        ~SoundPoolImpl();

        CARAPI constructor(
            /* [in] */ CSoundPool* proxy,
            /* [in] */ Int32 maxStreams,
            /* [in] */ IAudioAttributes* attr);

        CARAPI Load(
            /* [in] */ const String& path,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Load(
            /* [in] */ IContext* context,
            /* [in] */ Int32 resId,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Load(
            /* [in] */ IAssetFileDescriptor* afd,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Load(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ Int64 offset,
            /* [in] */ Int64 length,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Unload(
            /* [in] */ Int32 soundID,
            /* [out] */ Boolean* result);

        CARAPI Play(
            /* [in] */ Int32 soundID,
            /* [in] */ Float leftVolume,
            /* [in] */ Float rightVolume,
            /* [in] */ Int32 priority,
            /* [in] */ Int32 loop,
            /* [in] */ Float rate,
            /* [out] */ Int32* result);

        CARAPI _Play(
            /* [in] */ Int32 soundID,
            /* [in] */ Float leftVolume,
            /* [in] */ Float rightVolume,
            /* [in] */ Int32 priority,
            /* [in] */ Int32 loop,
            /* [in] */ Float rate,
            /* [out] */ Int32* result);

        CARAPI Pause(
            /* [in] */ Int32 streamID);

        CARAPI Resume(
            /* [in] */ Int32 streamID);

        CARAPI AutoPause();

        CARAPI AutoResume();

        CARAPI Stop(
            /* [in] */ Int32 streamID);

        CARAPI SetVolume(
            /* [in] */ Int32 streamID,
            /* [in] */ Float leftVolume,
            /* [in] */ Float rightVolume);

        CARAPI SetVolume(
            /* [in] */ Int32 streamID,
            /* [in] */ Float volume);

        CARAPI SetPriority(
            /* [in] */ Int32 streamID,
            /* [in] */ Int32 priority);

        CARAPI SetLoop(
            /* [in] */ Int32 streamID,
            /* [in] */ Int32 loop);

        CARAPI SetRate(
            /* [in] */ Int32 streamID,
            /* [in] */ Float rate);

        CARAPI SetOnLoadCompleteListener(
            /* [in] */ ISoundPoolOnLoadCompleteListener* listener);

        CARAPI ReleaseSoundPoolImpl();

        CARAPI ReleaseSoundPoolDelegate();

        // post event from native code to message handler
        static CARAPI_(void) PostEventFromNative(
            /* [in] */ IWeakReference* weakRef,
            /* [in] */ Int32 msg,
            /* [in] */ Int32 arg1,
            /* [in] */ Int32 arg2,
            /* [in] */ IInterface* obj);

    private:
        CARAPI_(Int32) _Load(
            /* [in] */ const String& uri,
            /* [in] */ Int32 priority);

        CARAPI_(Int32) _Load(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ Int64 offset,
            /* [in] */ Int64 length,
            /* [in] */ Int32 priority);

        CARAPI_(Boolean) IsRestricted();

        CARAPI_(void) _SetVolume(
            /* [in] */ Int32 streamID,
            /* [in] */ Float leftVolume,
            /* [in] */ Float rightVolume);

        CARAPI_(Int32) Native_setup(
            /* [in] */ IWeakReference* weakRef,
            /* [in] */ Int32 maxStreams,
            /* [in] */ IAudioAttributes* attributes/*AudioAttributes*/);

    protected:
        CARAPI_(void) Finalize();

    public:
        static ECode sStaticLoadLibrary;

        // SoundPool messages
        //
        // must match SoundPool.h
        static const Int32 SAMPLE_LOADED;

    private:
        static String TAG;
        static Boolean DEBUG;
        Int64 mNativeContext; // accessed by native methods

        AutoPtr<EventHandler> mEventHandler;
        AutoPtr<ISoundPoolOnLoadCompleteListener> mOnLoadCompleteListener;
        CSoundPool* mProxy;

        Object mLock;
        AutoPtr<IAudioAttributes> mAttributes;
        AutoPtr<IIAppOpsService> mAppOps;
    };

    class SoundPoolStub
        : public Object
        , public ISoundPoolDelegate
    {
    public:
        SoundPoolStub();

        ~SoundPoolStub();

        CAR_INTERFACE_DECL()

        CARAPI Load(
            /* [in] */ const String& path,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Load(
            /* [in] */ IContext* context,
            /* [in] */ Int32 resId,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Load(
            /* [in] */ IAssetFileDescriptor* afd,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Load(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ Int64 offset,
            /* [in] */ Int64 length,
            /* [in] */ Int32 priority,
            /* [out] */ Int32* result);

        CARAPI Unload(
            /* [in] */ Int32 soundID,
            /* [out] */ Boolean* result);

        CARAPI Play(
            /* [in] */ Int32 soundID,
            /* [in] */ Float leftVolume,
            /* [in] */ Float rightVolume,
            /* [in] */ Int32 priority,
            /* [in] */ Int32 loop,
            /* [in] */ Float rate,
            /* [out] */ Int32* result);

        CARAPI Pause(
            /* [in] */ Int32 streamID);

        CARAPI Resume(
            /* [in] */ Int32 streamID);

        CARAPI AutoPause();

        CARAPI AutoResume();

        CARAPI Stop(
            /* [in] */ Int32 streamID);

        CARAPI SetVolume(
            /* [in] */ Int32 streamID,
            /* [in] */ Float leftVolume,
            /* [in] */ Float rightVolume);

        CARAPI SetVolume(
            /* [in] */ Int32 streamID,
            /* [in] */ Float volume);

        CARAPI SetPriority(
            /* [in] */ Int32 streamID,
            /* [in] */ Int32 priority);

        CARAPI SetLoop(
            /* [in] */ Int32 streamID,
            /* [in] */ Int32 loop);

        CARAPI SetRate(
            /* [in] */ Int32 streamID,
            /* [in] */ Float rate);

        CARAPI SetOnLoadCompleteListener(
            /* [in] */ ISoundPoolOnLoadCompleteListener* listener);

        CARAPI ReleaseSoundPoolStub();

        //for compile
        CARAPI ReleaseSoundPoolDelegate();
    };

public:
    CSoundPool();

    ~CSoundPool();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructor. Constructs a SoundPool object with the following
     * characteristics:
     *
     * @param maxStreams the maximum number of simultaneous streams for this
     *                   SoundPool object
     * @param streamType the audio stream type as described in AudioManager
     *                   For example, game applications will normally use
     *                   {@link AudioManager#STREAM_MUSIC}.
     * @param srcQuality the sample-rate converter quality. Currently has no
     *                   effect. Use 0 for the default.
     * @return a SoundPool object, or NULL if creation failed
     * @deprecated use {@link SoundPool.Builder} instead to create and configure a
     *     SoundPool instance
     */
    CARAPI constructor(
        /* [in] */ Int32 maxStreams,
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 srcQuality);

    CARAPI constructor(
        /* [in] */ Int32 maxStreams,
        /* [in] */ IAudioAttributes* attributes);

    /**
     * Load the sound from the specified path.
     *
     * @param path the path to the audio file
     * @param priority the priority of the sound. Currently has no effect. Use
     *                 a value of 1 for future compatibility.
     * @return a sound ID. This value can be used to play or unload the sound.
     */
    CARAPI Load(
        /* [in] */ const String& path,
        /* [in] */ Int32 priority,
        /* [out] */ Int32* result);

    /**
     * Load the sound from the specified APK resource.
     *
     * Note that the extension is dropped. For example, if you want to load
     * a sound from the raw resource file "explosion.mp3", you would specify
     * "R.raw.explosion" as the resource ID. Note that this means you cannot
     * have both an "explosion.wav" and an "explosion.mp3" in the res/raw
     * directory.
     *
     * @param context the application context
     * @param resId the resource ID
     * @param priority the priority of the sound. Currently has no effect. Use
     *                 a value of 1 for future compatibility.
     * @return a sound ID. This value can be used to play or unload the sound.
     */
    CARAPI Load(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId,
        /* [in] */ Int32 priority,
        /* [out] */ Int32* result);

    /**
     * Load the sound from an asset file descriptor.
     *
     * @param afd an asset file descriptor
     * @param priority the priority of the sound. Currently has no effect. Use
     *                 a value of 1 for future compatibility.
     * @return a sound ID. This value can be used to play or unload the sound.
     */
    CARAPI Load(
        /* [in] */ IAssetFileDescriptor* afd,
        /* [in] */ Int32 priority,
        /* [out] */ Int32* result);

    /**
     * Load the sound from a FileDescriptor.
     *
     * This version is useful if you store multiple sounds in a single
     * binary. The offset specifies the offset from the start of the file
     * and the length specifies the length of the sound within the file.
     *
     * @param fd a FileDescriptor object
     * @param offset offset to the start of the sound
     * @param length length of the sound
     * @param priority the priority of the sound. Currently has no effect. Use
     *                 a value of 1 for future compatibility.
     * @return a sound ID. This value can be used to play or unload the sound.
     */
    CARAPI Load(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length,
        /* [in] */ Int32 priority,
        /* [out] */ Int32* result);

    /**
     * Unload a sound from a sound ID.
     *
     * Unloads the sound specified by the soundID. This is the value
     * returned by the load() function. Returns true if the sound is
     * successfully unloaded, false if the sound was already unloaded.
     *
     * @param soundID a soundID returned by the load() function
     * @return true if just unloaded, false if previously unloaded
     */
    CARAPI Unload(
        /* [in] */ Int32 soundID,
        /* [out] */ Boolean* result);

    /**
     * Play a sound from a sound ID.
     *
     * Play the sound specified by the soundID. This is the value
     * returned by the load() function. Returns a non-zero streamID
     * if successful, zero if it fails. The streamID can be used to
     * further control playback. Note that calling play() may cause
     * another sound to stop playing if the maximum number of active
     * streams is exceeded. A loop value of -1 means loop forever,
     * a value of 0 means don't loop, other values indicate the
     * number of repeats, e.g. a value of 1 plays the audio twice.
     * The playback rate allows the application to vary the playback
     * rate (pitch) of the sound. A value of 1.0 means play back at
     * the original frequency. A value of 2.0 means play back twice
     * as fast, and a value of 0.5 means playback at half speed.
     *
     * @param soundID a soundID returned by the load() function
     * @param leftVolume left volume value (range = 0.0 to 1.0)
     * @param rightVolume right volume value (range = 0.0 to 1.0)
     * @param priority stream priority (0 = lowest priority)
     * @param loop loop mode (0 = no loop, -1 = loop forever)
     * @param rate playback rate (1.0 = normal playback, range 0.5 to 2.0)
     * @return non-zero streamID if successful, zero if failed
     */
    CARAPI Play(
        /* [in] */ Int32 soundID,
        /* [in] */ Float leftVolume,
        /* [in] */ Float rightVolume,
        /* [in] */ Int32 priority,
        /* [in] */ Int32 loop,
        /* [in] */ Float rate,
        /* [out] */ Int32* result);

    /**
     * Pause a playback stream.
     *
     * Pause the stream specified by the streamID. This is the
     * value returned by the play() function. If the stream is
     * playing, it will be paused. If the stream is not playing
     * (e.g. is stopped or was previously paused), calling this
     * function will have no effect.
     *
     * @param streamID a streamID returned by the play() function
     */
    CARAPI Pause(
        /* [in] */ Int32 streamID);

    /**
     * Resume a playback stream.
     *
     * Resume the stream specified by the streamID. This
     * is the value returned by the play() function. If the stream
     * is paused, this will resume playback. If the stream was not
     * previously paused, calling this function will have no effect.
     *
     * @param streamID a streamID returned by the play() function
     */
    CARAPI Resume(
        /* [in] */ Int32 streamID);

    /**
     * Pause all active streams.
     *
     * Pause all streams that are currently playing. This function
     * iterates through all the active streams and pauses any that
     * are playing. It also sets a flag so that any streams that
     * are playing can be resumed by calling autoResume().
     */
    CARAPI AutoPause();

    /**
     * Resume all previously active streams.
     *
     * Automatically resumes all streams that were paused in previous
     * calls to autoPause().
     */
    CARAPI AutoResume();

    /**
     * Stop a playback stream.
     *
     * Stop the stream specified by the streamID. This
     * is the value returned by the play() function. If the stream
     * is playing, it will be stopped. It also releases any native
     * resources associated with this stream. If the stream is not
     * playing, it will have no effect.
     *
     * @param streamID a streamID returned by the play() function
     */
    CARAPI Stop(
        /* [in] */ Int32 streamID);

    /**
     * Set stream volume.
     *
     * Sets the volume on the stream specified by the streamID.
     * This is the value returned by the play() function. The
     * value must be in the range of 0.0 to 1.0. If the stream does
     * not exist, it will have no effect.
     *
     * @param streamID a streamID returned by the play() function
     * @param leftVolume left volume value (range = 0.0 to 1.0)
     * @param rightVolume right volume value (range = 0.0 to 1.0)
     */
    CARAPI SetVolume(
        /* [in] */ Int32 streamID,
        /* [in] */ Float leftVolume,
        /* [in] */ Float rightVolume);

    CARAPI SetVolume(
       /* [in] */ Int32 streamID,
       /* [in] */ Float volume);

    /**
     * Change stream priority.
     *
     * Change the priority of the stream specified by the streamID.
     * This is the value returned by the play() function. Affects the
     * order in which streams are re-used to play new sounds. If the
     * stream does not exist, it will have no effect.
     *
     * @param streamID a streamID returned by the play() function
     */
    CARAPI SetPriority(
        /* [in] */ Int32 streamID,
        /* [in] */ Int32 priority);

    /**
     * Set loop mode.
     *
     * Change the loop mode. A loop value of -1 means loop forever,
     * a value of 0 means don't loop, other values indicate the
     * number of repeats, e.g. a value of 1 plays the audio twice.
     * If the stream does not exist, it will have no effect.
     *
     * @param streamID a streamID returned by the play() function
     * @param loop loop mode (0 = no loop, -1 = loop forever)
     */
    CARAPI SetLoop(
        /* [in] */ Int32 streamID,
        /* [in] */ Int32 loop);

    /**
     * Change playback rate.
     *
     * The playback rate allows the application to vary the playback
     * rate (pitch) of the sound. A value of 1.0 means playback at
     * the original frequency. A value of 2.0 means playback twice
     * as fast, and a value of 0.5 means playback at half speed.
     * If the stream does not exist, it will have no effect.
     *
     * @param streamID a streamID returned by the play() function
     * @param rate playback rate (1.0 = normal playback, range 0.5 to 2.0)
     */
    CARAPI SetRate(
        /* [in] */ Int32 streamID,
        /* [in] */ Float rate);

    /**
     * Sets the callback hook for the OnLoadCompleteListener.
     */
    CARAPI SetOnLoadCompleteListener(
        /* [in] */ ISoundPoolOnLoadCompleteListener* listener);

    /**
     * Release the SoundPool resources.
     *
     * Release all memory and native resources used by the SoundPool
     * object. The SoundPool can no longer be used and the reference
     * should be set to null.
     */
    CARAPI ReleaseResources();

protected:
    CARAPI_(void) Finalize();

public:
    static const String TAG;
    static const Boolean DEBUG;

private:
    AutoPtr<ISoundPoolDelegate> mImpl;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CSOUNDPOOL_H__
