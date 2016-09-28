
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CX509PUBLICKEY_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CX509PUBLICKEY_H__

#include "_Org_Apache_Harmony_Security_X509_CX509PublicKey.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Security::IPublicKey;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CX509PublicKey)
    , public Object
    , public IX509PublicKey
    , public IPublicKey
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetAlgorithm(
        /* [out] */ String* pAlgorithm);

    CARAPI GetFormat(
        /* [out] */ String* pFormat);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI constructor(
        /* [in] */ const String& algorithm,
        /* [in] */ ArrayOf<Byte>* pEncoded,
        /* [in] */ ArrayOf<Byte>* pKeyBytes);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CX509PUBLICKEY_H__
