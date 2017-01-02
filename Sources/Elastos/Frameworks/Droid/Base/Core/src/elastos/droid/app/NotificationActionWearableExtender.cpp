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

#include "elastos/droid/app/NotificationActionWearableExtender.h"
#include "elastos/droid/app/CNotificationActionWearableExtender.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(NotificationActionWearableExtender, Object, INotificationActionExtender, INotificationActionWearableExtender)

const String NotificationActionWearableExtender::EXTRA_WEARABLE_EXTENSIONS("android.wearable.EXTENSIONS");
const String NotificationActionWearableExtender::KEY_FLAGS("flags");
const Int32 NotificationActionWearableExtender::FLAG_AVAILABLE_OFFLINE = 0x1;
const Int32 NotificationActionWearableExtender::DEFAULT_FLAGS = FLAG_AVAILABLE_OFFLINE;

NotificationActionWearableExtender::NotificationActionWearableExtender()
    : mFlags(DEFAULT_FLAGS)
{}

ECode NotificationActionWearableExtender::constructor()
{
    return NOERROR;
}

ECode NotificationActionWearableExtender::constructor(
    /* [in] */ INotificationAction* action)
{
    AutoPtr<IBundle> bundle;
    action->GetExtras((IBundle**)&bundle);
    AutoPtr<IBundle> wearableBundle;
    bundle->GetBundle(EXTRA_WEARABLE_EXTENSIONS, (IBundle**)&wearableBundle);
    if (wearableBundle != NULL) {
        wearableBundle->GetInt32(KEY_FLAGS, DEFAULT_FLAGS, &mFlags);
    }
    return NOERROR;
}

ECode NotificationActionWearableExtender::Extend(
    /* [in] */ INotificationActionBuilder* builder)
{
    AutoPtr<IBundle> wearableBundle;
    CBundle::New((IBundle**)&wearableBundle);
    if (mFlags != DEFAULT_FLAGS) {
        wearableBundle->PutInt32(KEY_FLAGS, mFlags);
    }
    AutoPtr<IBundle> bindle;
    builder->GetExtras((IBundle**)&bindle);
    bindle->PutBundle(EXTRA_WEARABLE_EXTENSIONS, wearableBundle);
    return NOERROR;
}

ECode NotificationActionWearableExtender::Clone(
    /* [out] */ INotificationActionWearableExtender** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<INotificationActionWearableExtender> that;
    CNotificationActionWearableExtender::New((INotificationActionWearableExtender**)&that);
    ((NotificationActionWearableExtender*)that.Get())->mFlags = mFlags;
    *result = that;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NotificationActionWearableExtender::SetAvailableOffline(
    /* [in] */ Boolean availableOffline)
{
    SetFlag(FLAG_AVAILABLE_OFFLINE, availableOffline);
    return NOERROR;
}

ECode NotificationActionWearableExtender::IsAvailableOffline(
    /* [out] */ Boolean* result)
{
    *result = (mFlags & FLAG_AVAILABLE_OFFLINE) != 0;
    return NOERROR;
}

void NotificationActionWearableExtender::SetFlag(
    /* [in] */ Int32 mask,
    /* [in] */ Boolean value)
{
    if (value) {
        mFlags |= mask;
    } else {
        mFlags &= ~mask;
    }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
