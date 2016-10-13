
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS8_CPRIVATEKEYINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS8_CPRIVATEKEYINFO_H__

#include "_Org_Apache_Harmony_Security_Pkcs8_CPrivateKeyInfo.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "ASN1Sequence.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs8 {

CarClass(CPrivateKeyInfo)
    , public Object
    , public IPrivateKeyInfo
{
private:
    class ASN1SequenceDerived
        : public ASN1Sequence
    {
    public:
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type *>* type);

        ASN1SequenceDerived();
    };

public:
    static AutoPtr<IASN1Sequence> InitStatic();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPrivateKeyInfo();

    CARAPI GetVersion(
        /* [out] */ Int32* ver);

    CARAPI GetAlgorithmIdentifier(
        /* [out] */ IAlgorithmIdentifier** identifier);

    CARAPI GetAttributes(
        /* [out] */ IList** attributes);

    CARAPI GetPrivateKey(
        /* [out, callee] */ ArrayOf<Byte>** privateKey);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI constructor(
        /* [in] */ Int32 version,
        /* [in] */ IAlgorithmIdentifier* privateKeyAlgorithm,
        /* [in] */ ArrayOf<Byte>* privateKey,
        /* [in] */ IList* attributes);

    CARAPI constructor(
        /* [in] */ Int32 version,
        /* [in] */ IAlgorithmIdentifier* privateKeyAlgorithm,
        /* [in] */ ArrayOf<Byte>* privateKey,
        /* [in] */ IList* attributes,
        /* [in] */ ArrayOf<Byte>* encoding);

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    Int32 mVersion;
    AutoPtr<IAlgorithmIdentifier> mPrivateKeyAlgorithm;
    AutoPtr<ArrayOf<Byte> > mPrivateKey;
    AutoPtr<IList> mAttributes;
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} // namespace Pkcs8
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS8_CPRIVATEKEYINFO_H__
