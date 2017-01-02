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

#ifndef __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLE_H__
#define __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "_Elastos_Droid_Internal_Content_CNativeLibraryHelperHandle.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::Pm::IPackageLite;
using Elastos::Core::Object;
using Elastos::Core::ICloseGuard;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CarClass(CNativeLibraryHelperHandle)
    , public Object
    , public INativeLibraryHelperHandle
    , public ICloseable
{
    friend class CNativeLibraryHelper;
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNativeLibraryHelperHandle();

    ~CNativeLibraryHelperHandle();

    CARAPI constructor(
        /* [in] */ ArrayOf<Int64>* apkHandles,
        /* [in] */ Boolean multiArch);

    CARAPI Close();

public:
    static CARAPI Create(
        /* [in] */ IFile* packageFile,
        /* [out] */ INativeLibraryHelperHandle** handle);

    static CARAPI CreatePackage(
        /* [in] */ Handle64 _pkg,
        /* [out] */ INativeLibraryHelperHandle** handle);

    static CARAPI Create(
        /* [in] */ IPackageLite* lite,
        /* [out] */ INativeLibraryHelperHandle** handle);

private:
    static CARAPI Create(
        /* [in] */ List<String>* codePaths,
        /* [in] */ Boolean multiArch,
        /* [out] */ INativeLibraryHelperHandle** handle);

private:
    AutoPtr<ICloseGuard> mGuard;
    volatile Boolean mClosed;

    AutoPtr<ArrayOf<Int64> > mApkHandles;
    Boolean mMultiArch;
};

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLE_H__
