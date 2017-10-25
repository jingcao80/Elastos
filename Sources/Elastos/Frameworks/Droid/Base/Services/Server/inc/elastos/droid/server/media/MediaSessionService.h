//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONSERVICE_H__
#define __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/media/CSessionManagerImpl.h"
#include "elastos/droid/server/media/MediaSessionStack.h"
#include "elastos/droid/server/media/MediaSessionRecord.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/SystemService.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Media::IIRemoteVolumeController;
using Elastos::Droid::Media::Session::IIActiveSessionsListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Server::IWatchdogMonitor;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

class CKeyEventWakeLockReceiver;

/**
 * System implementation of MediaSessionManager
 */
class MediaSessionService
    : public SystemService
    , public IWatchdogMonitor
{
public:
    /**
     * Information about a particular user. The contents of this object is
     * guarded by mLock.
     */
    class UserRecord : public Object
    {
    public:
        UserRecord(
            /* [in] */ IContext* context,
            /* [in] */ Int32 userId,
            /* [in] */ MediaSessionService* host)
            : mUserId(userId)
            , mHost(host)
        {}

        CARAPI_(void) StartLocked();

        CARAPI_(void) StopLocked();

        CARAPI_(void) DestroyLocked();

        CARAPI_(List<AutoPtr<MediaSessionRecord> >&) GetSessionsLocked();

        CARAPI_(void) AddSessionLocked(
            /* [in] */ MediaSessionRecord* session);

        CARAPI_(void) RemoveSessionLocked(
            /* [in] */ MediaSessionRecord* session);

        // CARAPI_(void) dumpLocked(PrintWriter pw, String prefix);

    private:
        Int32 mUserId;
        List<AutoPtr<MediaSessionRecord> > mSessions;
        AutoPtr<IPendingIntent> mLastMediaButtonReceiver;
        MediaSessionService* mHost;

        friend class MediaSessionService;
        friend class CSessionManagerImpl;
    };

    class SessionsListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        SessionsListenerRecord(
            /* [in] */ IIActiveSessionsListener* listener,
            /* [in] */ IComponentName* componentName,
            /* [in] */ Int32 userId,
            /* [in] */ Int32 pid,
            /* [in] */ Int32 uid,
            /* [in] */ MediaSessionService* host)
            : mListener(listener)
            , mComponentName(componentName)
            , mUserId(userId)
            , mPid(pid)
            , mUid(uid)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI ProxyDied();

    private:
        AutoPtr<IIActiveSessionsListener> mListener;
        AutoPtr<IComponentName> mComponentName;
        Int32 mUserId;
        Int32 mPid;
        Int32 mUid;
        MediaSessionService* mHost;

        friend class MediaSessionService;
        friend class CSessionManagerImpl;
    };

    class SettingsObserver : public ContentObserver
    {
    public:
        TO_STRING_IMPL("MediaSessionService::SettingsObserver")

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI constructor(
            /* [in] */ MediaSessionService* host);

        CARAPI_(void) Observe();

    private:
        AutoPtr<IUri> mSecureSettingsUri;
        MediaSessionService* mHost;

        friend class MediaSessionService;
    };

    class MessageHandler : public Handler
    {
    public:
        TO_STRING_IMPL("MediaSessionService::MessageHandler")

        MessageHandler(
            /* [in] */ MediaSessionService* host)
            : mHost(host)
        {
            Handler::constructor();
        }

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        using Handler::Post;

        CARAPI_(void) Post(
            /* [in] */ Int32 what,
            /* [in] */ Int32 arg1,
            /* [in] */ Int32 arg2);

    private:
        static const Int32 MSG_SESSIONS_CHANGED = 1;
        MediaSessionService* mHost;

        friend class MediaSessionService;
    };

