
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMES_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMES_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralNames.h"
#include "org/apache/harmony/security/asn1/ASN1SequenceOf.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralNames)
    , public Object
    , public IGeneralNames
{
private:
    class MyASN1SequenceOf
        : public ASN1SequenceOf
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [out] */ ICollection** values);

        TO_STRING_IMPL("CGeneralNames::MyASN1SequenceOf")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNames(
        /* [out] */ IList** ppNames);

    CARAPI GetPairsList(
        /* [out] */ ICollection** ppPairslist);

    CARAPI AddName(
        /* [in] */ IGeneralName* pName);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* pGeneralNames);

    CARAPI constructor(
        /* [in] */ IList* generalNames,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * ASN.1 DER X.509 GeneralNames encoder/decoder class.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    /** the values of GeneralName */
    AutoPtr<IList> mGeneralNames;

    /** the ASN.1 encoded form of GeneralNames */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMES_H__
