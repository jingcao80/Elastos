
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_GAMEPADDEVICE_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_GAMEPADDEVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Build.h"
#include <elrefbase.h>

using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IMotionRange;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;

using Elastos::ElRefBase;

// import java.util.Arrays;
// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Manages information related to each connected gamepad device.
 */
class GamepadDevice : public ElRefBase
{
public:
    GamepadDevice(
        /* [in] */ Int32 index,
        /* [in] */ IInputDevice* inputDevice);

    /**
     * Updates the axes and buttons maping of a gamepad device to a standard gamepad format.
     */
    CARAPI_(void) UpdateButtonsAndAxesMapping();

    /**
     * @return Device Id of the gamepad device.
     */
    CARAPI_(Int32) GetId();

    /**
     * @return Mapping status of the gamepad device.
     */
    CARAPI_(Boolean) IsStandardGamepad();

    /**
     * @return Device name of the gamepad device.
     */
    CARAPI_(String) GetName();

    /**
     * @return Device index of the gamepad device.
     */
    CARAPI_(Int32) GetIndex();

    /**
     * @return The timestamp when the gamepad device was last interacted.
     */
    CARAPI_(Int64) GetTimestamp();

    /**
     * @return The axes state of the gamepad device.
     */
    CARAPI_(AutoPtr< ArrayOf<Float> >) GetAxes();

    /**
     * @return The buttons state of the gamepad device.
     */
    CARAPI_(AutoPtr< ArrayOf<Float> >) GetButtons();

    /**
     * Reset the axes and buttons data of the gamepad device everytime gamepad data access is
     * paused.
     */
    CARAPI_(void) ClearData();

    /**
     * Handles key event from the gamepad device.
     * @return True if the key event from the gamepad device has been consumed.
     */
    CARAPI_(Boolean) HandleKeyEvent(
        /* [in] */ IKeyEvent* event);

    /**
     * Handles motion event from the gamepad device.
     * @return True if the motion event from the gamepad device has been consumed.
     */
    CARAPI_(Boolean) HandleMotionEvent(
        /* [in] */ IMotionEvent* event);

private:
    // An id for the gamepad.
    Int32 mDeviceId;
    // The index of the gamepad in the Navigator.
    Int32 mDeviceIndex;
    // Last time the data for this gamepad was updated.
    Int64 mTimestamp;
    // If this gamepad is mapped to standard gamepad?
    Boolean mIsStandardGamepad;

    // Array of values for all axes of the gamepad.
    // All axis values must be linearly normalized to the range [-1.0 .. 1.0].
    // As appropriate, -1.0 should correspond to "up" or "left", and 1.0
    // should correspond to "down" or "right".
    AutoPtr< ArrayOf<Float> > mAxisValues;

    AutoPtr< ArrayOf<Float> > mButtonsValues;

    // When the user agent recognizes the attached inputDevice, it is recommended
    // that it be remapped to a canonical ordering when possible. Devices that are
    // not recognized should still be exposed in their raw form. Therefore we must
    // pass the raw Button and raw Axis values.
    AutoPtr< ArrayOf<Float> > mRawButtons;
    AutoPtr< ArrayOf<Float> > mRawAxes;

    // An identification string for the gamepad.
    String mDeviceName;

    // Array of axes ids.
    AutoPtr< ArrayOf<Int32> > mAxes;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_GAMEPADDEVICE_H__
