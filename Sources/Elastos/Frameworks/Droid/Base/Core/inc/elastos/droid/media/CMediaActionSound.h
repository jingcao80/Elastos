
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAACTIONSOUND_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAACTIONSOUND_H__

#include "_Elastos_Droid_Media_CMediaActionSound.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaActionSound)
    , public Object
    , public IMediaActionSound
{
private:
    class SoundPoolOnLoadCompleteListener
        : public Object
        , public ISoundPoolOnLoadCompleteListener
    {
    public:
        CAR_INTERFACE_DECL()

        SoundPoolOnLoadCompleteListener(
            /* [in] */ IWeakReference* host);

        CARAPI OnLoadComplete(
            /* [in] */ ISoundPool* soundPool,
            /* [in] */ Int32 sampleId,
            /* [in] */ Int32 status);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CMediaActionSound();

    virtual ~CMediaActionSound();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Construct a new MediaActionSound instance. Only a single instance is
     * needed for playing any platform media action sound; you do not need a
     * separate instance for each sound type.
     */
    CARAPI constructor();

    /**
     * Preload a predefined platform sound to minimize latency when the sound is
     * played later by {@link #play}.
     * @param soundName The type of sound to preload, selected from
     *         SHUTTER_CLICK, FOCUS_COMPLETE, START_VIDEO_RECORDING, or
     *         STOP_VIDEO_RECORDING.
     * @see #play
     * @see #SHUTTER_CLICK
     * @see #FOCUS_COMPLETE
     * @see #START_VIDEO_RECORDING
     * @see #STOP_VIDEO_RECORDING
     */
    CARAPI Load(
        /* [in] */ Int32 soundName);

    /**
     * <p>Play one of the predefined platform sounds for media actions.</p>
     *
     * <p>Use this method to play a platform-specific sound for various media
     * actions. The sound playback is done asynchronously, with the same
     * behavior and content as the sounds played by
     * {@link android.hardware.Camera#takePicture Camera.takePicture},
     * {@link android.media.MediaRecorder#start MediaRecorder.start}, and
     * {@link android.media.MediaRecorder#stop MediaRecorder.stop}.</p>
     *
     * <p>Using this method makes it easy to match the default device sounds
     * when recording or capturing data through the preview callbacks, or when
     * implementing custom camera-like features in your
     * application.</p>
     *
     * <p>If the sound has not been loaded by {@link #load} before calling play,
     * play will load the sound at the cost of some additional latency before
     * sound playback begins. </p>
     *
     * @param soundName The type of sound to play, selected from
     *         SHUTTER_CLICK, FOCUS_COMPLETE, START_VIDEO_RECORDING, or
     *         STOP_VIDEO_RECORDING.
     * @see android.hardware.Camera#takePicture
     * @see android.media.MediaRecorder
     * @see #SHUTTER_CLICK
     * @see #FOCUS_COMPLETE
     * @see #START_VIDEO_RECORDING
     * @see #STOP_VIDEO_RECORDING
     */
    CARAPI Play(
        /* [in] */ Int32 soundName);

    /**
     * Free up all audio resources used by this MediaActionSound instance. Do
     * not call any other methods on a MediaActionSound instance after calling
     * release().
     */
    CARAPI ReleaseResources();

private:
    static const Int32 NUM_MEDIA_SOUND_STREAMS = 1;

    AutoPtr<ISoundPool> mSoundPool;
    AutoPtr<ArrayOf<Int32> > mSoundIds;
    Int32 mSoundIdToPlay;

    static const String SOUND_FILES[];
    static const String TAG;
    static const Int32 SOUND_NOT_LOADED = -1;

    AutoPtr<ISoundPoolOnLoadCompleteListener> mLoadCompleteListener;

};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIAACTIONSOUND_H__
