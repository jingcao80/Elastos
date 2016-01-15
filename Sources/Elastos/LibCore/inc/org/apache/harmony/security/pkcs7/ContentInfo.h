
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFO_H__

#include "core/Object.h"
#include "asn1/ASN1Sequence.h"

using Elastos::Core::Object;
using Elastos::Core::IArrayOf;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

class ContentInfo
    : public Object
    , public IContentInfo
{
private:
    class ASN1SequenceDerived
        : public ASN1Sequence
    {
    public:
        ASN1SequenceDerived(
            /* [in] */ ArrayOf<IASN1Type*>* type);

    protected:
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI GetSignedData(
        /* [out] */ ISignedData** signedData);

    CARAPI GetContent(
        /* [out] */ IInterface** content);

    CARAPI GetContentType(
        /* [out, callee] */ ArrayOf<Int32>** contentType);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    ContentInfo(
        /* [in] */ ArrayOf<Int32>* oid,
        /* [in] */ IInterface* content,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitStatic();

public:
    // OIDs
    static AutoPtr< ArrayOf<Int32> > DATA;
    static AutoPtr< ArrayOf<Int32> > SIGNED_DATA;
    static AutoPtr< ArrayOf<Int32> > ENVELOPED_DATA;
    static AutoPtr< ArrayOf<Int32> > SIGNED_AND_ENVELOPED_DATA;
    static AutoPtr< ArrayOf<Int32> > DIGESTED_DATA;
    static AutoPtr< ArrayOf<Int32> > ENCRYPTED_DATA;

    AutoPtr< IArrayOf > mOid;
    AutoPtr<IInterface> mContent;
    AutoPtr< ArrayOf<Byte> > mEncoding;

    static AutoPtr<IASN1Sequence> ASN1;
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFO_H__
