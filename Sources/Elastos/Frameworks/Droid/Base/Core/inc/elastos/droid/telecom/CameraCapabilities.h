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