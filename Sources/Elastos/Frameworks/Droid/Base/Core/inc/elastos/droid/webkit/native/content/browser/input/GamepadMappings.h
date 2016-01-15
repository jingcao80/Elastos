
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_GAMEPADMAPPINGS_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_GAMEPADMAPPINGS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;

// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Class to manage mapping information related to each supported gamepad controller device.
 */
//@JNINamespace("content")
class GamepadMappings
{
public:
    static CARAPI_(Boolean) MapToStandardGamepad(
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawAxes,
        /* [in] */ ArrayOf<Float>* rawButtons,
        /* [in] */ const String& deviceName);

private:
    static const String NVIDIA_SHIELD_DEVICE_NAME_PREFIX;
    static const String MICROSOFT_XBOX_PAD_DEVICE_NAME;
    static const String PS3_SIXAXIS_DEVICE_NAME;
    static const String SAMSUNG_EI_GP20_DEVICE_NAME;

private:
    static CARAPI_(void) MapCommonXYABButtons(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons);

    static CARAPI_(void) MapCommonStartSelectMetaButtons(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons);

    static CARAPI_(void) MapCommonThumbstickButtons(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons);

    static CARAPI_(void) MapCommonTriggerButtons(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons);

    static CARAPI_(void) MapCommonDpadButtons(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons);

    static CARAPI_(void) MapXYAxes(
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);

    static CARAPI_(void) MapRXAndRYAxesToRightStick(
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);

    static CARAPI_(void) MapZAndRZAxesToRightStick(
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);

    static CARAPI_(void) MapTriggerAxexToShoulderButtons(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawAxes);

    static CARAPI_(Float) NegativeAxisValueAsButton(
        /* [in] */ Float input);

    static CARAPI_(Float) PositiveAxisValueAsButton(
        /* [in] */ Float input);

    static CARAPI_(void) MapHatAxisToDpadButtons(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawAxes);

    /**
     * Method for mapping Nvidia gamepad axis and button values
     * to standard gamepad button and axes values.
     */
    static CARAPI_(void) MapShieldGamepad(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons,
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);

    /**
     * Method for mapping Microsoft XBox 360 gamepad axis and button values
     * to standard gamepad button and axes values.
     */
    static CARAPI_(void) MapXBox360Gamepad(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons,
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);

    static CARAPI_(void) MapPS3SixAxisGamepad(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons,
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);

    static CARAPI_(void) MapSamsungEIGP20Gamepad(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons,
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);

    /**
     * Method for mapping Unkown gamepad axis and button values
     * to standard gamepad button and axes values.
     */
    static CARAPI_(void) MapUnknownGamepad(
        /* [in] */ ArrayOf<Float>* mappedButtons,
        /* [in] */ ArrayOf<Float>* rawButtons,
        /* [in] */ ArrayOf<Float>* mappedAxes,
        /* [in] */ ArrayOf<Float>* rawAxes);
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_GAMEPADMAPPINGS_H__
