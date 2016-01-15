
#ifndef __ELASTOS_DROID_MEDIA_CRINGTONE_H__
#define __ELASTOS_DROID_MEDIA_CRINGTONE_H__

#include "_Elastos_Droid_Media_CRingtone.h"
#include "elastos/droid/ext/frameworkext.h"
#include "CRingtoneManager.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::ISurfaceHolder;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Ringtone provides a quick method for playing a ringtone, notification, or
 * other similar types of sounds.
 * <p>
 * For ways of retrieving {@link Ringtone} objects or to show a ringtone
 * picker, see {@link RingtoneManager}.
 *
 * @see RingtoneManager
 */
CarClass(CRingtone)
{
public:
    CRingtone();

    ~CRingtone();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean allowRemote);

    /**
     * Sets the stream type where this ringtone will be played.
     *
     * @param streamType The stream, see {@link AudioManager}.
     */
    CARAPI SetStreamType(
        /* [in] */ Int32 streamType);

    /**
     * Gets the stream type where this ringtone will be played.
     *
     * @return The stream type, see {@link AudioManager}.
     */
    CARAPI GetStreamType(
        /* [out] */ Int32* type);

    /**
     * Returns a human-presentable title for ringtone. Looks in media and DRM
     * content providers. If not in either, uses the filename
     *
     * @param context A context used for querying.
     */
    CARAPI GetTitle(
        /* [in] */ IContext* context,
        /* [out] */ String* title);

    /**
     * Set {@link Uri} to be used for ringtone playback. Attempts to open
     * locally, otherwise will delegate playback to remote
     * {@link IRingtonePlayer}.
     *
     * @hide
     */
    CARAPI SetUri(
        /* [in] */ IUri* uri);

    /** {@hide} */
    CARAPI GetUri(
        /* [out] */ IUri** result);

    CARAPI Play();

    CARAPI Stop();

    CARAPI IsPlaying(
        /* [out] */ Boolean* playing);

private:
    CARAPI_(String) GetTitle(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean followSettingsUri);

    CARAPI_(void) SetTitle(
        /* [in] */ const String& title);

    CARAPI_(void) DestroyLocalPlayer();

private:
    static const String TAG;
    static const Boolean LOGD;// = true;

    static AutoPtr<ArrayOf<String> > MEDIA_COLUMNS;
    static AutoPtr<ArrayOf<String> > DRM_COLUMNS;

    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
    Boolean mAllowRemote;
    AutoPtr<IIRingtonePlayer> mRemotePlayer;
    AutoPtr<IBinder> mRemoteToken;

    AutoPtr<IMediaPlayer> mLocalPlayer;

    AutoPtr<IUri> mUri;
    String mTitle;

    Int32 mStreamType;// = AudioManager.STREAM_RING;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CRINGTONE_H__
