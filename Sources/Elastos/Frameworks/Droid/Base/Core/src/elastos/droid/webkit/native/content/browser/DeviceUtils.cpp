
#include "elastos/droid/webkit/native/content/browser/DeviceUtils.h"
#include "elastos/droid/webkit/native/content/common/ContentSwitches.h"
#include "elastos/droid/webkit/native/base/CommandLine.h"
//TODO #include "elastos/droid/webkit/native/ui/base/DeviceFormFactor.h"

using Elastos::Droid::Webkit::Base::CommandLine;
using Elastos::Droid::Webkit::Content::Common::ContentSwitches;
//TODO using Elastos::Droid::Webkit::Ui::Base::DeviceFormFactor;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 * @param context Android's context
 * @return        Whether the app is should treat the device as a tablet for layout.
 */
// TODO(tedchoc): Transition all call sites to use DeviceFormFactor directly.  Then
//                remove this method.
Boolean DeviceUtils::IsTablet(
    /* [in] */ IContext* context)
{
    assert(0);
    // TODO
    // return DeviceFormFactor::IsTablet(context);
    return FALSE;
}

/**
 * Appends the switch specifying which user agent should be used for this device.
 * @param context The context for the caller activity.
 */
void DeviceUtils::AddDeviceSpecificUserAgentSwitch(
    /* [in] */ IContext* context)
{
    if (!IsTablet(context)) {
        CommandLine::GetInstance()->AppendSwitch(ContentSwitches::USE_MOBILE_UA);
    }
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
