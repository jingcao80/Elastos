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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGESTATS_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGESTATS_H__

#include "_Elastos_Droid_Content_Pm_CPackageStats.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * implementation of PackageStats associated with a
 * application package.
 */
CarClass(CPackageStats)
    , public Object
    , public IPackageStats
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageStats();

    ~CPackageStats();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& pkgName);

    CARAPI constructor(
        /* [in] */ const String& pkgName,
        /* [in] */ Int32 userHandle);

    CARAPI constructor(
        /* [in] */ IPackageStats* orig);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetUserHandle(
        /* [out] */ Int32* userHandle);

    CARAPI SetUserHandle(
        /* [in] */ Int32 userHandle);

    CARAPI GetCodeSize(
        /* [out] */ Int64* codeSize);

    CARAPI SetCodeSize(
        /* [in] */ Int64 codeSize);

    CARAPI GetDataSize(
        /* [out] */ Int64* dataSize);

    CARAPI SetDataSize(
        /* [in] */ Int64 dataSize);

    CARAPI GetCacheSize(
        /* [out] */ Int64* cacheSize);

    CARAPI SetCacheSize(
        /* [in] */ Int64 cacheSize);

    CARAPI GetExternalCodeSize(
        /* [out] */ Int64* externalCodeSize);

    CARAPI SetExternalCodeSize(
        /* [in] */ Int64 externalCodeSize);

    CARAPI GetExternalDataSize(
        /* [out] */ Int64* externalDataSize);

    CARAPI SetExternalDataSize(
        /* [in] */ Int64 externalDataSize);

    CARAPI GetExternalCacheSize(
        /* [out] */ Int64* externalCacheSize);

    CARAPI SetExternalCacheSize(
        /* [in] */ Int64 externalCacheSize);

    CARAPI GetExternalMediaSize(
        /* [out] */ Int64* externalMediaSize);

    CARAPI SetExternalMediaSize(
        /* [in] */ Int64 externalMediaSize);

    CARAPI GetExternalObbSize(
        /* [out] */ Int64* externalObbSize);

    CARAPI SetExternalObbSize(
        /* [in] */ Int64 externalObbSize);

public:
    /** Name of the package to which this stats applies. */
    String mPackageName;

    /** @hide */
    Int32 mUserHandle;

    /** Size of the code (e.g., APK) */
    Int64 mCodeSize;

    /**
     * Size of the internal data size for the application. (e.g.,
     * /data/data/<app>)
     */
    Int64 mDataSize;

    /** Size of cache used by the application. (e.g., /data/data/<app>/cache) */
    Int64 mCacheSize;

    /**
     * Size of the secure container on external storage holding the
     * application's code.
     */
    Int64 mExternalCodeSize;

    /**
     * Size of the external data used by the application (e.g.,
     * <sdcard>/Android/data/<app>)
     */
    Int64 mExternalDataSize;

    /**
     * Size of the external cache used by the application (i.e., on the SD
     * card). If this is a subdirectory of the data directory, this size will be
     * subtracted out of the external data size.
     */
    Int64 mExternalCacheSize;

    /** Size of the external media size used by the application. */
    Int64 mExternalMediaSize;

    /** Size of the package's OBBs placed on external media. */
    Int64 mExternalObbSize;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGESTATS_H__
