
#ifndef __ELASTOS_SECURITY_CERT_CPOLICYQUALIFIERINFO_H__
#define __ELASTOS_SECURITY_CERT_CPOLICYQUALIFIERINFO_H__

#include "_Elastos_Security_Cert_CPolicyQualifierInfo.h"

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPolicyQualifierInfo)
    , public Object
    , public IPolicyQualifierInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> **encoded);

    CARAPI GetPolicyQualifierId(
        /* [out] */ String *id);

    CARAPI GetPolicyQualifier(
        /* [out, callee] */ ArrayOf<Byte> **qualifier);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> *encoded);

private:
    // This PolicyQualifierInfo DER encoding
    const AutoPtr<ArrayOf<Byte> > mEncoded;
    // This PolicyQualifierInfo policy qualifier id -
    // OID represented as String containing non-negative integers
    // separated by periods
    const String mPolicyQualifierId;
    // DER encoding of the policy qualifier - part of encoded
    const AutoPtr<ArrayOf<Byte> > mPolicyQualifier;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPOLICYQUALIFIERINFO_H__
