
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CREASONCODE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CREASONCODE_H__

#include "_Org_Apache_Harmony_Security_X509_CReasonCode.h"
#include "ExtensionValue.h"

using Elastos::Core::IStringBuilder;
using Elastos::Security::Cert::CRLReason;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CReasonCode)
    , public ExtensionValue
    , public IReasonCode
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CReasonCode();

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

    CARAPI_(CRLReason) GetReason();

public:
    /**
     * ASN.1 Encoder/Decoder.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    // the reason code value
    Byte mCode;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CREASONCODE_H__
