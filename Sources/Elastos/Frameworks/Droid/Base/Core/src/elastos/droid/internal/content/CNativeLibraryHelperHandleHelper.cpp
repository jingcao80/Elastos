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

#include "elastos/droid/internal/content/CNativeLibraryHelperHandleHelper.h"
#include "elastos/droid/internal/content/CNativeLibraryHelperHandle.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CAR_INTERFACE_IMPL(CNativeLibraryHelperHandleHelper, Singleton, INativeLibraryHelperHandleHelper)
CAR_SINGLETON_IMPL(CNativeLibraryHelperHandleHelper)

ECode CNativeLibraryHelperHandleHelper::Create(
    /* [in] */ IFile* packageFile,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::Create(packageFile, handle);
}

ECode CNativeLibraryHelperHandleHelper::CreatePackage(
    /* [in] */ Handle64 pkg,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::CreatePackage(pkg, handle);
}

ECode CNativeLibraryHelperHandleHelper::Create(
    /* [in] */ IPackageLite* lite,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    return CNativeLibraryHelperHandle::Create(lite, handle);
}

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos
