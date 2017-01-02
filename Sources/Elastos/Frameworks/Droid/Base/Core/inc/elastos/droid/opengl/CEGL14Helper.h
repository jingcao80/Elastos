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

#ifndef __ELASTOS_DROID_OPENGL_CEGL14HELPER_H__
#define __ELASTOS_DROID_OPENGL_CEGL14HELPER_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CEGL14Helper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CEGL14Helper)
    , public Singleton
    , public IEGL14Helper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNoContext(
        /* [out] */ IEGLContext** cxt);

    CARAPI SetNoContext(
        /* [in] */ IEGLContext* cxt);

    CARAPI GetNoDisplay(
        /* [out] */ IEGLDisplay** dsp);

    CARAPI SetNoDisplay(
        /* [in] */ IEGLDisplay* dsp);

    CARAPI GetNoSurface(
        /* [out] */ IEGLSurface** sfc);

    CARAPI SetNoSurface(
        /* [in] */ IEGLSurface* sfc);
};

}// namespace Opengl
}// namespace Droid
}// namespace Elastos

#endif
