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

#include "elastos/droid/graphics/CImageFormat.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CImageFormat);
CAR_INTERFACE_IMPL(CImageFormat, Singleton, IImageFormat);
ECode CImageFormat::GetBitsPerPixel(
    /* [in] */ Int32 format,
    /* [out] */ Int32* bits)
{
    VALIDATE_NOT_NULL(bits);
    switch (format) {
        case IImageFormat::RGB_565:
        case IImageFormat::NV16:
        case IImageFormat::YUY2:
        case IImageFormat::Y16:
        case IImageFormat::RAW_SENSOR: {
            *bits = 16;
            return NOERROR;
        }
        case IImageFormat::Y8: {
            *bits = 8;
            return NOERROR;
        }
        case IImageFormat::YV12:
        case IImageFormat::NV21:
        case IImageFormat::YUV_420_888: {
            *bits = 12;
            return NOERROR;
        }
        case IImageFormat::RAW10: {
            *bits = 10;
            return NOERROR;
        }
    }

    *bits = -1;
    return NOERROR;
}

ECode CImageFormat::IsPublicFormat(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* is)
{
    VALIDATE_NOT_NULL(is);
    switch (format) {
        case IImageFormat::RGB_565:
        case IImageFormat::NV16:
        case IImageFormat::YUY2:
        case IImageFormat::YV12:
        case IImageFormat::JPEG:
        case IImageFormat::NV21:
        case IImageFormat::YUV_420_888:
        case IImageFormat::RAW_SENSOR:
        case IImageFormat::RAW10: {
            *is = TRUE;
            return NOERROR;
        }
    }

    *is = FALSE;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos