
#include "CGeneralNameHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CGeneralNameHelper)

CAR_INTERFACE_IMPL(CGeneralNameHelper, Singleton, IGeneralNameHelper)

ECode CGeneralNameHelper::CheckDNS(
    /* [in] */ const String& dns)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNameHelper::CheckURI(
    /* [in] */ const String& uri)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNameHelper::OidStrToInts(
    /* [in] */ const String& oid,
    /* [out, callee] */ ArrayOf<Int32> ** ppInts)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNameHelper::IpStrToBytes(
    /* [in] */ const String& ip,
    /* [out, callee] */ ArrayOf<Byte> ** ppBytes)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNameHelper::IpBytesToStr(
    /* [in] */ ArrayOf<Byte> * pIp,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNameHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Choice ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

