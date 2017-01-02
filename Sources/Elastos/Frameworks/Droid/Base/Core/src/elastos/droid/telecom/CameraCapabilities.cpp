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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CameraCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CameraCapabilities::
//===============================================================
CAR_INTERFACE_IMPL_2(CameraCapabilities, Object, ICameraCapabilities, IParcelable)

ECode CameraCapabilities::constructor()
{
    return NOERROR;
}

ECode CameraCapabilities::constructor(
    /* [in] */ Boolean zoomSupported,
    /* [in] */ Float maxZoom,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mZoomSupported = zoomSupported;
    mMaxZoom = maxZoom;
    mWidth = width;
    mHeight = height;
    return NOERROR;
}

ECode CameraCapabilities::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode CameraCapabilities::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Boolean bSupported = FALSE;
    IsZoomSupported(&bSupported);
    dest->WriteByte((Byte) (bSupported ? 1 : 0));
    Float zoom = 0.0f;
    GetMaxZoom(&zoom);
    dest->WriteFloat(zoom);
    Int32 width = 0;
    GetWidth(&width);
    dest->WriteInt32(width);
    Int32 height = 0;
    GetHeight(&height);
    dest->WriteInt32(height);
    return NOERROR;
}

ECode CameraCapabilities::IsZoomSupported(
    /* [out] */ Boolean* zoomSupported)
{
    VALIDATE_NOT_NULL(zoomSupported)
    *zoomSupported = mZoomSupported;
    return NOERROR;
}

ECode CameraCapabilities::GetMaxZoom(
    /* [out] */ Float* zoom)
{
    VALIDATE_NOT_NULL(zoom)
    *zoom = mMaxZoom;
    return NOERROR;
}

ECode CameraCapabilities::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mWidth;
    return NOERROR;
}

ECode CameraCapabilities::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = mHeight;
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos