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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYMETATAMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYMETATAMAPPER_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CLegacyMetadataMapper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CLegacyMetadataMapper)
    , public Singleton
    , public ILegacyMetadataMapperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create characteristics for a legacy device by mapping the {@code parameters}
     * and {@code info}
     *
     * @param parameters A non-{@code null} parameters set
     * @param info Camera info with camera facing direction and angle of orientation
     *
     * @return static camera characteristics for a camera device
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    CARAPI CreateCharacteristics(
        /* [in] */ IParameters* parameters,
        /* [in] */ IHardwareCameraInfo* info,
        /* [out] */ ICameraCharacteristics** outcc);

    /**
     * Create characteristics for a legacy device by mapping the {@code parameters}
     * and {@code info}
     *
     * @param parameters A string parseable by {@link Camera.Parameters#unflatten}
     * @param info Camera info with camera facing direction and angle of orientation
     * @return static camera characteristics for a camera device
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    CARAPI CreateCharacteristics(
        /* [in] */ const String& parameters,
        /* [in] */ ICameraInfo* info,
        /* [out] */ ICameraCharacteristics** outcc);

    /**
     * Set the legacy parameters using the {@link LegacyRequest legacy request}.
     *
     * <p>The legacy request's parameters are changed as a side effect of calling this
     * method.</p>
     *
     * @param request a non-{@code null} legacy request
     */
    CARAPI ConvertRequestMetadata(
        /* [in] */ ILegacyRequest* request);

    /**
     * Create a request template
     *
     * @param c a non-{@code null} camera characteristics for this camera
     * @param templateId a non-negative template ID
     *
     * @return a non-{@code null} request template
     *
     * @throws IllegalArgumentException if {@code templateId} was invalid
     *
     * @see android.hardware.camera2.CameraDevice#TEMPLATE_MANUAL
     */
    CARAPI CreateRequestTemplate(
        /* [in] */ ICameraCharacteristics* c,
        /* [in] */ Int32 templateId,
        /* [out] */ ICameraMetadataNative** outcm);
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYMETATAMAPPER_H__
