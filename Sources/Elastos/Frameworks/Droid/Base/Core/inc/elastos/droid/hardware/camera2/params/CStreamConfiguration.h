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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CSTREAMCONFIGURTION_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CSTREAMCONFIGURTION_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CStreamConfiguration.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISize;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CStreamConfiguration)
    , public Object
    , public IStreamConfiguration
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CStreamConfiguration();

    virtual ~CStreamConfiguration();

    CARAPI constructor() { return NOERROR; }
    /**
     * Create a new {@link StreamConfiguration}.
     *
     * @param format image format
     * @param width image width, in pixels (positive)
     * @param height image height, in pixels (positive)
     * @param input true if this is an input configuration, false for output configurations
     *
     * @throws IllegalArgumentException
     *              if width/height were not positive
     *              or if the format was not user-defined in ImageFormat/PixelFormat
     *                  (IMPL_DEFINED is ok)
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean input);

    /**
     * Get the internal image {@code format} in this stream configuration.
     *
     * @return an integer format
     *
     * @see ImageFormat
     * @see PixelFormat
     */
    CARAPI GetFormat(
        /* [out] */ Int32* format);


    /**
     * Return the width of the stream configuration.
     *
     * @return width > 0
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * Return the height of the stream configuration.
     *
     * @return height > 0
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * Convenience method to return the size of this stream configuration.
     *
     * @return a Size with positive width and height
     */
    CARAPI GetSize(
        /* [out] */ ISize** size);

    /**
     * Determines if this configuration is usable for input streams.
     *
     * <p>Input and output stream configurations are not interchangeable;
     * input stream configurations must be used when configuring inputs.</p>
     *
     * @return {@code true} if input configuration, {@code false} otherwise
     */
    CARAPI IsInput(
        /* [out] */ Boolean* result);

    /**
     * Determines if this configuration is usable for output streams.
     *
     * <p>Input and output stream configurations are not interchangeable;
     * out stream configurations must be used when configuring outputs.</p>
     *
     * @return {@code true} if output configuration, {@code false} otherwise
     *
     * @see CameraDevice#createCaptureSession
     */
    CARAPI IsOutput(
        /* [out] */ Boolean* result);

    /**
     * Check if this {@link StreamConfiguration} is equal to another {@link StreamConfiguration}.
     *
     * <p>Two vectors are only equal if and only if each of the respective elements is equal.</p>
     *
     * @return {@code true} if the objects were equal, {@code false} otherwise
     */
    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

private:
    Int32 mFormat;
    Int32 mWidth;
    Int32 mHeight;
    Boolean mInput;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CSTREAMCONFIGURTION_H__
