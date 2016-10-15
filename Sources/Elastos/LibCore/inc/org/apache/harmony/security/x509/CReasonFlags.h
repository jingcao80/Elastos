
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CREASONFLAGS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CREASONFLAGS_H__

#include "_Org_Apache_Harmony_Security_X509_CReasonFlags.h"
#include "ASN1NamedBitList.h"

using Elastos::Core::IStringBuilder;
using Org::Apache::Harmony::Security::Asn1::ASN1NamedBitList;
using Org::Apache::Harmony::Security::Asn1::IASN1BitString;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IBerOutputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CReasonFlags)
    , public Object
    , public IReasonFlags
{
private:
    /**
     * ASN.1 Encoder/Decoder.
     */
    class ASN1NamedBitListWrapper: public ASN1NamedBitList
    {
    public:
        ASN1NamedBitListWrapper();

        CARAPI constructor(
            /* [in] */ Int32 minBits);

        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result) /*throws IOException*/;

        CARAPI SetEncodingContent(
            /* [in] */ IBerOutputStream* out);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ ArrayOf<Boolean>* pFlags);

private:
    static CARAPI_(AutoPtr<IASN1BitString>) InitASN1();

public:
    static AutoPtr<IASN1BitString> ASN1;

private:
    /**
     * The names of the reasons.
     */
    static const String REASONS[];

    /** the value of extension */
    AutoPtr<ArrayOf<Boolean> > mFlags;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CREASONFLAGS_H__
