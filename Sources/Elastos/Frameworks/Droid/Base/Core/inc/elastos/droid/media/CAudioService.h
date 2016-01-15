#ifndef __ELASTOS_DROID_MEDIA_CAUDIOSERVICE_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOSERVICE_H__

#include "_Elastos_Droid_Media_CAudioService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The implementation of the volume manager service.
 * <p>
 * This implementation focuses on delivering a responsive UI. Most methods are
 * asynchronous to external calls. For example, the task of setting a volume
 * will update our internal state, but in a separate thread will set the system
 * volume and later persist to the database. Similarly, setting the ringer mode
 * will update the state and broadcast a change and in a separate thread later
 * persist the ringer mode.
 *
 * @hide
 */
CarClass(CAudioService)
    , public Object
    , public IAudioService
    , public IIAudioService
{
public:
    CAudioService();

    virtual ~CAudioService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Combines all of the attributes that have been set and return a new
     * {@link AudioAttributes} object.
     * @return a new {@link AudioAttributes} object
     */
    // @SuppressWarnings("unchecked") // for cloning of mTags
    CARAPI Build(
        /* [out] */ IAudioAttributes** result);

    /**
     * Sets the attribute describing what is the intended use of the the audio signal,
     * such as alarm or ringtone.
     * @param usage one of {@link AudioAttributes#USAGE_UNKNOWN},
     *     {@link AudioAttributes#USAGE_MEDIA},
     *     {@link AudioAttributes#USAGE_VOICE_COMMUNICATION},
     *     {@link AudioAttributes#USAGE_VOICE_COMMUNICATION_SIGNALLING},
     *     {@link AudioAttributes#USAGE_ALARM}, {@link AudioAttributes#USAGE_NOTIFICATION},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_RINGTONE},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_COMMUNICATION_REQUEST},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_COMMUNICATION_INSTANT},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_COMMUNICATION_DELAYED},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_EVENT},
     *     {@link AudioAttributes#USAGE_ASSISTANCE_ACCESSIBILITY},
     *     {@link AudioAttributes#USAGE_ASSISTANCE_NAVIGATION_GUIDANCE},
     *     {@link AudioAttributes#USAGE_ASSISTANCE_SONIFICATION},
     *     {@link AudioAttributes#USAGE_GAME}.
     * @return the same Builder instance.
     */
    CARAPI SetUsage(
        /* [in] */ Int32 usage);

    /**
     * Sets the attribute describing the content type of the audio signal, such as speech,
     * or music.
     * @param contentType the content type values, one of
     *     {@link AudioAttributes#CONTENT_TYPE_MOVIE},
     *     {@link AudioAttributes#CONTENT_TYPE_MUSIC},
     *     {@link AudioAttributes#CONTENT_TYPE_SONIFICATION},
     *     {@link AudioAttributes#CONTENT_TYPE_SPEECH},
     *     {@link AudioAttributes#CONTENT_TYPE_UNKNOWN}.
     * @return the same Builder instance.
     */
    CARAPI SetContentType(
        /* [in] */ Int32 contentType);

    /**
     * Sets the combination of flags.
     * @param flags the {@link AudioAttributes#FLAG_AUDIBILITY_ENFORCED} flag.
     * @return the same Builder instance.
     */
    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    /**
     * @hide
     * Add a custom tag stored as a string
     * @param tag
     * @return the same Builder instance.
     */
    CARAPI AddTag(
        /* [in] */ const String& tag);

    /**
     * Sets attributes as inferred from the legacy stream types.
     * Use this method when building an {@link AudioAttributes} instance to initialize some of
     * the attributes by information derived from a legacy stream type.
     * @param streamType one of {@link AudioManager#STREAM_VOICE_CALL},
     *   {@link AudioManager#STREAM_SYSTEM}, {@link AudioManager#STREAM_RING},
     *   {@link AudioManager#STREAM_MUSIC}, {@link AudioManager#STREAM_ALARM},
     *    or {@link AudioManager#STREAM_NOTIFICATION}.
     * @return the same Builder instance.
     */
    CARAPI SetLegacyStreamType(
        /* [in] */ Int32 streamType);

    /**
     * @hide
     * For internal framework use only, enables building from hidden stream types.
     * @param streamType
     * @return the same Builder instance.
     */
    CARAPI SetInternalLegacyStreamType(
        /* [in] */ Int32 streamType);

    /**
     * @hide
     * Sets the capture preset.
     * Use this audio attributes configuration method when building an {@link AudioRecord}
     * instance with {@link AudioRecord#AudioRecord(AudioAttributes, AudioFormat, int)}.
     * @param preset one of {@link MediaRecorder.AudioSource#DEFAULT},
     *     {@link MediaRecorder.AudioSource#MIC}, {@link MediaRecorder.AudioSource#CAMCORDER},
     *     {@link MediaRecorder.AudioSource#VOICE_RECOGNITION} or
     *     {@link MediaRecorder.AudioSource#VOICE_COMMUNICATION}.
     * @return the same Builder instance.
     */
    CARAPI SetCapturePreset(
        /* [in] */ Int32 preset);

    /**
     * @hide
     * Same as {@link #setCapturePreset(int)} but authorizes the use of HOTWORD and
     * REMOTE_SUBMIX.
     * @param preset
     * @return the same Builder instance.
     */
    CARAPI SetInternalCapturePreset(
        /* [in] */ Int32 preset);

private:
    Int32 mUsage;
    Int32 mContentType;
    Int32 mSource;
    Int32 mFlags;
    AutoPtr<IHashSet> mTags;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOSERVICE_H__
