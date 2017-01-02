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

#ifndef  __ELASTOS_DROID_PHONE_CALLSTATEMONITOR_H__
#define  __ELASTOS_DROID_PHONE_CALLSTATEMONITOR_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Os::Handler;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Dedicated Call state monitoring class.  This class communicates directly with
 * the call manager to listen for call state events and notifies registered
 * handlers.
 * It works as an inverse multiplexor for all classes wanted Call State updates
 * so that there exists only one channel to the telephony layer.
 *
 * TODO: Add manual phone state checks (getState(), etc.).
 */
class CallStateMonitor
    : public Handler
{
public:
    TO_STRING_IMPL("CallLogger")

    // Events generated internally:
    CallStateMonitor(
        /* [in] */ ICallManager* callManager);

    CARAPI AddListener(
        /* [in] */ IHandler* handler);

    //@Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
     * When radio technology changes, we need to to reregister for all the events which are
     * all tied to the old radio.
     */
    CARAPI UpdateAfterRadioTechnologyChange();

private:
    /**
     * Register for call state notifications with the CallManager.
     */
    CARAPI_(void) RegisterForNotifications();

public:
    // Events from the Phone object:
    static const Int32 PHONE_STATE_CHANGED = 1;
    static const Int32 PHONE_NEW_RINGING_CONNECTION = 2;
    static const Int32 PHONE_DISCONNECT = 3;
    static const Int32 PHONE_UNKNOWN_CONNECTION_APPEARED = 4;
    static const Int32 PHONE_STATE_DISPLAYINFO = 6;
    static const Int32 PHONE_STATE_SIGNALINFO = 7;
    static const Int32 PHONE_CDMA_CALL_WAITING = 8;
    static const Int32 PHONE_ENHANCED_VP_ON = 9;
    static const Int32 PHONE_ENHANCED_VP_OFF = 10;
    static const Int32 PHONE_RINGBACK_TONE = 11;
    static const Int32 PHONE_RESEND_MUTE = 12;
    static const Int32 PHONE_ON_DIAL_CHARS = 13;

    // Other events from call manager
    static const Int32 EVENT_OTA_PROVISION_CHANGE = 20;

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<ICallManager> mCallManager;
    AutoPtr<IArrayList> mRegisteredHandlers;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CALLSTATEMONITOR_H__