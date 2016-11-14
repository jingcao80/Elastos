
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_COTHERNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_COTHERNAME_H__

#include "_Org_Apache_Harmony_Security_X509_COtherName.h"
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

CarClass(COtherName)
    , public Object
    , public IOtherName
{
private:
    class MyASN1Sequence
        : public ASN1Sequence
    {
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("COtherName::MyASN1Sequence")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetValue(
        /* [out, callee] */ ArrayOf<Byte>** ppValue);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    COtherName();

    CARAPI constructor(
        /* [in] */ const String& typeID,
        /* [in] */ ArrayOf<Byte>* pValue);

    CARAPI constructor(
        /* [in] */ const String& typeID,
        /* [in] */ ArrayOf<Byte>* value,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* ppAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    /**
     * ASN.1 DER X.509 OtherName encoder/decoder class.
     */
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of typeID field of the structure */
    String mTypeID;

    /** the value of value field of the structure */
    AutoPtr<ArrayOf<Byte> > mValue;

    /** the ASN.1 encoded form of OtherName */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_COTHERNAME_H__
