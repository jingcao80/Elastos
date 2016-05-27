
#include "CClientSessionContext.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CClientSessionContext)

CAR_INTERFACE_IMPL(CClientSessionContext, Object, IClientSessionContext)

ECode CClientSessionContext::GetIds(
    /* [out] */ Elastos::Utility::IEnumeration ** ppIds)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::GetSession(
    /* [in] */ ArrayOf<Byte> * pSessionId,
    /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppSession)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::GetSessionCacheSize(
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::GetSessionTimeout(
    /* [out] */ Int32 * pTimeout)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::SetSessionCacheSize(
    /* [in] */ Int32 size)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::SetSessionTimeout(
    /* [in] */ Int32 seconds)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::GetSize(
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::SetPersistentCache(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLClientSessionCache * pPersistentCache)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::GetSessionEx(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppSession)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientSessionContext::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

