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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_TELECOMAPP_H__
#define __ELASTOS_DROID_SERVER_TELECOM_TELECOMAPP_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/MissedCallNotifier.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include "elastos/droid/server/telecom/TelecomServiceImpl.h"
#include <elastos/droid/app/Application.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::Application;
using Elastos::Droid::Internal::Telecom::IITelecomService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Top-level Application class for Telecom.
 */
class TelecomApp
    : public Application
{
public:
    /** {@inheritDoc} */
    // @Override
    CARAPI OnCreate();

    CARAPI GetMissedCallNotifier(
        /* [out] */ MissedCallNotifier** result);

    CARAPI GetPhoneAccountRegistrar(
        /* [out] */ PhoneAccountRegistrar** result);

private:
    /**
     * The Telecom service implementation.
     */
    AutoPtr<IITelecomService> mTelecomService;

    /**
     * Missed call notifier. Exists here so that the instance can be shared with
     * {@link TelecomBroadcastReceiver}.
     */
    AutoPtr<MissedCallNotifier> mMissedCallNotifier;

    /**
     * Blacklist call notifier. Exists here so that the instance can be shared with
     * {@link TelecomBroadcastReceiver}.
     */
    AutoPtr<BlacklistCallNotifier> mBlacklistCallNotifier;

    /**
     * Maintains the list of registered {@link android.telecom.PhoneAccountHandle}s.
     */
    AutoPtr<PhoneAccountRegistrar> mPhoneAccountRegistrar;

    /**
     * The calls manager for the Telecom service.
     */
    AutoPtr<CallsManager> mCallsManager;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_TELECOMAPP_H__
