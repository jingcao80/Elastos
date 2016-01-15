
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CRequestHandle.h"
#include "elastos/droid/net/http/CRequestHandleHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CRequestHandleHelper, Singleton, IRequestHandleHelper)

CAR_SINGLETON_IMPL(CRequestHandleHelper)

ECode CRequestHandleHelper::ComputeBasicAuthResponse(
    /* [in] */ const String& username,
    /* [in] */ const String& password,
    /* [out] */ String* result)
{
    return CRequestHandle::ComputeBasicAuthResponse(username, password, result);
}

ECode CRequestHandleHelper::AuthorizationHeader(
    /* [in] */ Boolean isProxy,
    /* [out] */ String* result)
{
    return CRequestHandle::AuthorizationHeader(isProxy, result);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
