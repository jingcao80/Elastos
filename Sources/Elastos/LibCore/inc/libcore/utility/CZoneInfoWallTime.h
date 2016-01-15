#ifndef __LIBCORE_UTILITY_CZONEINFOWALLTIME_H__
#define __LIBCORE_UTILITY_CZONEINFOWALLTIME_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"
#include "_Libcore_Utility_CZoneInfoWallTime.h"

using Elastos::Core::IInteger32;
using Elastos::Utility::IGregorianCalendar;

namespace Libcore {
namespace Utility {

CarClass(CZoneInfoWallTime)
    , public Object
    , public IZoneInfoWallTime
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CZoneInfoWallTime();

    CARAPI Localtime(
        /* [in] */ Int32 timeSeconds,
        /* [in] */ IZoneInfo* zoneInfo);

    CARAPI Mktime(
        /* [in] */ IZoneInfo* zoneInfo,
        /* [out] */ Int32* rst);

    CARAPI SetYear(
        /* [in] */ Int32 year);

    CARAPI SetMonth(
        /* [in] */ Int32 month);

    CARAPI SetMonthDay(
        /* [in] */ Int32 monthDay);

    CARAPI SetHour(
        /* [in] */ Int32 hour);

    CARAPI SetMinute(
        /* [in] */ Int32 minute);

    CARAPI SetSecond(
        /* [in] */ Int32 second);

    CARAPI SetWeekDay(
        /* [in] */ Int32 weekDay);

    CARAPI SetYearDay(
        /* [in] */ Int32 yearDay);

    CARAPI SetIsDst(
        /* [in] */ Int32 isDst);

    CARAPI SetGmtOffset(
        /* [in] */ Int32 gmtoff);

    CARAPI GetYear(
        /* [out] */ Int32* year);

    CARAPI GetMonth(
        /* [out] */ Int32* month);

    CARAPI GetMonthDay(
        /* [out] */ Int32* monthDay);

    CARAPI GetHour(
        /* [out] */ Int32* hour);

    CARAPI GetMinute(
        /* [out] */ Int32* minute);

    CARAPI GetSecond(
        /* [out] */ Int32* second);

    CARAPI GetWeekDay(
        /* [out] */ Int32* weekDay);

    CARAPI GetYearDay(
        /* [out] */ Int32* yearDay);

    CARAPI GetIsDst(
        /* [out] */ Int32* isDst);

    CARAPI GetGmtOffset(
        /* [out] */ Int32* gmtoff);

private:
    CARAPI TryOffsetAdjustments(
        /* [in] */ IZoneInfo* zoneInfo,
        /* [in] */ Int32 oldWallTimeSeconds,
        /* [in] */ IOffsetInterval* targetInterval,
        /* [in] */ Int32 transitionIndex,
        /* [in] */ Int32 isDstToFind,
        /* [out] */ IInteger32** result);

    CARAPI DoWallTimeSearch(
        /* [in] */ IZoneInfo* zoneInfo,
        /* [in] */ Int32 initialTransitionIndex,
        /* [in] */ Int32 wallTimeSeconds,
        /* [in] */ Boolean mustMatchDst,
        /* [out] */ IInteger32** result);

    CARAPI CopyFieldsToCalendar();

    CARAPI CopyFieldsFromCalendar();

    static CARAPI GetOffsetsOfType(
        /* [in] */ IZoneInfo* zoneInfo,
        /* [in] */ Int32 startIndex,
        /* [in] */ Int32 isDst,
        /* [out] */ ArrayOf<Int32>** result);

    static CARAPI FindTransitionIndex(
        /* [in] */ IZoneInfo* timeZone,
        /* [in] */ Int32 timeSeconds,
        /* [out] */ Int32* result);

private:
    AutoPtr<IGregorianCalendar> mCalendar;

    Int32 mYear;
    Int32 mMonth;
    Int32 mMonthDay;
    Int32 mHour;
    Int32 mMinute;
    Int32 mSecond;
    Int32 mWeekDay;
    Int32 mYearDay;
    Int32 mIsDst;
    Int32 mGmtOffsetSeconds;
};

} // namespace Libcore
} // namespace Utility

#endif