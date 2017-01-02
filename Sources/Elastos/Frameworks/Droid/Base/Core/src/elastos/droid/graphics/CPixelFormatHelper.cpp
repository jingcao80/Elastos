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

#include "elastos/droid/graphics/CPixelFormatHelper.h"
#include "elastos/droid/graphics/PixelFormat.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(CPixelFormatHelper, Singleton, IPixelFormatHelper)
CAR_SINGLETON_IMPL(CPixelFormatHelper);

ECode CPixelFormatHelper::FormatHasAlpha(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PixelFormat::FormatHasAlpha(format);
    return NOERROR;
}

ECode CPixelFormatHelper::GetPixelFormatInfo(
    /* [in] */ Int32 format,
    /* [in] */ IPixelFormat* info)
{
    return PixelFormat::GetPixelFormatInfo(format, info);
}

ECode CPixelFormatHelper::IsPublicFormat(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PixelFormat::IsPublicFormat(format);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
