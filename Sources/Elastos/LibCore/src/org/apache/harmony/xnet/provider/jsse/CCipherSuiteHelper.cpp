
#include "CCipherSuiteHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CCipherSuiteHelper)

CAR_INTERFACE_IMPL(CCipherSuiteHelper, Singleton, ICipherSuiteHelper)

ECode CCipherSuiteHelper::GetByName(
    /* [in] */ const String& name,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite ** ppSuite)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCipherSuiteHelper::GetByCode(
    /* [in] */ Byte b1,
    /* [in] */ Byte b2,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite ** ppSuite)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCipherSuiteHelper::GetByCodeEx(
    /* [in] */ Byte b1,
    /* [in] */ Byte b2,
    /* [in] */ Byte b3,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite ** ppSuite)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCipherSuiteHelper::GetSupported(
    /* [out, callee] */ ArrayOf<Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite *> ** ppSupported)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCipherSuiteHelper::GetSupportedCipherSuiteNames(
    /* [out, callee] */ ArrayOf<String> ** ppNames)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCipherSuiteHelper::GetClientKeyType(
    /* [in] */ Byte keyType,
    /* [out] */ String * pType)
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

