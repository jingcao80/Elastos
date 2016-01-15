
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1GENERALIZEDTIME_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1GENERALIZEDTIME_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1GeneralizedTime.h"
#include <ASN1Time.h>

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1GeneralizedTime), public ASN1Time
{
public:
    static AutoPtr<IASN1Type> Init();

    static CARAPI GetInstance(
    /* [out] */ IASN1GeneralizedTime** instance);

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

private:
    // default implementation
    static AutoPtr<IASN1Type> sASN1;

    // FIXME support only one format for encoding, do we need others?
    //
    // According to X.680:
    // four digit year, seconds always presented
    // and fractional-seconds elements without
    // trailing 0's (must be cut later from content)
    static String GEN_PATTERN;// = "yyyyMMddHHmmss.SSS";
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1GENERALIZEDTIME_H__
