
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_ALGORITHMIDENTIFIER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_ALGORITHMIDENTIFIER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

class AlgorithmIdentifier
    : public Object
    , public IAlgorithmIdentifier
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetAlgorithm(
        /* [out] */ String* pAlgorithm);

    CARAPI GetAlgorithmName(
        /* [out] */ String* pAlgorithmName);

    CARAPI GetParameters(
        /* [out, callee] */ ArrayOf<Byte>** ppParameters);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface* pAi,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pResult);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI constructor(
        /* [in] */ const String& algorithm);

    CARAPI constructor(
        /* [in] */ const String& algorithm,
        /* [in] */ ArrayOf<Byte>* pParameters);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_ALGORITHMIDENTIFIER_H__
