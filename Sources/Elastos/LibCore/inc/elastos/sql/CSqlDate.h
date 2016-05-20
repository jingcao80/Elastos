
#ifndef __ELASTOS_SQL_CSQL_CDATE_H__
#define __ELASTOS_SQL_CSQL_CDATE_H__

#include "_Elastos_Sql_CSqlDate.h"
#include "Date.h"

using Elastos::Utility::Date;

namespace Elastos {
namespace Sql {

CarClass(CSqlDate)
    , public Date
    , public ISQLDate
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int64 theDate);

    CARAPI constructor(
        /* [in] */ Int32 theYear,
        /* [in] */ Int32 theMonth,
        /* [in] */ Int32 theDay);

    CARAPI GetHours(
        /* [out] */ Int32 * value);

    CARAPI GetMinutes(
        /* [out] */ Int32 * value);

    CARAPI GetSeconds(
        /* [out] */ Int32 * value);

    CARAPI SetHours(
        /* [in] */ Int32 hour);

    CARAPI SetMinutes(
        /* [in] */ Int32 minute);

    CARAPI SetSeconds(
        /* [in] */ Int32 second);

    CARAPI SetTime(
        /* [in] */ Int64 milliseconds);

    CARAPI ToString(
        /* [out] */ String * str);

public:
    static CARAPI ValueOf(
        /* [in] */ const String& dateString,
        /* [out] */ ISQLDate** date);

private:
    CARAPI_(void) Format(
        /* [in] */ Int32 date,
        /* [in] */ Int32 digits,
        /* [in] */ StringBuilder * sb);

private:
    /*
     * Private method which normalizes a Time value, removing all low
     * significance digits corresponding to milliseconds, seconds, minutes and
     * hours, so that the returned Time value corresponds to 00:00:00 GMT on a
     * particular day.
     */
    static CARAPI_(Int64) NormalizeTime(
        /* [in] */ Int64 theTime);

private:
    const static String PADDING;
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CSQL_CDATE_H__
