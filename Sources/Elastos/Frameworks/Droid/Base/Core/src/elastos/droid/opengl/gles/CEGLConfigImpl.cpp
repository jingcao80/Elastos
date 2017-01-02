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

#include "elastos/droid/opengl/gles/CEGLConfigImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

UInt32 CEGLConfigImpl::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CEGLConfigImpl::Release()
{
    return ElRefBase::Release();
}

PInterface CEGLConfigImpl::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLConfig*)this;
    }
    else if (riid == Elastosx::Microedition::Khronos::Egl::EIID_IEGLConfig) {
        return (Elastosx::Microedition::Khronos::Egl::IEGLConfig*)this;
    }
    return Object::Probe(riid);
}

ECode CEGLConfigImpl::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid)

    if (object == (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLConfig*)this) {
        *iid = Elastosx::Microedition::Khronos::Egl::EIID_IEGLConfig;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

CAR_OBJECT_IMPL(CEGLConfigImpl)

ECode CEGLConfigImpl::constructor(
    /* [in] */ Int64 config)
{
    mEGLConfig = config;
    return NOERROR;
}

Int64 CEGLConfigImpl::GetEGLConfig()
{
    return mEGLConfig;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos
