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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_CPHONEACCOUNTBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CPHONEACCOUNTBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Server_Telecom_CPhoneAccountBroadcastReceiver.h"
#include "elastos/droid/server/telecom/PhoneAccountBroadcastReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Captures {@code android.intent.action.ACTION_PACKAGE_FULLY_REMOVED} intents and triggers the
 * removal of associated {@link android.telecom.PhoneAccount}s via the
 * {@link com.android.telecom.PhoneAccountRegistrar}.
 * Note: This class listens for the {@code PACKAGE_FULLY_REMOVED} intent rather than
 * {@code PACKAGE_REMOVED} as {@code PACKAGE_REMOVED} is triggered on re-installation of the same
 * package, where {@code PACKAGE_FULLY_REMOVED} is triggered only when an application is completely
 * uninstalled.  This is desirable as we do not wish to un-register all
 * {@link android.telecom.PhoneAccount}s associated with a package being re-installed to ensure
 * the enabled state of the accounts is retained.
 */
CarClass(CPhoneAccountBroadcastReceiver)
    , public PhoneAccountBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CPHONEACCOUNTBROADCASTRECEIVER_H__
