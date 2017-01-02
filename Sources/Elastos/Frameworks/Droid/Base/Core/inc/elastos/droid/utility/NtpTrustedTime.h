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

#ifndef __ELASTOS_DROID_UTILITY_NTPTRUSTEDTIME_H__
#define __ELASTOS_DROID_UTILITY_NTPTRUSTEDTIME_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::INtpTrustedTime;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * {@link TrustedTime} that connects with a remote NTP server as its trusted
 * time source.
 *
 * @hide
 */
class NtpTrustedTime
    : public Object
    , public INtpTrustedTime
    , public ITrustedTime
{
private:
    NtpTrustedTime(
        /* [in] */ const String& server,
        /* [in] */ Int64 timeout);

public:
    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ INtpTrustedTime** instance);

    CARAPI ForceRefresh(
        /* [out] */  Boolean* isRefreshed);

    CARAPI HasCache(
        /* [out] */  Boolean* hasCache);

    CARAPI GetCacheAge(
        /* [out] */  Int64* cacheAge);

    CARAPI GetCacheCertainty(
        /* [out] */  Int64* cacheCertainty);

    CARAPI GetCurrentTimeMillis(
        /* [out] */  Int64* currentTimeMillis);

    virtual CARAPI GetCachedNtpTime(
        /* [out] */ Int64* cacheNtpTime);

    virtual CARAPI GetCachedNtpTimeReference(
        /* [out] */ Int64* cachedNtpTimeReference);

private:
    static const String TAG;
    static const Boolean LOGD;

    static AutoPtr<NtpTrustedTime> sSingleton;

    const String mServer;
    const Int64 mTimeout;

    Boolean mHasCache;
    Int64 mCachedNtpTime;
    Int64 mCachedNtpElapsedRealtime;
    Int64 mCachedNtpCertainty;
    static Object mLock;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_NTPTRUSTEDTIME_H__
