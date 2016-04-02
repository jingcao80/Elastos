
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/CRemoteConnectionHelper.h"
#include "elastos/droid/telecomm/telecom/RemoteConnection.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
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
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos