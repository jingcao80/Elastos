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

#include "elastos/droid/opengl/gles/CEGLContextHelper.h"
#include "elastos/droid/opengl/gles/CEGLContextImpl.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastosx::Microedition::Khronos::Egl::EIID_IEGLContextHelper;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CAR_INTERFACE_IMPL(CEGLContextHelper, Singleton, IEGLContextHelper)

CAR_SINGLETON_IMPL(CEGLContextHelper)

ECode CEGLContextHelper::GetEGL(
    /* [out] */ IEGL** egl)
{
    VALIDATE_NOT_NULL(egl)
    AutoPtr<IEGL> gl = CEGLContextImpl::GetEGL();
    *egl = gl;
    REFCOUNT_ADD(*egl)
    return NOERROR;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

