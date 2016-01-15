
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTPUBLICKEYINFOHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTPUBLICKEYINFOHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CSubjectPublicKeyInfoHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CSubjectPublicKeyInfoHelper)
{
public:
    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTPUBLICKEYINFOHELPER_H__
