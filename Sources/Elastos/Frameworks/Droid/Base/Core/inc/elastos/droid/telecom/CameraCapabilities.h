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

#ifndef __ELASTOS_DROID_TELECOM_CAMERACAPABILITIES_H__
#define __ELASTOS_DROID_TELECOM_CAMERACAPABILITIES_H__

#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Represents the camera capabilities important to a Video Telephony provider.
 * @hide
 */
class CameraCapabilities
    : public Object
    , public ICameraCapabilities
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Create a call camera capabilities instance.
     *
     * @param zoomSupported True when camera supports zoom.
     * @param maxZoom Maximum zoom supported by camera.
     * @param width The width of the camera video (in pixels).
     * @param height The height of the camera video (in pixels).
     */
    CARAPI constructor(
        /* [in] */ Boolean zoomSupported,
        /* [in] */ Float maxZoom,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Flatten this object in to a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     *              May be 0 or {@link #PARCELABLE_WRITE_RETURN_VALUE}.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Whether the camera supports zoom.
     */
    CARAPI IsZoomSupported(
        /* [out] */ Boolean* zoomSupported);

    /**
     * The maximum zoom supported by the camera.
     */
    CARAPI GetMaxZoom(
        /* [out] */ Float* zoom);

    /**
     * The width of the camera video in pixels.
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * The height of the camera video in pixels.
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

private:
    /**
     * Whether the camera supports zoom.
     */
    Boolean mZoomSupported;

    /**
     * The maximum zoom supported by the camera.
     */
    Float mMaxZoom;

    /**
     * The width of the camera video in pixels.
     */
    Int32 mWidth;

    /**
     * The height of the camera video in pixels.
     */
    Int32 mHeight;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CAMERACAPABILITIES_H__