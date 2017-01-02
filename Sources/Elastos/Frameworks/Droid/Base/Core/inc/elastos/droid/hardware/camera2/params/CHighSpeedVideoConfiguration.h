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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CHIGHSPEEDVIDEOCONFIGURATION_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CHIGHSPEEDVIDEOCONFIGURATION_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CHighSpeedVideoConfiguration.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/Range.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::Range;
using Elastos::Core::Object;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CHighSpeedVideoConfiguration)
    , public Object
    , public IHighSpeedVideoConfiguration
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CHighSpeedVideoConfiguration();

    virtual ~CHighSpeedVideoConfiguration();

    CARAPI constructor();

    /**
     * Create a new {@link HighSpeedVideoConfiguration}.
     *
     * @param width image width, in pixels (positive)
     * @param height image height, in pixels (positive)
     * @param fpsMin minimum frames per second for the configuration (positive)
     * @param fpsMax maximum frames per second for the configuration (larger or equal to 60)
     *
     * @throws IllegalArgumentException
     *              if width/height/fpsMin were not positive or fpsMax less than 60
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 fpsMin,
        /* [in] */ Int32 fpsMax);

    /**
     * Return the width of the high speed video configuration.
     *
     * @return width > 0
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * Return the height of the high speed video configuration.
     *
     * @return height > 0
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * Return the minimum frame per second of the high speed video configuration.
     *
     * @return fpsMin > 0
     */
    CARAPI GetFpsMin(
        /* [out] */ Int32* min);

    /**
     * Return the maximum frame per second of the high speed video configuration.
     *
     * @return fpsMax >= 60
     */
    CARAPI GetFpsMax(
        /* [out] */ Int32* max);

    /**
     * Convenience method to return the size of this high speed video configuration.
     *
     * @return a Size with positive width and height
     */
    CARAPI GetSize(
        /* [out] */ ISize** size);

    /**
     * Convenience method to return the FPS range of this high speed video configuration.
     *
     * @return a Range with high bound >= 60
     */
    CARAPI GetFpsRange(
        /* [out] */ IInterface** range);

    /**
     * Check if this {@link HighSpeedVideoConfiguration} is equal to another
     * {@link HighSpeedVideoConfiguration}.
     *
     * <p>Two configurations are equal if and only if each of the respective elements is equal.</p>
     *
     * @return {@code true} if the objects were equal, {@code false} otherwise
     */
    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equal);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

private:
    Int32 mWidth;
    Int32 mHeight;
    Int32 mFpsMin;
    Int32 mFpsMax;
    AutoPtr<ISize> mSize;
    AutoPtr<Range<IInteger32> > mFpsRange;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CHIGHSPEEDVIDEOCONFIGURATION_H__
