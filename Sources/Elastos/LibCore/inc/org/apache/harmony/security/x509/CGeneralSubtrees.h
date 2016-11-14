
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralSubtrees.h"
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

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralSubtrees)
    , public Object
    , public IGeneralSubtrees
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

        TO_STRING_IMPL("CGeneralSubtrees::MyASN1Sequence")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSubtrees(
        /* [out] */ IList** ppSubtrees);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI constructor(
        /* [in] */ IList* pGeneralSubtrees);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * ASN.1 DER X.509 GeneralSubtrees encoder/decoder class.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    /** the list of values of GeneralSubtrees */
    AutoPtr<IList> mGeneralSubtrees;

    /** the ASN.1 encoded form of GeneralSubtrees */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__
