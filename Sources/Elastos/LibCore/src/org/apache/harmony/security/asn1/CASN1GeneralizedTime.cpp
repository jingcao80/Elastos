
#include "CASN1GeneralizedTime.h"
#include "CBerInputStream.h"
#include "CoreUtils.h"
#include "TimeZone.h"
#include "elastos/utility/Locale.h"
#include "CSimpleDateFormat.h"

using Elastos::Core::CoreUtils;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::Locale;
using Elastos::Utility::TimeZone;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Type> CASN1GeneralizedTime::sASN1 = Init();

String CASN1GeneralizedTime::GEN_PATTERN("yyyyMMddHHmmss.SSS");

CAR_OBJECT_IMPL(CASN1GeneralizedTime)
CAR_INTERFACE_IMPL(CASN1GeneralizedTime, ASN1Time, IASN1GeneralizedTime)
AutoPtr<IASN1Type> CASN1GeneralizedTime::Init()
{
    return new CASN1GeneralizedTime();
}

ECode CASN1GeneralizedTime::GetInstance(
    /* [out] */ IASN1GeneralizedTime** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = IASN1GeneralizedTime::Probe(sASN1);
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1GeneralizedTime::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadGeneralizedTime();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
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
    CSimpleDateFormat::New(GEN_PATTERN, Locale::US, (ISimpleDateFormat**)&sdf);
    AutoPtr<ITimeZone> tz;
    TimeZone::GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    IDateFormat::Probe(sdf)->SetTimeZone(tz);
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    String temp;
    IDateFormat::Probe(sdf)->Format(IDate::Probe(content), &temp);

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
    AutoPtr<ArrayOf<Byte> > ct = temp.GetBytes();
    ((CBerInputStream*)bos)->mContent = CoreUtils::ConvertByteArray(ct);
    return bos->SetLength(ct->GetLength());
}

ECode CASN1GeneralizedTime::constructor()
{
    return ASN1Time::constructor(IASN1Constants::TAG_GENERALIZEDTIME);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

