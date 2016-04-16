
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/GamepadMappings.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/CanonicalAxisIndex.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/CanonicalButtonIndex.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

const String GamepadMappings::NVIDIA_SHIELD_DEVICE_NAME_PREFIX("NVIDIA Corporation NVIDIA Controller");
const String GamepadMappings::MICROSOFT_XBOX_PAD_DEVICE_NAME("Microsoft X-Box 360 pad");
const String GamepadMappings::PS3_SIXAXIS_DEVICE_NAME("Sony PLAYSTATION(R)3 Controller");
const String GamepadMappings::SAMSUNG_EI_GP20_DEVICE_NAME("Samsung Game Pad EI-GP20");

Boolean GamepadMappings::MapToStandardGamepad(
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawAxes,
    /* [in] */ ArrayOf<Float>*rawButtons,
    /* [in] */ const String& deviceName)
{
    if (deviceName.StartWith(NVIDIA_SHIELD_DEVICE_NAME_PREFIX)) {
        MapShieldGamepad(mappedButtons, rawButtons, mappedAxes, rawAxes);
        return TRUE;
    }
    else if (deviceName.Equals(MICROSOFT_XBOX_PAD_DEVICE_NAME)) {
        MapXBox360Gamepad(mappedButtons, rawButtons, mappedAxes, rawAxes);
        return TRUE;
    }
    else if (deviceName.Equals(PS3_SIXAXIS_DEVICE_NAME)) {
        MapPS3SixAxisGamepad(mappedButtons, rawButtons, mappedAxes, rawAxes);
        return TRUE;
    }
    else if (deviceName.Equals(SAMSUNG_EI_GP20_DEVICE_NAME)) {
        MapSamsungEIGP20Gamepad(mappedButtons, rawButtons, mappedAxes, rawAxes);
        return TRUE;
    }

    MapUnknownGamepad(mappedButtons, rawButtons, mappedAxes, rawAxes);

    return FALSE;
}

void GamepadMappings::MapCommonXYABButtons(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons)
{
    Float a = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_A];
    Float b = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_B];
    Float x = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_X];
    Float y = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_Y];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_PRIMARY] = a;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_SECONDARY] = b;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_TERTIARY] = x;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_QUATERNARY] = y;
}

void GamepadMappings::MapCommonStartSelectMetaButtons(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons)
{
    Float start = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_START];
    Float select = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_SELECT];
    Float mode = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_MODE];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_START] = start;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_BACK_SELECT] = select;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_META] = mode;
}

void GamepadMappings::MapCommonThumbstickButtons(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons)
{
    Float thumbL = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_THUMBL];
    Float thumbR = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_THUMBR];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_LEFT_THUMBSTICK] = thumbL;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_RIGHT_THUMBSTICK] = thumbR;
}

void GamepadMappings::MapCommonTriggerButtons(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons)
{
    Float l1 = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_L1];
    Float r1 = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_R1];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_LEFT_TRIGGER] = l1;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_RIGHT_TRIGGER] = r1;
}

void GamepadMappings::MapCommonDpadButtons(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons)
{
    Float dpadDown = (*rawButtons)[IKeyEvent::KEYCODE_DPAD_DOWN];
    Float dpadUp = (*rawButtons)[IKeyEvent::KEYCODE_DPAD_UP];
    Float dpadLeft = (*rawButtons)[IKeyEvent::KEYCODE_DPAD_LEFT];
    Float dpadRight = (*rawButtons)[IKeyEvent::KEYCODE_DPAD_RIGHT];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_DOWN] = dpadDown;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_UP] = dpadUp;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_LEFT] = dpadLeft;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_RIGHT] = dpadRight;
}

void GamepadMappings::MapXYAxes(
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    (*mappedAxes)[CanonicalAxisIndex::AXIS_LEFT_STICK_X] = (*rawAxes)[IMotionEvent::AXIS_X];
    (*mappedAxes)[CanonicalAxisIndex::AXIS_LEFT_STICK_Y] = (*rawAxes)[IMotionEvent::AXIS_Y];
}

void GamepadMappings::MapRXAndRYAxesToRightStick(
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    (*mappedAxes)[CanonicalAxisIndex::AXIS_RIGHT_STICK_X] = (*rawAxes)[IMotionEvent::AXIS_RX];
    (*mappedAxes)[CanonicalAxisIndex::AXIS_RIGHT_STICK_Y] = (*rawAxes)[IMotionEvent::AXIS_RY];
}

void GamepadMappings::MapZAndRZAxesToRightStick(
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    (*mappedAxes)[CanonicalAxisIndex::AXIS_RIGHT_STICK_X] = (*rawAxes)[IMotionEvent::AXIS_Z];
    (*mappedAxes)[CanonicalAxisIndex::AXIS_RIGHT_STICK_Y] = (*rawAxes)[IMotionEvent::AXIS_RZ];
}

