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

#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_REALSYSTEMFACADE_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_REALSYSTEMFACADE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Providers.Downloads.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::INetworkInfo;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

class RealSystemFacade
    : public Object
    , public ISystemFacade
{
public:
    CAR_INTERFACE_DECL()

    RealSystemFacade(
        /* [in] */ IContext* context);

    CARAPI GetCurrentTimeMillis(
        /* [out] */ Int64* result);

    CARAPI GetActiveNetworkInfo(
        /* [in] */ Int32 uid,
        /* [out] */ INetworkInfo** result);

    CARAPI IsActiveNetworkMetered(
        /* [out] */ Boolean* result);

    CARAPI IsNetworkRoaming(
        /* [out] */ Boolean* result);

    CARAPI GetMaxBytesOverMobile(
        /* [out] */ Int64* result);

    CARAPI GetRecommendedMaxBytesOverMobile(
        /* [out] */ Int64* result);

    CARAPI SendBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI UserOwnsPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

private:
    AutoPtr<IContext> mContext;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_REALSYSTEMFACADE_H__