public:
    MediaSessionService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnStart();

    CARAPI_(void) UpdateSession(
        /* [in] */ MediaSessionRecord* record);

    CARAPI_(void) OnSessionPlaystateChange(
        /* [in] */ MediaSessionRecord* record,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    CARAPI_(void) OnSessionPlaybackTypeChanged(
        /* [in] */ MediaSessionRecord* record);

    // @Override
    CARAPI OnStartUser(
        /* [in] */ Int32 userHandle);

    // @Override
    CARAPI OnSwitchUser(
        /* [in] */ Int32 userId);

    // @Override
    CARAPI OnStopUser(
        /* [in] */ Int32 userHandle);

    // @Override
    CARAPI Monitor();

    CARAPI_(void) SessionDied(
        /* [in] */ MediaSessionRecord* session);

    CARAPI_(void) DestroySession(
        /* [in] */ MediaSessionRecord* session);

    CARAPI EnforcePhoneStatePermission(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid);

private:
    CARAPI_(AutoPtr<IIAudioService>) GetAudioService();

    CARAPI_(void) UpdateUser();

    CARAPI_(void) UpdateActiveSessionListeners();

    /**
     * Stop the user and unbind from everything.
     *
     * @param user The user to dispose of
     */
    CARAPI_(void) DestroyUserLocked(
        /* [in] */ UserRecord* user);

    /*
     * When a session is removed several things need to happen.
     * 1. We need to remove it from the relevant user.
     * 2. We need to remove it from the priority stack.
     * 3. We need to remove it from all sessions.
     * 4. If this is the system priority session we need to clear it.
     * 5. We need to unlink to death from the cb binder
     * 6. We need to tell the session to do any final cleanup (onDestroy)
     */
    CARAPI_(void) DestroySessionLocked(
        /* [in] */ MediaSessionRecord* session);

    CARAPI EnforcePackageName(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid);

    /**
     * Checks a caller's authorization to register an IRemoteControlDisplay.
     * Authorization is granted if one of the following is true:
     * <ul>
     * <li>the caller has android.Manifest.permission.MEDIA_CONTENT_CONTROL
     * permission</li>
     * <li>the caller's listener is one of the enabled notification listeners
     * for the caller's user</li>
     * </ul>
     */
    CARAPI EnforceMediaPermissions(
        /* [in] */ IComponentName* compName,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 resolvedUserId);

    CARAPI EnforceStatusBarPermission(
        /* [in] */ const String& action,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid);

    /**
     * This checks if the component is an enabled notification listener for the
     * specified user. Enabled components may only operate on behalf of the user
     * they're running as.
     *
     * @param compName The component that is enabled.
     * @param userId The user id of the caller.
     * @param forUserId The user id they're making the request on behalf of.
     * @return True if the component is enabled, false otherwise
     */
    CARAPI_(Boolean) IsEnabledNotificationListener(
        /* [in] */ IComponentName* compName,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 forUserId);

    CARAPI CreateSessionInternal(
        /* [in] */ Int32 callerPid,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 userId,
        /* [in] */ const String& callerPackageName,
        /* [in] */ IISessionCallback* cb,
        /* [in] */ const String& tag,
        /* [out] */ MediaSessionRecord** record);

    /*
     * When a session is created the following things need to happen.
     * 1. Its callback binder needs a link to death
     * 2. It needs to be added to all sessions.
     * 3. It needs to be added to the priority stack.
     * 4. It needs to be added to the relevant user record.
     */
    CARAPI CreateSessionLocked(
        /* [in] */ Int32 callerPid,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 userId,
        /* [in] */ const String& callerPackageName,
        /* [in] */ IISessionCallback* cb,
        /* [in] */ const String& tag,
        /* [out] */ MediaSessionRecord** record);

    CARAPI_(AutoPtr<UserRecord>) GetOrCreateUser(
        /* [in] */ Int32 userId);

    CARAPI_(List<AutoPtr<SessionsListenerRecord> >::Iterator) FindIndexOfSessionsListenerLocked(
        /* [in] */ IIActiveSessionsListener* listener);

    CARAPI_(Boolean) IsSessionDiscoverable(
        /* [in] */ MediaSessionRecord* record);

    CARAPI_(void) PushSessionsChanged(
        /* [in] */ Int32 userId);

    CARAPI_(void) PushRemoteVolumeUpdateLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) RememberMediaButtonReceiverLocked(
        /* [in] */ MediaSessionRecord* record);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 WAKELOCK_TIMEOUT = 5000;

    AutoPtr<CSessionManagerImpl> mSessionManagerImpl;
    AutoPtr<MediaSessionStack> mPriorityStack;

    List<AutoPtr<MediaSessionRecord> > mAllSessions;
    HashMap<Int32, AutoPtr<UserRecord> > mUserRecords;
    List<AutoPtr<SessionsListenerRecord> > mSessionsListeners;
    Object mLock;
    AutoPtr<MessageHandler> mHandler;
    AutoPtr<IPowerManagerWakeLock> mMediaEventWakeLock;

    AutoPtr<IKeyguardManager> mKeyguardManager;
    AutoPtr<IIAudioService> mAudioService;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<SettingsObserver> mSettingsObserver;

    Int32 mCurrentUserId;

    // Used to notify system UI when remote volume was changed. TODO find a
    // better way to handle this.
    AutoPtr<IIRemoteVolumeController> mRvc;

    friend class CSessionManagerImpl;
    friend class UserRecord;
    friend class SessionsListenerRecord;
    friend class SettingsObserver;
    friend class HandleMessage;
    friend class CKeyEventWakeLockReceiver;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONSERVICE_H__
