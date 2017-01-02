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

#ifndef __ELASTOS_DROID_DEFAULTCONTAINERSERVICE_DEFAULTCONTAINERSERVICE_H__
#define __ELASTOS_DROID_DEFAULTCONTAINERSERVICE_DEFAULTCONTAINERSERVICE_H__

#include <elastos/droid/ext/frameworkext.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Internal.h>
#include <elastos/droid/app/IntentService.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IntentService;
using Elastos::Droid::Internal::App::IIMediaContainerService;
using Elastos::Droid::Internal::Content::INativeLibraryHelperHandle;
using Elastos::Droid::Internal::Os::IIParcelFileDescriptorFactory;
using Elastos::Droid::Content::Pm::IPackageInfoLite;
using Elastos::Droid::Content::Pm::IPackageLite;
using Elastos::Droid::Content::Res::IObbInfo;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace DefContainer {

class DefaultContainerService
    : public IntentService
{
public:
    class MediaContainerService
        : public Object
        , public IIMediaContainerService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MediaContainerService();

        CARAPI constructor(
            /* [in] */ IService* host);

        CARAPI CopyPackageToContainer(
            /* [in] */ const String& packagePath,
            /* [in] */ const String& containerId,
            /* [in] */ const String& key,
            /* [in] */ Boolean isExternal,
            /* [in] */ Boolean isForwardLocked,
            /* [in] */ const String& abiOverride,
            /* [out] */ String* result);

        CARAPI CopyPackage(
            /* [in] */ const String& packagePath,
            /* [in] */ IIParcelFileDescriptorFactory* target,
            /* [out] */ Int32* result);

        CARAPI GetMinimalPackageInfo(
            /* [in] */ const String& packagePath,
            /* [in] */ Int32 flags,
            /* [in] */ const String& abiOverride,
            /* [out] */ IPackageInfoLite** result);

        CARAPI GetObbInfo(
            /* [in] */ const String& filename,
            /* [out] */ IObbInfo** result);

        CARAPI CalculateDirectorySize(
            /* [in] */ const String& directory,
            /* [out] */ Int64* result);

        CARAPI GetFileSystemStats(
            /* [in] */ const String& path,
            /* [out, callee] */ ArrayOf<Int64>** result);

        CARAPI ClearDirectory(
            /* [in] */ const String& directory);

        CARAPI CalculateInstalledSize(
            /* [in] */ const String& packagePath,
            /* [in] */ Boolean isForwardLocked,
            /* [in] */ const String& abiOverride,
            /* [out] */ Int64* result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        DefaultContainerService* mHost;
    };

public:
    DefaultContainerService();

    CARAPI constructor();

    CARAPI EraseFiles(
        /* [in] */ ArrayOf<IFile*> * paths);

    CARAPI EraseFiles(
        /* [in] */ IFile* path);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

protected:
    CARAPI OnHandleIntent(
        /* [in] */ IIntent* intent);

private:
    CARAPI CopyPackageToContainerInner(
        /* [in] */ IPackageLite* pkg,
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ const String& newCid,
        /* [in] */ const String& key,
        /* [in] */ Boolean isExternal,
        /* [in] */ Boolean isForwardLocked,
        /* [in] */ const String& abiOverride,
        /* [out] */ String* result);

    CARAPI CopyPackageInner(
        /* [in] */ IPackageLite* pkg,
        /* [in] */ IIParcelFileDescriptorFactory* target,
        /* [out] */ Int32* result);

    CARAPI CopyFile(
        /* [in] */ const String& sourcePath,
        /* [in] */ IIParcelFileDescriptorFactory* target,
        /* [in] */ const String& targetName);

    CARAPI CopyFile(
        /* [in] */ const String& sourcePath,
        /* [in] */ IFile* targetDir,
        /* [in] */ const String& targetName,
        /* [in] */ Boolean isForwardLocked);

private:
    static const String TAG;
    AutoPtr<IIMediaContainerService> mBinder;
};

} //namespace DefContainer
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_DEFAULTCONTAINERSERVICE_DEFAULTCONTAINERSERVICE_H__