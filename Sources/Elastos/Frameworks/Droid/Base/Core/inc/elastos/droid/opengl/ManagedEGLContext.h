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

#ifndef __ELASTOS_DROID_OPENGL_MANAGEDEGLCONTEXT_H__
#define __ELASTOS_DROID_OPENGL_MANAGEDEGLCONTEXT_H__

#include "elastos/Mutex.h"
#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
namespace Elastos {
namespace Droid {
namespace Opengl {

class ManagedEGLContext
    : public IManagedEGLContext
    , public ElRefBase
{
typedef Elastosx::Microedition::Khronos::Egl::IEGLConfig XIEGLConfig;
typedef Elastosx::Microedition::Khronos::Egl::IEGLContext XIEGLContext;
typedef Elastosx::Microedition::Khronos::Egl::IEGLDisplay XIEGLDisplay;
typedef Elastosx::Microedition::Khronos::Egl::IEGLSurface XIEGLSurface;

public:
    CAR_INTERFACE_DECL()

    ManagedEGLContext(
        /* [in] */ XIEGLContext* ctx);

    CARAPI GetContext(
        /* [out] */ XIEGLContext** ctx);

    CARAPI Terminate();

    CARAPI ExecTerminate();

    static CARAPI DoTerminate(
        /* [out] */ Boolean* rst);

private:
    AutoPtr<XIEGLContext> mContext;
    static Object sLock;
    static List<AutoPtr<ManagedEGLContext> > sActive;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OPENGL_MANAGEDEGLCONTEXT_H__
