#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONMANAGER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONMANAGER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Media_Session_CMediaSessionManager.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IIRemoteVolumeController;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Provides support for interacting with {@link MediaSession media sessions}
 * that applications have published to express their ongoing media playback
 * state.
 * <p>
 * Use <code>Context.getSystemService(Context.MEDIA_SESSION_SERVICE)</code> to
 * get an instance of this class.
 *
 * @see MediaSession
 * @see MediaController
 */
CarClass(CMediaSessionManager)
    , public Object
    , public IMediaSessionManager
{
private:
    class SessionsChangedWrapper
        : public Object
    {
    public:
        SessionsChangedWrapper(
            /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * listener,
            /* [in] */ IHandler * handler,
            /* [in] */ CMediaSessionManager * host,
            /* [in] */ IContext * context);

    public:
        AutoPtr<IMediaSessionManagerOnActiveSessionsChangedListener> mListener;
        AutoPtr<IHandler> mHandler;
        AutoPtr<IIActiveSessionsListener> mStub;
        AutoPtr<IContext> mContext;
        CMediaSessionManager * mHost;
    };

    class ActiveSessionsListener
        : public Object
        , public IIActiveSessionsListener
    {
    public:
        CAR_INTERFACE_DECL()

        ActiveSessionsListener(
            /* [in] */ SessionsChangedWrapper * host,
            /* [in] */ IContext * context,
            /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * listener,
            /* [in] */ IHandler * handler)
            : mHost(host)
            , mContext(context)
            , mListener(listener)
            , mHandler(handler)
        {}

        CARAPI OnActiveSessionsChanged(
            /* [in] */ IList * sessions);

    public:
        SessionsChangedWrapper * mHost;
        AutoPtr<IContext> mContext;
        AutoPtr<IMediaSessionManagerOnActiveSessionsChangedListener> mListener;
        AutoPtr<IHandler> mHandler;
    };

    class ActiveSessionsChangedRunnable
        : public Runnable
    {
    public:
        ActiveSessionsChangedRunnable(
            /* [in] */ ActiveSessionsListener * host,
            /* [in] */ IContext * context,
            /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * listener,
            /* [in] */ IHandler * handler,
            /* [in] */ IList * list)
            : mHost(host)
            , mContext(context)
            , mListener(listener)
            , mHandler(handler)
            , mTokens(list)
        {}

        CARAPI Run();

    private:
        ActiveSessionsListener * mHost;
        AutoPtr<IContext> mContext;
        AutoPtr<IMediaSessionManagerOnActiveSessionsChangedListener> mListener;
        AutoPtr<IHandler> mHandler;
        AutoPtr<IList> mTokens;
    };

public:
    CMediaSessionManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaSessionManager();

    CARAPI constructor(
        /* [in] */ IContext * context);

    /**
     * Create a new session in the system and get the binder for it.
     *
     * @param tag A short name for debugging purposes.
     * @return The binder object from the system
     * @hide
     */
    CARAPI CreateSession(
        /* [in] */ IISessionCallback * cbStub,
        /* [in] */ const String& tag,
        /* [in] */ Int32 userId,
        /* [out] */ IISession ** result);

    /**
     * Get a list of controllers for all ongoing sessions. The controllers will
     * be provided in priority order with the most important controller at index
     * 0.
     * <p>
     * This requires the android.Manifest.permission.MEDIA_CONTENT_CONTROL
     * permission be held by the calling app. You may also retrieve this list if
     * your app is an enabled notification listener using the
     * {@link NotificationListenerService} APIs, in which case you must pass the
     * {@link ComponentName} of your enabled listener.
     *
     * @param notificationListener The enabled notification listener component.
     *            May be null.
     * @return A list of controllers for ongoing sessions.
     */
    CARAPI GetActiveSessions(
        /* [in] */ IComponentName * notificationListener,
        /* [out] */ IList ** result);

    /**
     * Get active sessions for a specific user. To retrieve actions for a user
     * other than your own you must hold the
     * {@link android.Manifest.permission#INTERACT_ACROSS_USERS_FULL} permission
     * in addition to any other requirements. If you are an enabled notification
     * listener you may only get sessions for the users you are enabled for.
     *
     * @param notificationListener The enabled notification listener component.
     *            May be null.
     * @param userId The user id to fetch sessions for.
     * @return A list of controllers for ongoing sessions.
     * @hide
     */
    CARAPI GetActiveSessionsForUser(
        /* [in] */ IComponentName * notificationListener,
        /* [in] */ Int32 userId,
        /* [out] */ IList ** result);

    /**
     * Add a listener to be notified when the list of active sessions
     * changes.This requires the
     * android.Manifest.permission.MEDIA_CONTENT_CONTROL permission be held by
     * the calling app. You may also retrieve this list if your app is an
     * enabled notification listener using the
     * {@link NotificationListenerService} APIs, in which case you must pass the
     * {@link ComponentName} of your enabled listener. Updates will be posted to
     * the thread that registered the listener.
     *
     * @param sessionListener The listener to add.
     * @param notificationListener The enabled notification listener component.
     *            May be null.
     */
    CARAPI AddOnActiveSessionsChangedListener(
        /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * sessionListener,
        /* [in] */ IComponentName * notificationListener);

    /**
     * Add a listener to be notified when the list of active sessions
     * changes.This requires the
     * android.Manifest.permission.MEDIA_CONTENT_CONTROL permission be held by
     * the calling app. You may also retrieve this list if your app is an
     * enabled notification listener using the
     * {@link NotificationListenerService} APIs, in which case you must pass the
     * {@link ComponentName} of your enabled listener. Updates will be posted to
     * the handler specified or to the caller's thread if the handler is null.
     *
     * @param sessionListener The listener to add.
     * @param notificationListener The enabled notification listener component.
     *            May be null.
     * @param handler The handler to post events to.
     */
    CARAPI AddOnActiveSessionsChangedListener(
        /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * sessionListener,
        /* [in] */ IComponentName * notificationListener,
        /* [in] */ IHandler * handler);

    /**
     * Add a listener to be notified when the list of active sessions
     * changes.This requires the
     * android.Manifest.permission.MEDIA_CONTENT_CONTROL permission be held by
     * the calling app. You may also retrieve this list if your app is an
     * enabled notification listener using the
     * {@link NotificationListenerService} APIs, in which case you must pass the
     * {@link ComponentName} of your enabled listener.
     *
     * @param sessionListener The listener to add.
     * @param notificationListener The enabled notification listener component.
     *            May be null.
     * @param userId The userId to listen for changes on.
     * @param handler The handler to post updates on.
     * @hide
     */
    CARAPI AddOnActiveSessionsChangedListener(
        /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * sessionListener,
        /* [in] */ IComponentName * notificationListener,
        /* [in] */ Int32 userId,
        /* [in] */ IHandler * handler);

    /**
     * Stop receiving active sessions updates on the specified listener.
     *
     * @param listener The listener to remove.
     */
    CARAPI RemoveOnActiveSessionsChangedListener(
        /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * listener);

    /**
     * Set the remote volume controller to receive volume updates on. Only for
     * use by system UI.
     *
     * @param rvc The volume controller to receive updates on.
     * @hide
     */
    CARAPI SetRemoteVolumeController(
        /* [in] */ IIRemoteVolumeController * rvc);

    /**
     * Send a media key event. The receiver will be selected automatically.
     *
     * @param keyEvent The KeyEvent to send.
     * @hide
     */
    CARAPI DispatchMediaKeyEvent(
        /* [in] */ IKeyEvent * keyEvent);

    /**
     * Send a media key event. The receiver will be selected automatically.
     *
     * @param keyEvent The KeyEvent to send.
     * @param needWakeLock True if a wake lock should be held while sending the key.
     * @hide
     */
    CARAPI DispatchMediaKeyEvent(
        /* [in] */ IKeyEvent * keyEvent,
        /* [in] */ Boolean needWakeLock);

    /**
     * Dispatch an adjust volume request to the system. It will be sent to the
     * most relevant audio stream or media session. The direction must be one of
     * {@link AudioManager#ADJUST_LOWER}, {@link AudioManager#ADJUST_RAISE},
     * {@link AudioManager#ADJUST_SAME}.
     *
     * @param suggestedStream The stream to fall back to if there isn't a
     *            relevant stream
     * @param direction The direction to adjust volume in.
     * @param flags Any flags to include with the volume change.
     * @hide
     */
    CARAPI DispatchAdjustVolume(
        /* [in] */ Int32 suggestedStream,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags);

    /**
     * Check if the global priority session is currently active. This can be
     * used to decide if media keys should be sent to the session or to the app.
     *
     * @hide
     */
    CARAPI IsGlobalPriorityActive(
        /* [out] */ Boolean * result);

public:
    IContext* mContext;

private:
    const static String TAG;

    AutoPtr<IArrayMap> mListeners;
    Object mLock;
    AutoPtr<IISessionManager> mService;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONMANAGER_H__
