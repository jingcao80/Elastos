
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1IMPLICIT_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1IMPLICIT_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Implicit.h"
#include <ASN1Type.h>

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Implicit), public ASN1Type
{
public:
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

    CARAPI constructor(
        /* [in] */ Int32 tagNumber,
        /* [in] */ IASN1Type* type);

private:
    /** primitive type of tagging */
    static const Int32 TAGGING_PRIMITIVE = 0;

    /** constructed type of tagging */
    static const Int32 TAGGING_CONSTRUCTED = 1;

    /** string type of tagging */
    static const TAGGING_STRING = 2;

    /** tagged ASN.1 type */
    AutoPtr<IASN1Type> mType;

    /**
     * type of tagging. There are three of them
     * 1) primitive: only primitive identifier is valid
     * 2) constructed: only constructed identifier is valid
     * 3) string: both identifiers are valid
     */
    Int32 mTaggingType;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1IMPLICIT_H__