void GamepadMappings::MapTriggerAxexToShoulderButtons(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    Float lTrigger = (*rawAxes)[IMotionEvent::AXIS_LTRIGGER];
    Float rTrigger = (*rawAxes)[IMotionEvent::AXIS_RTRIGGER];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_LEFT_SHOULDER] = lTrigger;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_RIGHT_SHOULDER] = rTrigger;
}

Float GamepadMappings::NegativeAxisValueAsButton(
    /* [in] */ Float input)
{
    return (input < -0.5f) ? 1.f : 0.f;
}

Float GamepadMappings::PositiveAxisValueAsButton(
    /* [in] */ Float input)
{
    return (input > 0.5f) ? 1.f : 0.f;
}

void GamepadMappings::MapHatAxisToDpadButtons(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    Float hatX = (*rawAxes)[IMotionEvent::AXIS_HAT_X];
    Float hatY = (*rawAxes)[IMotionEvent::AXIS_HAT_Y];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_LEFT] = NegativeAxisValueAsButton(hatX);
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_RIGHT] = PositiveAxisValueAsButton(hatX);
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_UP] = NegativeAxisValueAsButton(hatY);
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_DPAD_DOWN] = PositiveAxisValueAsButton(hatY);
}

/**
 * Method for mapping Nvidia gamepad axis and button values
 * to standard gamepad button and axes values.
 */
void GamepadMappings::MapShieldGamepad(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons,
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    MapCommonXYABButtons(mappedButtons, rawButtons);
    MapCommonTriggerButtons(mappedButtons, rawButtons);
    MapCommonThumbstickButtons(mappedButtons, rawButtons);
    MapCommonStartSelectMetaButtons(mappedButtons, rawButtons);
    MapTriggerAxexToShoulderButtons(mappedButtons, rawAxes);
    MapHatAxisToDpadButtons(mappedButtons, rawAxes);

    MapXYAxes(mappedAxes, rawAxes);
    MapZAndRZAxesToRightStick(mappedAxes, rawAxes);
}

/**
 * Method for mapping Microsoft XBox 360 gamepad axis and button values
 * to standard gamepad button and axes values.
 */
void GamepadMappings::MapXBox360Gamepad(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons,
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    // These are actually mapped the same way in Android.
    MapShieldGamepad(mappedButtons, rawButtons, mappedAxes, rawAxes);
}

void GamepadMappings::MapPS3SixAxisGamepad(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons,
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    // On PS3 X/Y has higher priority.
    Float a = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_A];
    Float b = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_B];
    Float x = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_X];
    Float y = (*rawButtons)[IKeyEvent::KEYCODE_BUTTON_Y];
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_PRIMARY] = x;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_SECONDARY] = y;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_TERTIARY] = a;
    (*mappedButtons)[CanonicalButtonIndex::BUTTON_QUATERNARY] = b;

    MapCommonTriggerButtons(mappedButtons, rawButtons);
    MapCommonThumbstickButtons(mappedButtons, rawButtons);
    MapCommonDpadButtons(mappedButtons, rawButtons);
    MapCommonStartSelectMetaButtons(mappedButtons, rawButtons);
    MapTriggerAxexToShoulderButtons(mappedButtons, rawAxes);

    MapXYAxes(mappedAxes, rawAxes);
    MapZAndRZAxesToRightStick(mappedAxes, rawAxes);
}

void GamepadMappings::MapSamsungEIGP20Gamepad(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons,
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    MapCommonXYABButtons(mappedButtons, rawButtons);
    MapCommonTriggerButtons(mappedButtons, rawButtons);
    MapCommonThumbstickButtons(mappedButtons, rawButtons);
    MapCommonStartSelectMetaButtons(mappedButtons, rawButtons);
    MapHatAxisToDpadButtons(mappedButtons, rawAxes);

    MapXYAxes(mappedAxes, rawAxes);
    MapRXAndRYAxesToRightStick(mappedAxes, rawAxes);
}

/**
 * Method for mapping Unkown gamepad axis and button values
 * to standard gamepad button and axes values.
 */
void GamepadMappings::MapUnknownGamepad(
    /* [in] */ ArrayOf<Float>*mappedButtons,
    /* [in] */ ArrayOf<Float>*rawButtons,
    /* [in] */ ArrayOf<Float>*mappedAxes,
    /* [in] */ ArrayOf<Float>*rawAxes)
{
    MapCommonXYABButtons(mappedButtons, rawButtons);
    MapCommonTriggerButtons(mappedButtons, rawButtons);
    MapCommonThumbstickButtons(mappedButtons, rawButtons);
    MapCommonStartSelectMetaButtons(mappedButtons, rawButtons);
    MapTriggerAxexToShoulderButtons(mappedButtons, rawAxes);
    MapCommonDpadButtons(mappedButtons, rawButtons);

    MapXYAxes(mappedAxes, rawAxes);
    MapRXAndRYAxesToRightStick(mappedAxes, rawAxes);
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
