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

#ifndef __ELASTOS_DROID_APP_CCONTENTPROVIDERHOLDER_H__
#define __ELASTOS_DROID_APP_CCONTENTPROVIDERHOLDER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CContentProviderHolder.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::Pm::IProviderInfo;


CarClass(CContentProviderHolder)
    , public Object
    , public IContentProviderHolder
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderHolder();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IProviderInfo* info);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetProviderInfo(
        /* [out] */ IProviderInfo** info);

    CARAPI SetProviderInfo(
        /* [in] */ IProviderInfo* info);

    CARAPI GetContentProvider(
        /* [out] */ IIContentProvider** provider);

    CARAPI SetContentProvider(
        /* [in] */ IIContentProvider* provider);

    CARAPI GetConnection(
        /* [out] */ IBinder** connection);

    CARAPI SetConnection(
        /* [in] */ IBinder* connection);

    CARAPI GetNoReleaseNeeded(
        /* [out] */ Boolean* noReleaseNeeded);

    CARAPI SetNoReleaseNeeded(
        /* [in] */ Boolean noReleaseNeeded);

private:
    AutoPtr<IProviderInfo> mInfo;
    AutoPtr<IIContentProvider> mProvider;
    AutoPtr<IBinder> mConnection;
    Boolean mNoReleaseNeeded;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CCONTENTPROVIDERHOLDER_H__
