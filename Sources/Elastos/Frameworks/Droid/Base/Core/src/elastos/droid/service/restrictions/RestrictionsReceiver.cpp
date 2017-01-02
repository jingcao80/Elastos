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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/service/restrictions/RestrictionsReceiver.h"

using Elastos::Droid::Content::IRestrictionsManager;
using Elastos::Droid::Os::IPersistableBundle;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Restrictions {

CAR_INTERFACE_IMPL(RestrictionsReceiver, BroadcastReceiver, IRestrictionsReceiver)

const String RestrictionsReceiver::TAG("RestrictionsReceiver");

ECode RestrictionsReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (IRestrictionsManager::ACTION_REQUEST_PERMISSION.Equals(action)) {
        String packageName;
        intent->GetStringExtra(IRestrictionsManager::EXTRA_PACKAGE_NAME, &packageName);
        String requestType;
        intent->GetStringExtra(IRestrictionsManager::EXTRA_REQUEST_TYPE, &requestType);
        String requestId;
        intent->GetStringExtra(IRestrictionsManager::EXTRA_REQUEST_ID, &requestId);
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IRestrictionsManager::EXTRA_REQUEST_BUNDLE, (IParcelable**)&parcelable);
        AutoPtr<IPersistableBundle> request = IPersistableBundle::Probe(parcelable.Get());
        OnRequestPermission(context, packageName, requestType, requestId, request);
    }
    return NOERROR;
}

} // namespace Restrictions
} // namespace Service
} // namepsace Droid
} // namespace Elastos
