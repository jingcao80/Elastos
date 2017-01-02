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

#ifndef __ELASTOS_DROID_OPENGL_CETC1TEXTURE_H__
#define __ELASTOS_DROID_OPENGL_CETC1TEXTURE_H__

#include "Elastos.Droid.Opengl.h"
#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_Droid_Opengl_CETC1Texture.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CETC1Texture)
    , public Object
    , public IETC1Texture
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI GetData(
        /* [out] */ Elastos::IO::IByteBuffer** data);

    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Elastos::IO::IByteBuffer* data);

private:
    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<IByteBuffer> mData;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CETC1TEXTURE_H__
