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

#include "elastos/droid/opengl/CGLDebugHelper.h"
#include "elastos/droid/opengl/GLErrorWrapper.h"
#include "elastos/droid/opengl/GLLogWrapper.h"
#include "elastos/droid/opengl/EGLLogWrapper.h"
#include "elastos/droid/ext/frameworkext.h"

#include "Elastos.CoreLibrary.IO.h"

using Elastosx::Microedition::Khronos::Opengles::EIID_IGL;

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL(CGLDebugHelper, Singleton, IGLDebugHelper)

CAR_SINGLETON_IMPL(CGLDebugHelper)

ECode CGLDebugHelper::Wrap(
    /* [in] */ IGL* gl,
    /* [in] */ Int32 configFlags,
    /* [in] */ Elastos::IO::IWriter* log,
    /* [out] */ IGL** glInstance)
{
    VALIDATE_NOT_NULL(glInstance)

    AutoPtr<IGL> rst;
    if ( configFlags != 0 ) {
        rst = (IGL*)(new GLErrorWrapper(gl, configFlags))->Probe(EIID_IGL);
    }
    if ( log != NULL ) {
        Boolean logArgumentNames =
            (IGLDebugHelper::_CONFIG_LOG_ARGUMENT_NAMES & configFlags) != 0;
        rst = (IGL*)(new GLLogWrapper(gl, log, logArgumentNames))->Probe(EIID_IGL);
    }
    *glInstance = rst;
    REFCOUNT_ADD(*glInstance)
    return NOERROR;
}

ECode CGLDebugHelper::Wrap(
    /* [in] */ IEGL* egl,
    /* [in] */ Int32 configFlags,
    /* [in] */ Elastos::IO::IWriter* log,
    /* [out] */ IEGL** eglInterface)
{
    VALIDATE_NOT_NULL(eglInterface)

    if (log != NULL) {
        IEGL11* tmp = new EGLLogWrapper(egl, configFlags, log);
        *eglInterface = IEGL::Probe(tmp);
    }
    REFCOUNT_ADD(*eglInterface)
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

