
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1IMPLICIT_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1IMPLICIT_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Implicit.h"
#include "ASN1Type.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Implicit)
    , public ASN1Type
    , public IASN1Implicit
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [out] */ Boolean* checkTag);

    CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* bos);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* out);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* out);

    CARAPI constructor(
        /* [in] */ Int32 tagNumber,
        /* [in] */ IASN1Type* type);

private:
    /** primitive type of tagging */
    static const Int32 TAGGING_PRIMITIVE;

    /** constructed type of tagging */
    static const Int32 TAGGING_CONSTRUCTED;

    /** string type of tagging */
    static const Int32 TAGGING_STRING;

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
