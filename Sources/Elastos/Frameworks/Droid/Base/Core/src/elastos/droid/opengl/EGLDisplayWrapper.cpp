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

#include "elastos/droid/opengl/EGLDisplayWrapper.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(EGLDisplayWrapper, EGLObjectHandle, IEGLDisplay)

ECode EGLDisplayWrapper::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if(o == NULL || o->Probe(EIID_IEGLDisplay) == NULL) {
        *result = FALSE;
    } else if (Probe(EIID_IInterface) == o) {
        *result = TRUE;
    } else {
        AutoPtr<IEGLObjectHandle> display = IEGLObjectHandle::Probe(o);
        Int64 handle, thisHanlde;
        display->GetNativeHandle(&handle);
        GetNativeHandle(&thisHanlde);
        *result = (handle == thisHanlde);
    }
    return NOERROR;
}

AutoPtr<IEGLDisplay> EGLDisplayWrapper::CreateInstance(
    /* [in] */ Int64 handle)
{
    AutoPtr<IEGLDisplay> instance = new EGLDisplayWrapper(handle);
    return instance;
}

EGLDisplayWrapper::EGLDisplayWrapper(
    /* [in] */ Int64 handle)
    : EGLObjectHandle(handle)
{}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos