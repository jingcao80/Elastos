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

#include "elastos/droid/opengl/CETC1Texture.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.CoreLibrary.IO.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL(CETC1Texture, Object, IETC1Texture)

CAR_OBJECT_IMPL(CETC1Texture)

ECode CETC1Texture::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)

    *width = mWidth;
    return NOERROR;
}

ECode CETC1Texture::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)

    *height = mHeight;
    return NOERROR;
}

ECode CETC1Texture::GetData(
    /* [out] */ Elastos::IO::IByteBuffer** data)
{
    VALIDATE_NOT_NULL(data)

    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CETC1Texture::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Elastos::IO::IByteBuffer* data)
{
    mWidth = width;
    mHeight = height;
    mData = data;
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

