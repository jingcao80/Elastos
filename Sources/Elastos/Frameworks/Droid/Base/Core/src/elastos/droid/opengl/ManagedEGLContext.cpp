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

#include "elastos/droid/os/Looper.h"
#include "elastos/droid/opengl/ManagedEGLContext.h"
#include "elastos/droid/opengl/gles/CEGL10Helper.h"
#include "elastos/droid/opengl/gles/CEGLImpl.h"
#include "elastos/droid/opengl/gles/CEGLContextImpl.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Looper;
using Elastos::Droid::Opengl::gles::CEGLImpl;
using Elastos::Droid::Opengl::gles::CEGL10Helper;
using Elastos::Droid::Opengl::gles::CEGLContextImpl;
using Elastosx::Microedition::Khronos::Egl::IEGL10Helper;
using Elastosx::Microedition::Khronos::Egl::IEGL10;

namespace Elastos {
namespace Droid {
namespace Opengl {

Mutex ManagedEGLContext::sLock;
List<AutoPtr<ManagedEGLContext> > ManagedEGLContext::sActive;

CAR_INTERFACE_IMPL(ManagedEGLContext, IManagedEGLContext)

ManagedEGLContext::ManagedEGLContext(
    /* [in] */ XIEGLContext* ctx)
{
    mContext = ctx;
    AutoLock lock(sLock);
    sActive.PushBack(this);
}

ECode ManagedEGLContext::GetContext(
    /* [out] */ XIEGLContext** ctx)
{
    *ctx = mContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode ManagedEGLContext::Terminate()
{
    return ExecTerminate();
}

ECode ManagedEGLContext::ExecTerminate()
{
    return OnTerminate(mContext);
}

ECode ManagedEGLContext::DoTerminate(
    /* [out] */ Boolean* rst)
{
    List<AutoPtr<ManagedEGLContext> > active;
    if (Looper::GetMainLooper() != Looper::GetMyLooper()) {
        SLOGGERE("ManagedEGLContext", "Called on wrong thread")
        *rst = FALSE;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    {
        AutoLock lock(sLock);
        // If there are no active managed contexts, we will not even
        // try to terminate.
        if (sActive.GetSize() <= 0) {
            *rst = FALSE;
            return NOERROR;
        }

        // Need to check how many EGL contexts are actually running,
        // to compare with how many we are managing.
        AutoPtr<IEGL10> egl = (IEGL10*)((CEGLContextImpl::GetEGL()).Get());
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLDisplay> display;
        AutoPtr<IInterface> defaultDisplay;
        helper->GetDefaultDisplay((IInterface**)&defaultDisplay);
        egl->EglGetDisplay(defaultDisplay,(XIEGLDisplay**)&display);

        AutoPtr<XIEGLDisplay> noDisplay;
        helper->GetNoDisplay((XIEGLDisplay**)&noDisplay);
        if (display == noDisplay) {
            SLOGGERW("ManagedEGLContext", "doTerminate failed: no display")
            *rst = FALSE;
            return NOERROR;
        }

        if ((UInt32)CEGLImpl::GetInitCount(display) != sActive.GetSize()) {
            SLOGGERW("ManagedEGLContext", "doTerminate failed: EGL count is %d but managed count is %d",
                CEGLImpl::GetInitCount(display), sActive.GetSize())
            *rst = FALSE;
            return NOERROR;
        }

        active = sActive;
        sActive.Clear();
    }

    List<AutoPtr<ManagedEGLContext> >::Iterator it = active.Begin();
    for (; it != active.End(); it++) {
        AutoPtr<ManagedEGLContext> managedCtx = *it;
        managedCtx->ExecTerminate();
    }

    *rst = TRUE;
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos