
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CHttpsConnectionHelper.h"
#include "elastos/droid/net/http/HttpsConnection.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CHttpsConnectionHelper, Singleton, IHttpsConnectionHelper)

CAR_SINGLETON_IMPL(CHttpsConnectionHelper)

ECode CHttpsConnectionHelper::InitializeEngine(
    /* [in] */ IFile* sessionDir)
{
    return HttpsConnection::InitializeEngine(sessionDir);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
