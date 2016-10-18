
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralSubtree.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralSubtree)
    , public Object
    , public IGeneralSubtree
{
private:
    class MyASN1Sequence
        : public ASN1Sequence
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetBase(
        /* [out] */ IGeneralName** ppBase);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ IGeneralName* pBase,
        /* [in] */ Int32 minimum,
        /* [in] */ Int32 maximum);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    /**
     * ASN.1 DER X.509 GeneralSubtree encoder/decoder class.
     */
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of base field of the structure */
    AutoPtr<IGeneralName> mBase;

    /** the value of minimum field of the structure */
    Int32 mMinimum;

    /** the value of maximum field of the structure */
    Int32 mMaximum;

    /** the ASN.1 encoded form of GeneralSubtree */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__
