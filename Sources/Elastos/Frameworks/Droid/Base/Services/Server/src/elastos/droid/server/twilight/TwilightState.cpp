
#include "elastos/droid/server/twilight/TwilightState.h"
#include <elastos/core/StringBuilder.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Core::StringBuilder;
using Elastos::Text::IDateFormat;
using Elastos::Text::IDateFormatHelper;
using Elastos::Text::CDateFormatHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Twilight {

TwilightState::TwilightState(
    /* [in] */ Boolean isNight,
    /* [in] */ Int64 yesterdaySunset,
    /* [in] */ Int64 todaySunrise,
    /* [in] */ Int64 todaySunset,
    /* [in] */ Int64 tomorrowSunrise)
    : mIsNight(isNight)
    , mYesterdaySunset(yesterdaySunset)
    , mTodaySunrise(todaySunrise)
    , mTodaySunset(todaySunset)
    , mTomorrowSunrise(tomorrowSunrise)
{
}

Boolean TwilightState::IsNight()
{
    return mIsNight;
}

Int64 TwilightState::GetYesterdaySunset()
{
    return mYesterdaySunset;
}

Int64 TwilightState::GetTodaySunrise()
{
    return mTodaySunrise;
}

Int64 TwilightState::GetTodaySunset()
{
    return mTodaySunset;
}

Int64 TwilightState::GetTomorrowSunrise()
{
    return mTomorrowSunrise;
}

ECode TwilightState::IsNight(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsNight;
    return NOERROR;
}

ECode TwilightState::GetYesterdaySunset(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mYesterdaySunset;
    return NOERROR;
}

ECode TwilightState::GetTodaySunrise(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTodaySunrise;
    return NOERROR;
}

ECode TwilightState::GetTodaySunset(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTodaySunset;
    return NOERROR;
}

ECode TwilightState::GetTomorrowSunrise(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTomorrowSunrise;
    return NOERROR;
}

ECode TwilightState::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    ITwilightState* obj = ITwilightState::Probe(other);
    if (obj == NULL) return NOERROR;

    TwilightState* o = (TwilightState*)obj;
    *result = Equals(o);
    return NOERROR;
}

Boolean TwilightState::Equals(
        /* [in] */ TwilightState* other)
{
    return other != NULL
            && mIsNight == other->IsNight()
            && mYesterdaySunset == other->GetYesterdaySunset()
            && mTodaySunrise == other->GetTodaySunrise()
            && mTodaySunset == other->GetTodaySunset()
            && mTomorrowSunrise == other->GetTomorrowSunrise();
}

Int32 TwilightState::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = 0; // don't care
    return 0;
}


ECode TwilightState::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDateFormatHelper> helper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
    AutoPtr<IDateFormat> f;
    helper->GetDateTimeInstance((IDateFormat**)&f);

    AutoPtr<IDate> ys, sunrise, sunset, ts;
    CDate::New(mYesterdaySunset, (IDate**)&ys);
    CDate::New(mTodaySunrise, (IDate**)&sunrise);
    CDate::New(mTodaySunset, (IDate**)&sunset);
    CDate::New(mTomorrowSunrise, (IDate**)&ts);
    String str;

    StringBuilder sb("{TwilightState: isNight=");
    sb += mIsNight;

    sb += ", mYesterdaySunset=";
    f->Format(ys, &str);
    sb += str;

    sb += ", mTodaySunrise=";
    f->Format(sunrise, &str);
    sb += str;

    sb += ", mTodaySunset=";
    f->Format(sunset, &str);
    sb += str;

    sb += ", mTomorrowSunrise=";
    f->Format(ts, &str);
    sb += str;

    *result = sb.ToString();
    return NOERROR;
}

} // namespace Twilight
} // namespace Server
} // namespace Droid
} // namespace Elastos
