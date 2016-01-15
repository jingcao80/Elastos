
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CSTREAMCONFIGURTIONDURATION_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CSTREAMCONFIGURTIONDURATION_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CStreamConfigurationDuration.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISize;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CStreamConfigurationDuration)
    , public Object
    , public IStreamConfigurationDuration
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CStreamConfigurationDuration();

    virtual ~CStreamConfigurationDuration();

    CARAPI constructor();

    /**
     * Create a new {@link StreamConfigurationDuration}.
     *
     * @param format image format
     * @param width image width, in pixels (positive)
     * @param height image height, in pixels (positive)
     * @param durationNs duration in nanoseconds (non-negative)
     *
     * @throws IllegalArgumentException
     *          if width/height were not positive, or durationNs was negative
     *          or if the format was not user-defined in ImageFormat/PixelFormat
     *              (IMPL_DEFINED is OK)
     *
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int64 durationNs);

    /**
     * Get the internal image {@code format} in this stream configuration duration
     *
     * @return an integer format
     *
     * @see ImageFormat
     * @see PixelFormat
     */
    CARAPI GetFormat(
        /* [out] */ Int32* format);


    /**
     * Return the width of the stream configuration duration.
     *
     * @return width > 0
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * Return the height of the stream configuration duration
     *
     * @return height > 0
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * Convenience method to return the size of this stream configuration duration.
     *
     * @return a Size with positive width and height
     */
    CARAPI GetSize(
        /* [out] */ ISize** size);

    /**
     * Get the time duration (in nanoseconds).
     *
     * @return long >= 0
     */
    CARAPI GetDuration(
        /* [out] */ Int64* duration);

    /**
     * Check if this {@link StreamConfigurationDuration} is equal to another
     * {@link StreamConfigurationDuration}.
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
    Int64 mDurationNs;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CSTREAMCONFIGURTIONDURATION_H__
