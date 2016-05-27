
#include "CProtocolVersionHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CProtocolVersionHelper)

CAR_INTERFACE_IMPL(CProtocolVersionHelper, Singleton, IProtocolVersionHelper)

ECode CProtocolVersionHelper::GetSupportedProtocols(
    /* [out, callee] */ ArrayOf<String> ** ppSupportedProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersionHelper::IsSupported(
    /* [in] */ ArrayOf<Byte> * pVer,
    /* [out] */ Boolean * pIsSupported)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersionHelper::GetByVersion(
    /* [in] */ ArrayOf<Byte> * pVer,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppVs)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersionHelper::IsSupportedEx(
    /* [in] */ const String& name,
    /* [out] */ Boolean * pIsSupported)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersionHelper::GetByName(
    /* [in] */ const String& name,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersionHelper::GetLatestVersion(
    /* [in] */ ArrayOf<String> * pProtocols,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersionHelper::GetSSLv3(
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppSslv3)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersionHelper::GetTLSv1(
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IProtocolVersion ** ppTlsv1)
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

