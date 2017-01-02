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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_STATUSBARNOTIFIER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_STATUSBARNOTIFIER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallsManager;
/**
 * Manages the special status bar notifications used by the phone app.
 */
class StatusBarNotifier
    : public CallsManagerListenerBase
{
public:
    StatusBarNotifier();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ CallsManager* callsManager);

    /** ${inheritDoc} */
    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    CARAPI NotifyMute(
        /* [in] */ Boolean isMuted);

    CARAPI NotifySpeakerphone(
        /* [in] */ Boolean isSpeakerphone);

private:
    static const String SLOT_MUTE;

    static const String SLOT_SPEAKERPHONE;

    AutoPtr<IContext> mContext;

    AutoPtr<CallsManager> mCallsManager;

    AutoPtr<IStatusBarManager> mStatusBarManager;

    Boolean mIsShowingMute;

    Boolean mIsShowingSpeakerphone;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_STATUSBARNOTIFIER_H__
