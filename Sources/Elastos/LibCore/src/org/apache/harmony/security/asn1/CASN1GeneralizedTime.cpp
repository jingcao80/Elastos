
#include "CASN1GeneralizedTime.h"
#include <cmdef.h>

using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Type> CASN1GeneralizedTime::sASN1 = Init();

String CASN1GeneralizedTime::GEN_PATTERN("yyyyMMddHHmmss.SSS");

AutoPtr<IASN1Type> CASN1GeneralizedTime::Init()
{
    return new CASN1GeneralizedTime();
}

ECode CASN1GeneralizedTime::GetInstance(
    /* [out] */ IASN1GeneralizedTime** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1GeneralizedTime::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Time::GetId(id);
}

ECode CASN1GeneralizedTime::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Time::GetConstrId(constrId);
}

ECode CASN1GeneralizedTime::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Time::Decode(encoded, object);
}

ECode CASN1GeneralizedTime::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Time::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1GeneralizedTime::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Time::DecodeEx2(is, object);
}

ECode CASN1GeneralizedTime::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Time::Verify(encoded);
}

ECode CASN1GeneralizedTime::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Time::VerifyEx(is);
}

ECode CASN1GeneralizedTime::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Time::Encode(object, encode);
}

ECode CASN1GeneralizedTime::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadGeneralizedTime();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis);
}

ECode CASN1GeneralizedTime::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Time::CheckTag(identifier, checkTag);
}

ECode CASN1GeneralizedTime::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1Time::GetDecodedObject(bis, object);
}

ECode CASN1GeneralizedTime::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Time::EncodeASN(bos);
}

ECode CASN1GeneralizedTime::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeGeneralizedTime();
}

ECode CASN1GeneralizedTime::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(GEN_PATTERN, (ISimpleDateFormat**)&sdf);
    AutoPtr<ITimeZoneHelper> tzh;
    AutoPtr<ITimeZone> tz;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    tzh->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    sdf->SetTimeZone(tz);
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    String temp;
    sdf->FormatDate(IDate::Probe(content), &temp);

    // cut off trailing 0s
    Int32 nullId;
    Int32 currLength;
    while (((nullId = temp.LastIndexOf('0', currLength = temp.GetLength() - 1)) != -1)
            && (nullId == currLength)) {
        temp = temp.Substring(0, nullId);
    }
    // deal with point (cut off if it is last char)
    if (temp.GetChar(currLength) == '.') {
        temp = temp.Substring(0, currLength);
    }

    temp += "Z";
    AutoPtr<ArrayOf<Byte> > ct;
    temp.GetBytes((ArrayOf<Byte>**)&ct);
    bos->SetContent(ct);
    return bos->SetLength(ct->GetLength());
}

ECode CASN1GeneralizedTime::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Time::GetEncodedLength(bos, length);
}

ECode CASN1GeneralizedTime::ToString(
    /* [out] */ String* result)
{
    return ASN1Time::ToString(result);
}

ECode CASN1GeneralizedTime::constructor()
{
    return ASN1Time::Init(IASN1Constants::TAG_GENERALIZEDTIME);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

