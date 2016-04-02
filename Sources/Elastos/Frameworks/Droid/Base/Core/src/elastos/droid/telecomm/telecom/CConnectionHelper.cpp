
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/CConnectionHelper.h"
#include "elastos/droid/telecomm/telecom/Connection.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// CConnectionHelper::
//===============================================================
CAR_SINGLETON_IMPL(CConnectionHelper)

CAR_INTERFACE_IMPL(CConnectionHelper, Singleton, IConnectionHelper)

ECode CConnectionHelper::CreateFailedConnection(
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Connection::CreateFailedConnection(disconnectCause, result);
}

ECode CConnectionHelper::CreateCanceledConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Connection::CreateCanceledConnection(result);
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos