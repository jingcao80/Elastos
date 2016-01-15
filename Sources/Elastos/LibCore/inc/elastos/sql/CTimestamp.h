
#ifndef __ELASTOS_SQL_CTIMESTAMP_H__
#define __ELASTOS_SQL_CTIMESTAMP_H__

#include "_Elastos_Sql_CTimestamp.h"
#include "Date.h"

using Elastos::Utility::Date;

namespace Elastos {
namespace Sql {

CarClass(CTimestamp)
    , public Date
    , public ITimestamp
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CTimestamp();

    CARAPI constructor(
        /* [in] */ Int32 theYear,
        /* [in] */ Int32 theMonth,
        /* [in] */ Int32 theDate,
        /* [in] */ Int32 theHour,
        /* [in] */ Int32 theMinute,
        /* [in] */ Int32 theSecond,
        /* [in] */ Int32 theNano);

    CARAPI constructor(
        /* [in] */ Int64 theTime);

    CARAPI CompareTo(
        /* [in] */ IInterface* date,
        /* [out] */ Int32* result);

    CARAPI GetTime(
        /* [out] */ Int64* time);

    CARAPI SetTime(
        /* [in] */ Int64 milliseconds);

    CARAPI ToString(
        /* [out] */ String* localeStr);

    CARAPI After(
        /* [in] */ ITimestamp * theTimestamp,
        /* [out] */ Boolean * value);

    CARAPI Before(
        /* [in] */ ITimestamp * theTimestamp,
        /* [out] */ Boolean * value);

    CARAPI CompareTo(
        /* [in] */ ITimestamp * theTimestamp,
        /* [out] */ Int32 * value);

    CARAPI Equals(
        /* [in] */ IInterface * pTheObject,
        /* [out] */ Boolean * value);

    CARAPI Equals(
        /* [in] */ ITimestamp * theTimestamp,
        /* [out] */ Boolean * value);

    CARAPI GetNanos(
        /* [out] */ Int32 * pNano);

    CARAPI SetNanos(
        /* [in] */ Int32 n);

public:
    static CARAPI ValueOf(
        /* [in] */ const String& str,
        /* [out] */ ITimestamp** ts);

private:
    CARAPI_(void) Format(
        /* [in] */ Int32 date,
        /* [in] */ Int32 digits,
        /* [in] */ StringBuilder* sb);

    CARAPI SetTimeImpl(
        /* [in] */ Int64 theTime);

private:
    static CARAPI BadTimestampString(
        /* [in] */ const String& s);

private:
    Int32 mNanos;

    const static String TIME_FORMAT_REGEX;

    const static String PADDING;
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CTIMESTAMP_H__
