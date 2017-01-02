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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_PHONESTATEBROADCASTER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_PHONESTATEBROADCASTER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Internal::Telephony::IITelephonyRegistry;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Send a {@link TelephonyManager#ACTION_PHONE_STATE_CHANGED} broadcast when the call state
 * changes.
 */
class PhoneStateBroadcaster
    : public CallsManagerListenerBase
{
public:
    PhoneStateBroadcaster();

    CARAPI constructor();

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    // @Override
    CARAPI OnCallAdded(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    CARAPI GetCallState(
        /* [out] */ Int32* result);

private:
    CARAPI SendPhoneStateChangedBroadcast(
        /* [in] */ ICall* call,
        /* [in] */ Int32 phoneState);

    AutoPtr<IITelephonyRegistry> mRegistry;

    Int32 mCurrentState;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_PHONESTATEBROADCASTER_H__
