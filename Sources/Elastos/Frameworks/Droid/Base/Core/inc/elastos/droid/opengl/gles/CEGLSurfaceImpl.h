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

#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLSURFACEIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLSURFACEIMPL_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_Gles_CEGLSurfaceImpl.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {
class CEGLImpl;

CarClass(CEGLSurfaceImpl)
    , public Object
    , public Elastosx::Microedition::Khronos::Egl::IEGLSurface
{
    friend class CEGLImpl;
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 surface);

    CARAPI Equals(
        /* [in] */ IInterface *object,
        /* [out] */ Boolean *equals);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI_(Int64) GetEGLSurface();

private:
    Int64 mEGLSurface;
    Int64 mNativePixelRef;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_GLES_CEGLSURFACEIMPL_H__
