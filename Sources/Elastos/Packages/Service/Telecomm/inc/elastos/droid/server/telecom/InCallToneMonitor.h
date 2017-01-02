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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEMONITOR_H__
#define __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEMONITOR_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Monitors events from CallsManager and plays in-call tones for events which require them, such as
 * different type of call disconnections (busy tone, congestion tone, etc).
 */
class InCallToneMonitor
    : public CallsManagerListenerBase
{
public:
    CARAPI constructor(
        /* [in] */ InCallTonePlayer::Factory* playerFactory,
        /* [in] */ CallsManager* callsManager);

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

private:
    AutoPtr<InCallTonePlayer::Factory> mPlayerFactory;

    AutoPtr<CallsManager> mCallsManager;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEMONITOR_H__
