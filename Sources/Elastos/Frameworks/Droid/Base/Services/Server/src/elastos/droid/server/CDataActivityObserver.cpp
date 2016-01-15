
#include "elastos/droid/server/CDataActivityObserver.h"
#include "elastos/droid/server/CConnectivityService.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {

ECode CDataActivityObserver::InterfaceClassDataActivityChanged(
    /* [in] */ const String& label,
    /* [in] */ Boolean active,
    /* [in] */ Int64 tsNanos)
{
    Int32 deviceType = StringUtils::ParseInt32(label);
    mHost->SendDataActivityBroadcast(deviceType, active, tsNanos);
    return NOERROR;
}

ECode CDataActivityObserver::constructor(
    /* [in] */ IIConnectivityManager* host)
{
    mHost = (CConnectivityService*)host;
    return NOERROR;
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos
