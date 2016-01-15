
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMEHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralNameHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralNameHelper)
{
public:
    CARAPI CheckDNS(
        /* [in] */ const String& dns);

    CARAPI CheckURI(
        /* [in] */ const String& uri);

    CARAPI OidStrToInts(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Int32> ** ppInts);

    CARAPI IpStrToBytes(
        /* [in] */ const String& ip,
        /* [out, callee] */ ArrayOf<Byte> ** ppBytes);

    CARAPI IpBytesToStr(
        /* [in] */ ArrayOf<Byte> * pIp,
        /* [out] */ String * pStr);

    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Choice ** ppAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMEHELPER_H__
