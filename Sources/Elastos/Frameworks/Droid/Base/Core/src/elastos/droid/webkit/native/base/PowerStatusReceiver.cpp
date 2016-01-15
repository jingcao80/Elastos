
#include "elastos/droid/webkit/native/base/PowerMonitor.h"
#include "elastos/droid/webkit/native/base/PowerStatusReceiver.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos