
#include "elastos/droid/webkit/webview/chromium/native/base/PowerMonitor.h"
#include "elastos/droid/webkit/webview/chromium/native/base/PowerStatusReceiver.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

//@Override
ECode PowerStatusReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    PowerMonitor::OnBatteryChargingChanged(intent);
    return NOERROR;
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
