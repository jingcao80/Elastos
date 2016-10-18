
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CINVALIDITYDATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CINVALIDITYDATE_H__

#include "_Org_Apache_Harmony_Security_X509_CInvalidityDate.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IDate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CInvalidityDate)
    , public ExtensionValue
    , public IInvalidityDate
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI GetDate(
        /* [out] */ IDate** ppDate);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

    /**
     * Constructs the object from a date instance.
     */
    CARAPI constructor(
        /* [in] */ IDate* date);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * ASN.1 Encoder/Decoder.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    /** invalidity date value */
    AutoPtr<IDate> mDate;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CINVALIDITYDATE_H__
