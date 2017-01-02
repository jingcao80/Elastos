//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CASN1UTCTime.h"
#include "CBerInputStream.h"
#include "CArrayOf.h"
#include "CByte.h"
#include "CoreUtils.h"
#include "CSimpleDateFormat.h"
#include "TimeZone.h"
#include "elastos/utility/Locale.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IByte;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::Locale;
using Elastos::Utility::TimeZone;

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

INIT_PROI_3 AutoPtr<IASN1UTCTime> CASN1UTCTime::ASN1 = InitStatic();

CAR_OBJECT_IMPL(CASN1UTCTime)
CAR_INTERFACE_IMPL(CASN1UTCTime, ASN1Time, IASN1UTCTime)
AutoPtr<IASN1UTCTime> CASN1UTCTime::InitStatic()
{
    AutoPtr<IASN1UTCTime> at;
    CASN1UTCTime::New((IASN1UTCTime**)&at);
    return at;
}

ECode CASN1UTCTime::GetInstance(
    /* [out] */ IASN1UTCTime** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = ASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1UTCTime::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadUTCTime();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
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
    CSimpleDateFormat::New(UTC_PATTERN, Locale::US, (ISimpleDateFormat**)&sdf);
    AutoPtr<ITimeZone> tz;
    TimeZone::GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    IDateFormat::Probe(sdf)->SetTimeZone(tz);
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    String temp;
    IDateFormat::Probe(sdf)->Format(IDate::Probe(content), &temp);
    AutoPtr<ArrayOf<Byte> > ct = temp.GetBytes();
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, ct->GetLength(), (IArrayOf**)&arr);
    for (Int32 i = 0; i < ct->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*ct)[i], (IByte**)&bt);
        arr->Set(i, bt.Get());
    }
    bos->SetContent(CoreUtils::ConvertByteArray(ct));
    bos->SetLength(ct->GetLength());
    return NOERROR;
}

ECode CASN1UTCTime::constructor()
{
    return ASN1Time::constructor(IASN1Constants::TAG_UTCTIME);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

