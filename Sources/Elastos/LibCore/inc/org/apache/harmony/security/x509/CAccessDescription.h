
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CACCESSDESCRIPTION_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CACCESSDESCRIPTION_H__

#include "_Org_Apache_Harmony_Security_X509_CAccessDescription.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CAccessDescription)
    , public Object
    , public IAccessDescription
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
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Returns ASN.1 encoded form of this X.509 AccessDescription.
     */
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI ToString(
        /* [out] */ String* pStr);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    CARAPI constructor(
        /* [in] */ const String& accessMethod,
        /* [in] */ IGeneralName* accessLocation,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    /**
     * Custom AccessDescription DER encoder/decoder
     */
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of access method */
    String mAccessMethod;

    /** the value of accessLocation */
    AutoPtr<IGeneralName> mAccessLocation;

    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CACCESSDESCRIPTION_H__
