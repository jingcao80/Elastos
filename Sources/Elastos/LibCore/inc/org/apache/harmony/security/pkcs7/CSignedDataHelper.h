
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNEDDATAHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNEDDATAHELPER_H__

#include "_Org_Apache_Harmony_Security_Pkcs7_CSignedDataHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CarClass(CSignedDataHelper)
    , public Singleton
    , public ISignedDataHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** asn1);
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNEDDATAHELPER_H__
