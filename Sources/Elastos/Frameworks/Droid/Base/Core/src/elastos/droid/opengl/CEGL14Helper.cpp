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

#include "elastos/droid/opengl/CEGL14Helper.h"
#include "elastos/droid/opengl/CEGL14.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(CEGL14Helper, Singleton, IEGL14Helper)

CAR_SINGLETON_IMPL(CEGL14Helper)

ECode CEGL14Helper::GetNoContext(
    /* [out] */ IEGLContext** cxt)
{
    VALIDATE_NOT_NULL(cxt)

    *cxt = CEGL14::eglNoContextObject;
    REFCOUNT_ADD(*cxt)
    return NOERROR;
}

ECode CEGL14Helper::SetNoContext(
    /* [in] */ IEGLContext* cxt)
{
    CEGL14::eglNoContextObject = cxt;
    return NOERROR;
}

ECode CEGL14Helper::GetNoDisplay(
    /* [out] */ IEGLDisplay** dsp)
{
    VALIDATE_NOT_NULL(dsp)

    *dsp = CEGL14::eglNoDisplayObject;
    REFCOUNT_ADD(*dsp)
    return NOERROR;
}

ECode CEGL14Helper::SetNoDisplay(
    /* [in] */ IEGLDisplay* dsp)
{
    CEGL14::eglNoDisplayObject = dsp;
    return NOERROR;
}

ECode CEGL14Helper::GetNoSurface(
    /* [out] */ IEGLSurface** sfc)
{
    VALIDATE_NOT_NULL(sfc)

    *sfc = CEGL14::eglNoSurfaceObject;
    REFCOUNT_ADD(*sfc)
    return NOERROR;
}

ECode CEGL14Helper::SetNoSurface(
    /* [in] */ IEGLSurface* sfc)
{
    CEGL14::eglNoSurfaceObject = sfc;
    return NOERROR;
}

}// namespace Opengl
}// namespace Droid
}// namespace Elastos
