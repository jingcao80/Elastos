
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1BITSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1BITSTRING_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1BitString.h"
#include "ASN1BitString.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1BitString), public ASN1BitString
{
public:
    CAR_OBJECT_DECL()

    static CARAPI GetInstance(
    /* [out] */ IASN1Type** instance);

    static CARAPI_(AutoPtr<IASN1Type>) Init();

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI GetConstrId(
        /* [out] */ Int32* constrId);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [out] */ IInterface** object);

    CARAPI DecodeEx(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 encodingLen,
        /* [out] */ IInterface** object);

    CARAPI DecodeEx2(
        /* [in] */ IInputStream* is,
        /* [out] */ IInterface** object);

    CARAPI Verify(
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI VerifyEx(
        /* [in] */ IInputStream* is);

    CARAPI Encode(
        /* [in] */ IInterface* object,
        /* [out, callee] */ ArrayOf<Byte>** encode);

    CARAPI DecodeEx3(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [out] */ Boolean* checkTag);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* bos);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI GetEncodedLength(
        /* [in] */ IBerOutputStream* bos,
        /* [out] */ Int32* length);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI constructor();
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1BITSTRING_H__
