
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAXHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAXHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CInfoAccessSyntaxHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CInfoAccessSyntaxHelper)
    , public Singleton
    , public IInfoAccessSyntaxHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte> * pEncoding,
        /* [out] */ Org::Apache::Harmony::Security::X509::IInfoAccessSyntax ** ppSyntax);

    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Type ** ppAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAXHELPER_H__
