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

#include "elastos/droid/opengl/gles/CEGL10Helper.h"
#include "elastos/droid/opengl/gles/CEGLDisplayImpl.h"
#include "elastos/droid/opengl/gles/CEGLContextImpl.h"
#include "elastos/droid/opengl/gles/CEGLSurfaceImpl.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastosx::Microedition::Khronos::Egl::EIID_IEGL10Helper;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

static AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLDisplay> InitDisplay()
{
    AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLDisplay> obj;
    CEGLDisplayImpl::New(0, (Elastosx::Microedition::Khronos::Egl::IEGLDisplay**)&obj);
    return obj;
}

static AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLContext> InitContext()
{
    AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLContext> obj;
    CEGLContextImpl::New(0, (Elastosx::Microedition::Khronos::Egl::IEGLContext**)&obj);
    return obj;
}

static AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLSurface> InitSurface()
{
    AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLSurface> obj;
    CEGLSurfaceImpl::New(0, (Elastosx::Microedition::Khronos::Egl::IEGLSurface**)&obj);
    return obj;
}

AutoPtr<IInterface> CEGL10Helper::sDefaultDisplay = NULL;
AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLDisplay> CEGL10Helper::sNoDisplay = InitDisplay();
AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLContext> CEGL10Helper::sNoContext = InitContext();
AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLSurface> CEGL10Helper::sNoSurface = InitSurface();

CAR_INTERFACE_IMPL(CEGL10Helper, Singleton, IEGL10Helper)
CAR_SINGLETON_IMPL(CEGL10Helper)

ECode CEGL10Helper::GetDefaultDisplay(
    /* [out] */ IInterface** disp)
{
    VALIDATE_NOT_NULL(disp)

    *disp = sDefaultDisplay;
    REFCOUNT_ADD(*disp);
    return NOERROR;
}

ECode CEGL10Helper::GetNoDisplay(
    /* [out] */ XIEGLDisplay** disp)
{
    VALIDATE_NOT_NULL(disp)

    *disp = sNoDisplay;
    REFCOUNT_ADD(*disp);
    return NOERROR;
}

ECode CEGL10Helper::GetNoContext(
    /* [out] */ XIEGLContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)

    *ctx = sNoContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode CEGL10Helper::GetNoSurface(
    /* [out] */ XIEGLSurface** sfc)
{
    VALIDATE_NOT_NULL(sfc)

    *sfc = sNoSurface;
    REFCOUNT_ADD(*sfc);
    return NOERROR;
}


} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos