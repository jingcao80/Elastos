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

#ifndef __ELASTOS_DROID_OPENGL_CGLUTILS_H__
#define __ELASTOS_DROID_OPENGL_CGLUTILS_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CGLUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLUtils)
    , public Singleton
    , public IGLUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInternalFormat(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ Int32* rst);

    CARAPI GetType(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ Int32* type);

    CARAPI TexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 border);

    CARAPI TexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 type,
        /* [in] */ Int32 border);

    CARAPI TexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 border);

    CARAPI TexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ IBitmap* bitmap);

    CARAPI TexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type);

    CARAPI GetEGLErrorString(
        /* [in] */ Int32 error,
        /* [out] */ String* str);

    CARAPI SetTracingLevel(
        /* [in] */ Int32 level);

private:
    static CARAPI_(Int32) Native_getInternalFormat(
        /* [in] */ IBitmap* bitmap);

    static CARAPI_(Int32) Native_getType(
        /* [in] */ IBitmap* bitmap);

    static CARAPI_(Int32) Native_texImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 type,
        /* [in] */ Int32 border);

    static CARAPI_(Int32) Native_texSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type);
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLUTILS_H__
