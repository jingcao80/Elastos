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

#ifndef __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPER_H__
#define __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPER_H__

#include "_Elastos_Droid_Internal_Content_CNativeLibraryHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CarClass(CNativeLibraryHelper)
    , public Singleton
    , public INativeLibraryHelper
{
    friend class CNativeLibraryHelperHandle;
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Copies native binaries to a shared library directory.
     *
     * @param handle APK file to scan for native libraries
     * @param sharedLibraryDir directory for libraries to be copied to
     * @return {@link PackageManager#INSTALL_SUCCEEDED} if successful or another
     *         error code from that class if not
     */
    CARAPI CopyNativeBinaries(
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ IFile* sharedLibraryDir,
        /* [in] */ const String& abi,
        /* [out] */ Int32* result);

    /**
     * Checks if a given APK contains native code for any of the provided
     * {@code supportedAbis}. Returns an index into {@code supportedAbis} if a matching
     * ABI is found, {@link PackageManager#NO_NATIVE_LIBRARIES} if the
     * APK doesn't contain any native code, and
     * {@link PackageManager#INSTALL_FAILED_NO_MATCHING_ABIS} if none of the ABIs match.
     */
    CARAPI FindSupportedAbi(
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ ArrayOf<String>* supportedAbis,
        /* [out] */ Int32* result);

    // Convenience method to call removeNativeBinariesFromDirLI(File)
    CARAPI RemoveNativeBinariesLI(
        /* [in] */ const String& nativeLibraryPath);

    /**
     * Remove the native binaries of a given package. This deletes the files
     */
    CARAPI RemoveNativeBinariesFromDirLI(
        /* [in] */ IFile* nativeLibraryRoot,
        /* [in] */ Boolean deleteRootDir);

    CARAPI CopyNativeBinariesForSupportedAbi(
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ IFile* libraryRoot,
        /* [in] */ ArrayOf<String>* abiList,
        /* [in] */ Boolean useIsaSubdir,
        /* [out] */ Int32* result);

    CARAPI CopyNativeBinariesWithOverride(
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ IFile* libraryRoot,
        /* [in] */ const String& abiOverride,
        /* [out] */ Int32* result);

    CARAPI SumNativeBinariesWithOverride(
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ const String& abiOverride,
        /* [out] */ Int64* sum);

    CARAPI HasRenderscriptBitcode(
        /* [in] */ INativeLibraryHelperHandle* _handle,
        /* [out] */ Boolean* result);

    // for epk
    // copy 'ecoName'.eco in epkFile to ecoDir
    CARAPI CopyEcoLI(
        /* [in] */ const String& epkPath,
        /* [in] */ const String& ecoName,
        /* [in] */ const String& ecoDir,
        /* [out] */ Int32* result);

private:
    CARAPI_(Int64) SumNativeBinaries(
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ const String& abi);

    CARAPI CreateNativeLibrarySubdir(
        /* [in] */ IFile* path);

    CARAPI_(Int64) SumNativeBinariesForSupportedAbi(
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ ArrayOf<String>* abiList);

    static CARAPI_(Int64) NativeOpenApk(
        /* [in] */ const String& path);

    static CARAPI_(void) NativeClose(
        /* [in] */ Int64 handle);

    static CARAPI_(Int64) NativeSumNativeBinaries(
        /* [in] */ Int64 handle,
        /* [in] */ const String& cpuAbi);

    static CARAPI_(Int32) NativeCopyNativeBinaries(
        /* [in] */ Int64 handle,
        /* [in] */ const String& sharedLibraryPath,
        /* [in] */ const String& abiToCopy);

    static CARAPI_(Int32) NativeFindSupportedAbi(
        /* [in] */ Int64 handle,
        /* [in] */ ArrayOf<String>* supportedAbis);

    static CARAPI_(Int32) HasRenderscriptBitcode(
        /* [in] */ Int64 apkHandle);

private:
    static const String TAG;
    static const Boolean DEBUG_NATIVE;
    // We don't care about the other return values for now.
    static const Int32 BITCODE_PRESENT;
};

}
}
}
}

#endif // __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPER_H__
