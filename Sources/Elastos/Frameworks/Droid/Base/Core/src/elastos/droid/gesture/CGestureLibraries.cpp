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

#include "elastos/droid/gesture/CGestureLibraries.h"
#include "elastos/droid/gesture/GestureLibraries.h"

using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CFile;

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_SINGLETON_IMPL(CGestureLibraries)

CAR_INTERFACE_IMPL(CGestureLibraries, Singleton, IGestureLibraries)

ECode CGestureLibraries::FromFile(
    /* [in] */ const String& path,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromFile(path);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

CARAPI CGestureLibraries::FromFile(
    /* [in] */ IFile* path,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromFile(path);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

CARAPI CGestureLibraries::FromPrivateFile(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& name,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromPrivateFile(ctx, name);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

CARAPI CGestureLibraries::FromRawResource(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 resourceId,
    /* [out] */ IGestureLibrary** gestureLib)
{
    VALIDATE_NOT_NULL(gestureLib);
    AutoPtr<IGestureLibrary> ret = GestureLibraries::FromRawResource(ctx, resourceId);
    *gestureLib = ret;
    REFCOUNT_ADD(*gestureLib);
    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
