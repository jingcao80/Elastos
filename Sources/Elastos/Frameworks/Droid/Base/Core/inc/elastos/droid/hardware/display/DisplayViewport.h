
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_DISPLAYVIEWPORT_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_DISPLAYVIEWPORT_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Describes how the pixels of physical display device reflects the content of
 * a logical display.
 * <p>
 * This information is used by the input system to translate touch input from
 * physical display coordinates into logical display coordinates.
 * </p>
 *
 * @hide Only for use within the system server.
 */
class DisplayViewport
    : public Object
    , public IDisplayViewport
{
public:
    CAR_INTERFACE_DECL()

    DisplayViewport();

    virtual ~DisplayViewport();

    CARAPI constructor();

    CARAPI CopyFrom(
        /* [in] */ IDisplayViewport* viewport);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI IsValid(
        /* [out] */ Boolean* valid);

    CARAPI SetValid(
        /* [in] */ Boolean valid);

    CARAPI SetDisplayId(
        /* [in] */ Int32 displayId);

    CARAPI GetDisplayId(
        /* [out] */ Int32* displayId);

    CARAPI SetOrientation(
        /* [in] */ Int32 orientation);

    CARAPI GetOrientation(
        /* [out] */ Int32* orientation);

    CARAPI SetLogicalFrame(
        /* [in] */ IRect* rect);

    CARAPI GetLogicalFrame(
        /* [out] */ IRect** rect);

    CARAPI SetPhysicalFrame(
        /* [in] */ IRect* rect);

    CARAPI GetPhysicalFrame(
        /* [out] */ IRect** rect);

    CARAPI SetDeviceWidth(
        /* [in] */ Int32 width);

    CARAPI GetDeviceWidth(
        /* [out] */ Int32* width);

    CARAPI SetDeviceHeight(
        /* [in] */ Int32 height);

    CARAPI GetDeviceHeight(
        /* [out] */ Int32* height);

public:
    // True if this viewport is valid.
    Boolean mValid;

    // The logical display id.
    Int32 mDisplayId;

    // The rotation applied to the physical coordinate system.
    Int32 mOrientation;

    // The portion of the logical display that are presented on this physical display.
    AutoPtr<IRect> mLogicalFrame;// = new Rect();

    // The portion of the (rotated) physical display that shows the logical display contents.
    // The relation between logical and physical frame defines how the coordinate system
    // should be scaled or translated after rotation.
    AutoPtr<IRect> mPhysicalFrame;// = new Rect();

    // The full width and height of the display device, rotated in the same
    // manner as physicalFrame.  This expresses the full native size of the display device.
    // The physical frame should usually fit within this area.
    Int32 mDeviceWidth;
    Int32 mDeviceHeight;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_DISPLAYVIEWPORT_H__
