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

#include "elastos/droid/opengl/EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(EGLObjectHandle, Object, IEGLObjectHandle)

ECode EGLObjectHandle::GetHandle(
    /* [out] */ Int32* handle)
{
    VALIDATE_NOT_NULL(handle)

    *handle = mHandle;
    return NOERROR;
}

ECode EGLObjectHandle::GetNativeHandle(
    /* [out] */ Int64* handle)
{
    *handle = mHandle;
    return NOERROR;
}

ECode EGLObjectHandle::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    if ((mHandle & 0xffffffffL) != mHandle) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *hashCode = (Int32)mHandle;
    return NOERROR;
}

EGLObjectHandle::EGLObjectHandle(
    /* [in] */ Int32 handle)
{
    mHandle = handle;
}

EGLObjectHandle::EGLObjectHandle(
    /* [in] */ Int64 handle)
{
    mHandle = handle;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos