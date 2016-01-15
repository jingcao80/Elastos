
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1BITSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1BITSTRING_H__

#include "ASN1StringType.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1BitString
    : public ASN1StringType
{
public:
    static CARAPI GetInstance(
    /* [out] */ IASN1Type** instance);

    static CARAPI_(AutoPtr<IASN1Type>) Init();

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI GetConstrId(
        /* [out] */ Int32* constrId);

    virtual CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [out] */ IInterface** object);

    virtual CARAPI DecodeEx(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 encodingLen,
        /* [out] */ IInterface** object);

    virtual CARAPI DecodeEx2(
        /* [in] */ IInputStream* is,
        /* [out] */ IInterface** object);

    virtual CARAPI Verify(
        /* [in] */ ArrayOf<Byte>* encoded);

    virtual CARAPI VerifyEx(
        /* [in] */ IInputStream* is);

    virtual CARAPI Encode(
        /* [in] */ IInterface* object,
        /* [out, callee] */ ArrayOf<Byte>** encode);

    virtual CARAPI DecodeEx3(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    virtual CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [out] */ Boolean* checkTag);

    virtual CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    virtual CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI GetEncodedLength(
        /* [in] */ IBerOutputStream* bos,
        /* [out] */ Int32* length);

    virtual CARAPI ToString(
        /* [out] */ String* result);

    CARAPI constructor();

private:
    // default implementation
    static AutoPtr<IASN1Type> sASN1; // = new ASN1BitString();
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1BITSTRING_H__
