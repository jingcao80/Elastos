
#include "CASN1UTCTime.h"
#include <cmdef.h>

// FIXME support only one format for encoding, do we need others?
//
// According to X.680 coordinated universal time format:
// two digit year, seconds always presented,
// no fractional-seconds elements, 'Z' at the end
static const String UTC_PATTERN("yyMMddHHmmss'Z'");

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1UTCTime> CASN1UTCTime::ASN1 = InitStatic();

CAR_OBJECT_IMPL(CASN1UTCTime)

AutoPtr<IASN1UTCTime> CASN1UTCTime::InitStatic()
{
    return new CASN1UTCTime();
}

ECode CASN1UTCTime::GetInstance(
    /* [out] */ IASN1UTCTime** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = ASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1UTCTime::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Time::GetId(id);
}

ECode CASN1UTCTime::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Time::GetConstrId(constrId);
}

ECode CASN1UTCTime::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Time::Decode(encoded, object);
}

ECode CASN1UTCTime::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Time::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1UTCTime::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Time::DecodeEx2(is, object);
}

ECode CASN1UTCTime::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Time::Verify(encoded);
}

ECode CASN1UTCTime::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Time::VerifyEx(is);
}

ECode CASN1UTCTime::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Time::Encode(object, encode);
}

ECode CASN1UTCTime::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadUTCTime();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1UTCTime::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Time::CheckTag(identifier, checkTag);
}

ECode CASN1UTCTime::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1Time::GetDecodedObject(bis, object);
}

ECode CASN1UTCTime::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Time::EncodeASN(bos);
}

ECode CASN1UTCTime::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeUTCTime();
}

ECode CASN1UTCTime::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(UTC_PATTERN, (ISimpleDateFormat**)&sdf);
    AutoPtr<ITimeZoneHelper> tzh;
    AutoPtr<ITimeZone> tz;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    tzh->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    sdf->SetTimeZone(tz);
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    String temp;
    sdf->FormatDate(IDate::Probe(content), &temp);
    AutoPtr<ArrayOf<Byte> > ct;
    temp.GetBytes((ArrayOf<Byte>**)&ct);
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, ct->GetLength(), (IArrayOf**)&arr);
    for (Int32 i = 0; i < ct->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*ct)[i], (IByte**)&bt);
        arr->Put(i, bt.Get());
    }
    bos->SetContent(ct.Get());
    bos->SetLength(ct->GetLength());
}

ECode CASN1UTCTime::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Time::GetEncodedLength(bos, length);
}

ECode CASN1UTCTime::ToString(
    /* [out] */ String* result)
{
    return ASN1Time::ToString(result);
}

ECode CASN1UTCTime::constructor()
{
    return ASN1Time::Init(IASN1Constants::TAG_UTCTIME);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

