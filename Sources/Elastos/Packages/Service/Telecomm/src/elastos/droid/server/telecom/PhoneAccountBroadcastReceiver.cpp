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

#include "elastos/droid/server/telecom/PhoneAccountBroadcastReceiver.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include <Elastos.Droid.Net.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(PhoneAccountBroadcastReceiver, BroadcastReceiver, IPhoneAccountBroadcastReceiver)

ECode PhoneAccountBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_PACKAGE_FULLY_REMOVED.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri == NULL) {
            return NOERROR;
        }
        String packageName;
        uri->GetSchemeSpecificPart(&packageName);
        HandlePackageRemoved(context, packageName);
    }
    return NOERROR;
}

ECode PhoneAccountBroadcastReceiver::HandlePackageRemoved(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName)
{
    AutoPtr <CallsManager> callsManager = CallsManager::GetInstance();
    if (callsManager != NULL) {
        AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
        callsManager->GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
        phoneAccountRegistrar->ClearAccounts(packageName);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
