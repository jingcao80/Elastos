
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CRemoteConnectionHelper.h"
#include "elastos/droid/telecom/RemoteConnection.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CRemoteConnectionHelper::
//===============================================================
CAR_SINGLETON_IMPL(CRemoteConnectionHelper)

CAR_INTERFACE_IMPL(CRemoteConnectionHelper, Singleton, IRemoteConnectionHelper)

ECode CRemoteConnectionHelper::Failure(
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [out] */ IRemoteConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return RemoteConnection::Failure(disconnectCause, result);
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos