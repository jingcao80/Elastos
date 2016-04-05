
#include "elastos/droid/settings/inputmethod/KeyboardLayoutPickerFragment.h"

using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String KeyboardLayoutPickerFragment::EXTRA_INPUT_DEVICE_IDENTIFIER("input_device_identifier");

CAR_INTERFACE_IMPL(KeyboardLayoutPickerFragment, SettingsPreferenceFragment, IInputDeviceListener);

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos
