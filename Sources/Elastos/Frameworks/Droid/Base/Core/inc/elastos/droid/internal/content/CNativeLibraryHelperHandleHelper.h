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

#ifndef __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLEHELPER_H__

#include "_Elastos_Droid_Internal_Content_CNativeLibraryHelperHandleHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Pm::IPackageLite;
using Elastos::Core::Singleton;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CarClass(CNativeLibraryHelperHandleHelper)
    , public Singleton
    , public INativeLibraryHelperHandleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [in] */ IFile* packageFile,
        /* [out] */ INativeLibraryHelperHandle** handle);

    CARAPI CreatePackage(
        /* [in] */ Handle64 pkg,
        /* [out] */ INativeLibraryHelperHandle** handle);

    CARAPI Create(
        /* [in] */ IPackageLite* lite,
        /* [out] */ INativeLibraryHelperHandle** handle);
};

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_CONTENT_CNATIVELIBRARYHELPERHANDLEHELPER_H__
