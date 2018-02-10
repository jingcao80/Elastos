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

#ifndef __ELASTOS_DROID_SERVER_MEDIA_CSESSIONMANAGERIMPL_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CSESSIONMANAGERIMPL_H__

#include "_Elastos_Droid_Server_Media_CSessionManagerImpl.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/media/MediaSessionService.h"
#include "elastos/droid/server/media/CKeyEventWakeLockReceiver.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Media::IIRemoteVolumeController;
using Elastos::Droid::Media::Session::IISessionManager;
using Elastos::Droid::Media::Session::IISessionCallback;
using Elastos::Droid::Media::Session::IISession;
using Elastos::Droid::Media::Session::IIActiveSessionsListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

class MediaSessionRecord;

CarClass(CSessionManagerImpl)
    , public Object
    , public IISessionManager
    , public IBinder
{
public:
    class KeyEventDone : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CSessionManagerImpl::KeyEventDone")

        KeyEventDone(
            /* [in] */ MediaSessionService* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        MediaSessionService* mHost;
    };

public:
    CSessionManagerImpl();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE host);

    CARAPI CreateSession(
        /* [in] */ const String& packageName,
        /* [in] */ IISessionCallback* cb,
        /* [in] */ const String& tag,
        /* [in] */ Int32 userId,
        /* [out] */ IISession** result);

    CARAPI GetSessions(
        /* [in] */ IComponentName* compName,
        /* [in] */ Int32 userId,
        /* [out] */ IList** result);

    CARAPI AddSessionsListener(
        /* [in] */ IIActiveSessionsListener* listener,
        /* [in] */ IComponentName* compName,
        /* [in] */ Int32 userId);

    CARAPI RemoveSessionsListener(
        /* [in] */ IIActiveSessionsListener* listener);

    /**
     * Handles the dispatching of the media button events to one of the
     * registered listeners, or if there was none, broadcast an
     * ACTION_MEDIA_BUTTON intent to the rest of the system.
     *
     * @param keyEvent a non-null KeyEvent whose key code is one of the
     *            supported media buttons
     * @param needWakeLock true if a PARTIAL_WAKE_LOCK needs to be held
     *            while this key event is dispatched.
     */
    CARAPI DispatchMediaKeyEvent(
        /* [in] */ IKeyEvent* keyEvent,
        /* [in] */ Boolean needWakeLock);

    CARAPI DispatchAdjustVolume(
        /* [in] */ Int32 suggestedStream,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 flags);

    // This is for the system volume UI only
    CARAPI SetRemoteVolumeController(
        /* [in] */ IIRemoteVolumeController* rvc);

    // For PhoneWindowManager to precheck media keys
    CARAPI IsGlobalPriorityActive(
        /* [out] */ Boolean* result);

    // public void dump(FileDescriptor fd, final PrintWriter pw, String[] args);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI VerifySessionsRequest(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result);

    CARAPI DispatchAdjustVolumeLocked(
        /* [in] */ Int32 suggestedStream,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags,
        /* [in] */ MediaSessionRecord* session);

    CARAPI_(void) HandleVoiceKeyEventLocked(
        /* [in] */ IKeyEvent* keyEvent,
        /* [in] */ Boolean needWakeLock,
        /* [in] */ MediaSessionRecord* session);

    CARAPI_(void) DispatchMediaKeyEventLocked(
        /* [in] */ IKeyEvent* keyEvent,
        /* [in] */ Boolean needWakeLock,
        /* [in] */ MediaSessionRecord* session);

    CARAPI_(void) StartVoiceInput(
        /* [in] */ Boolean needWakeLock);

    CARAPI_(Boolean) IsVoiceKey(
        /* [in] */ Int32 keyCode);

private:
    static const String EXTRA_WAKELOCK_ACQUIRED;
    static const Int32 WAKELOCK_RELEASE_ON_FINISHED = 1980; // magic number

    Boolean mVoiceButtonDown;
    Boolean mVoiceButtonHandled;

    MediaSessionService* mHost;
    AutoPtr<CKeyEventWakeLockReceiver> mKeyEventReceiver;
    AutoPtr<KeyEventDone> mKeyEventDone;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CSESSIONMANAGERIMPL_H__
