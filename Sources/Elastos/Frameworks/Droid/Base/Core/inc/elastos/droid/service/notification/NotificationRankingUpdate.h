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

#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_NOTIFICATIONRANKINGUPDATE_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_NOTIFICATIONRANKINGUPDATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

class NotificationRankingUpdate
    : public Object
    , public INotificationRankingUpdate
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    NotificationRankingUpdate();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<String>* interceptedKeys,
        /* [in] */ IBundle* visibilityOverrides,
        /* [in] */ Int32 firstAmbientIndex);

    CARAPI constructor(
        /* [in] */ IParcelable* in);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);


    CARAPI GetOrderedKeys(
        /* [out, callee] */ ArrayOf<String>** orderedKeys);

    CARAPI GetFirstAmbientIndex(
        /* [out] */ Int32* index);

    CARAPI GetInterceptedKeys(
         /* [out, callee] */ ArrayOf<String>** interceptedKeys);

    CARAPI GetVisibilityOverrides(
        /* [out] */ IBundle** visibilityOverrides);

private:
    AutoPtr<ArrayOf<String> > mKeys;
    AutoPtr<ArrayOf<String> > mInterceptedKeys;
    Int32 mFirstAmbientIndex;
    AutoPtr<IBundle> mVisibilityOverrides;

};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_NOTIFICATIONRANKINGUPDATE_H__
