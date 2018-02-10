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

#ifndef __ELASTOS_DROID_JAVAPROXY_CMEDIACONTAINERSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CMEDIACONTAINERSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CMediaContainerServiceNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::Pm::IPackageInfoLite;
using Elastos::Droid::Content::Res::IObbInfo;
using Elastos::Droid::Internal::App::IIMediaContainerService;
using Elastos::Droid::Internal::Os::IIParcelFileDescriptorFactory;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CMediaContainerServiceNative)
    , public Object
    , public IIMediaContainerService
    , public IBinder
{
public:
    ~CMediaContainerServiceNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI CopyPackageToContainer(
        /* [in] */ const String& packagePath,
        /* [in] */ const String& containerId,
        /* [in] */ const String& key,
        /* [in] */ Boolean isExternal,
        /* [in] */ Boolean isForwardLocked,
        /* [in] */ const String& abiOverride,
        /* [out] */ String* path);

    CARAPI CopyPackage(
        /* [in] */ const String& packagePath,
        /* [in] */ IIParcelFileDescriptorFactory* target,
        /* [out] */ Int32* result);

    CARAPI GetMinimalPackageInfo(
        /* [in] */ const String& packagePath,
        /* [in] */ Int32 flags,
        /* [in] */ const String& abiOverride,
        /* [out] */ IPackageInfoLite** pkgLite);

    CARAPI GetObbInfo(
        /* [in] */ const String& filename,
        /* [out] */ IObbInfo** obbInfo);

    CARAPI CalculateDirectorySize(
        /* [in] */ const String& directory,
        /* [out] */ Int64* size);

    CARAPI GetFileSystemStats(
        /* [in] */ const String& path,
        /* [out, callee] */ ArrayOf<Int64>** stats);

    CARAPI ClearDirectory(
        /* [in] */ const String& directory);

    CARAPI CalculateInstalledSize(
        /* [in] */ const String& packagePath,
        /* [in] */ Boolean isForwardLocked,
        /* [in] */ const String& abiOverride,
        /* [out] */ Int64* size);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CMEDIACONTAINERSERVICENATIVE_H__
