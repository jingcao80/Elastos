#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONLEGACYHELPER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONLEGACYHELPER_H__

#include "_Elastos_Droid_Media_Session_CMediaSessionLegacyHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/session/MediaSessionCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Helper for connecting existing APIs up to the new session APIs. This can be
 * used by RCC, AudioFocus, etc. to create a single session that translates to
 * all those components.
 *
 * @hide
 */
CarClass(CMediaSessionLegacyHelper)
    , public Object
    , public IMediaSessionLegacyHelper
{
private:
    class MediaButtonListener
        : public MediaSessionCallback
    {
    public:
        MediaButtonListener(
            /* [in] */ IPendingIntent * pi,
            /* [in] */ IContext * context);

        CARAPI OnMediaButtonEvent(
            /* [in] */ IIntent * mediaButtonIntent,
            /* [out] */ Boolean * result);

        CARAPI OnPlay();

        CARAPI OnPause();

        CARAPI OnSkipToNext();

        CARAPI OnSkipToPrevious();

        CARAPI OnFastForward();

        CARAPI OnRewind();

        CARAPI OnStop();

    private:
        CARAPI_(void) SendKeyEvent(
            /* [in] */ Int32 keyCode);

    private:
        AutoPtr<IPendingIntent> mPendingIntent;
        AutoPtr<IContext> mContext;
    };

    class SessionHolder
        : public Object
    {
    private:
        class SessionCallback
            : public MediaSessionCallback
        {
        public:
            SessionCallback(
                /* [in] */ SessionHolder * host)
                : mHost(host)
            {}

            CARAPI OnMediaButtonEvent(
                /* [in] */ IIntent * mediaButtonIntent,
                /* [out] */ Boolean * result);

            CARAPI OnPlay();

            CARAPI OnPause();

            CARAPI OnSkipToNext();

            CARAPI OnSkipToPrevious();

            CARAPI OnFastForward();

            CARAPI OnRewind();

            CARAPI OnStop();

            CARAPI OnSeekTo(
                /* [in] */ Int64 pos);

            CARAPI OnSetRating(
                /* [in] */ IRating * rating);

            // @Override
            CARAPI SetBrowsedPlayer();

            // @Override
            CARAPI SetPlayItem(
                /* [in] */ Int32 scope,
                /* [in] */ Int64 uid);

            // @Override
            CARAPI GetNowPlayingEntries();

        private:
            SessionHolder * mHost;
        };

    public:
        SessionHolder(
            /* [in] */ IMediaSession * session,
            /* [in] */ IPendingIntent * pi,
            /* [in] */ CMediaSessionLegacyHelper * host);

        CARAPI Update();

    public:
        AutoPtr<IMediaSession> mSession;
        AutoPtr<IPendingIntent> mPi;
        AutoPtr<MediaButtonListener> mMediaButtonListener;
        AutoPtr<IMediaSessionCallback> mRccListener;
        Int32 mFlags;
        AutoPtr<SessionCallback> mCb;

    private:
        CMediaSessionLegacyHelper * mHost;
    };

public:
    CMediaSessionLegacyHelper();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaSessionLegacyHelper();

    CARAPI constructor(
        /* [in] */ IContext * context);

    CARAPI GetSession(
        /* [in] */ IPendingIntent * pi,
        /* [out] */ IMediaSession ** reseult);

    CARAPI SendMediaButtonEvent(
        /* [in] */ IKeyEvent * keyEvent,
        /* [in] */ Boolean needWakeLock);

    CARAPI SendVolumeKeyEvent(
        /* [in] */ IKeyEvent * keyEvent,
        /* [in] */ Boolean musicOnly);

    CARAPI SendAdjustVolumeBy(
        /* [in] */ Int32 suggestedStream,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 flags);

    CARAPI IsGlobalPriorityActive(
        /* [out] */ Boolean * result);

    CARAPI AddRccListener(
        /* [in] */ IPendingIntent * pi,
        /* [in] */ IMediaSessionCallback * listener);

    CARAPI RemoveRccListener(
        /* [in] */ IPendingIntent * pi);

    CARAPI AddMediaButtonListener(
        /* [in] */ IPendingIntent * pi,
        /* [in] */ IComponentName * mbrComponent,
        /* [in] */ IContext * context);

    CARAPI RemoveMediaButtonListener(
        /* [in] */ IPendingIntent * pi);

    static CARAPI GetHelper(
        /* [in] */ IContext * context,
        /* [out] */ IMediaSessionLegacyHelper ** result);

    static CARAPI GetOldMetadata(
        /* [in] */ IMediaMetadata * metadata,
        /* [in] */ Int32 artworkWidth,
        /* [in] */ Int32 artworkHeight,
        /* [out] */ IBundle ** result);

private:
    /**
     * Scale a bitmap to fit the smallest dimension by uniformly scaling the
     * incoming bitmap. If the bitmap fits, then do nothing and return the
     * original.
     *
     * @param bitmap
     * @param maxWidth
     * @param maxHeight
     * @return
     */
    static CARAPI_(AutoPtr<IBitmap>) ScaleBitmapIfTooBig(
        /* [in] */ IBitmap * bitmap,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight);

    CARAPI_(AutoPtr<SessionHolder>) GetHolder(
        /* [in] */ IPendingIntent * pi,
        /* [in] */ Boolean createIfMissing);

    static CARAPI_(void) SendKeyEvent(
        /* [in] */ IPendingIntent * pi,
        /* [in] */ IContext * context,
        /* [in] */ IIntent * intent);

private:
    static String TAG;
    static Boolean DEBUG;
    static Object sLock;
    static AutoPtr<IMediaSessionLegacyHelper> sInstance;

    AutoPtr<IContext> mContext;
    AutoPtr<IMediaSessionManager> mSessionManager;
    AutoPtr<IHandler> mHandler;
    // The legacy APIs use PendingIntents to register/unregister media button
    // receivers and these are associated with RCC.
    AutoPtr<IArrayMap> mSessions;

};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONLEGACYHELPER_H__
