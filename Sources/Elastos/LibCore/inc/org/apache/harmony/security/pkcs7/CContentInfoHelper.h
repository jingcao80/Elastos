
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFOHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFOHELPER_H__

#include "_Org_Apache_Harmony_Security_Pkcs7_CContentInfoHelper.h"
#include "core/Singleton.h"

using Elastos::Core::Singleton;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CarClass(CContentInfoHelper)
    , public Singleton
    , public IContentInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetData(
        /* [out, callee] */ ArrayOf<Int32>** data);

    CARAPI GetSignedData(
        /* [out, callee] */ ArrayOf<Int32>** signedData);

    CARAPI GetEnvelopedData(
        /* [out, callee] */ ArrayOf<Int32>** envelopedData);

    CARAPI GetSignedAndEnvelopedData(
        /* [out, callee] */ ArrayOf<Int32>** signedAndEnvelopedData);

    CARAPI GetDigestedData(
        /* [out, callee] */ ArrayOf<Int32>** digestedData);

    CARAPI GetEncryptedData(
        /* [out, callee] */ ArrayOf<Int32>** encryptedData);

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** asn1);
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFOHELPER_H__
