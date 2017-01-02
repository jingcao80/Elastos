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

#ifndef __ELASTOS_DROID_OPENGL_CGLDEBUGHELPER_H__
#define __ELASTOS_DROID_OPENGL_CGLDEBUGHELPER_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CGLDebugHelper.h"
#include <elastos/core/Singleton.h>

using Elastosx::Microedition::Khronos::Opengles::IGL;
using Elastosx::Microedition::Khronos::Egl::IEGL;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLDebugHelper)
    , public Singleton
    , public IGLDebugHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Wrap(
        /* [in] */ IGL* gl,
        /* [in] */ Int32 configFlags,
        /* [in] */ Elastos::IO::IWriter* log,
        /* [out] */ IGL** glInstance);

    CARAPI Wrap(
        /* [in] */ IEGL* egl,
        /* [in] */ Int32 configFlags,
        /* [in] */ Elastos::IO::IWriter* log,
        /* [out] */ IEGL** eglInterface);

private:
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLDEBUGHELPER_H__
