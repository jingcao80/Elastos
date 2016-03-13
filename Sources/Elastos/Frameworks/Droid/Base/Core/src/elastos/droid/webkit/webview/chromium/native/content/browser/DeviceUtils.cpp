
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DeviceUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/ContentSwitches.h"
#include "elastos/droid/webkit/webview/chromium/native/base/CommandLine.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/DeviceFormFactor.h"

using Elastos::Droid::Webkit::Webview::Chromium::Base::CommandLine;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Common::ContentSwitches;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::DeviceFormFactor;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
    return DeviceFormFactor::IsTablet(context);
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
