//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <eltypes.h>
#include <_pubcrt.h>
#define _NO_INCLIST
#include <elasys_server.h>

#ifdef  __cplusplus
extern "C" {
#endif

_ELASTOS Int32 __cdecl _String_GetLength(const char *string,
    _ELASTOS Int32 nMaxLen);

_ELASTOS_NAMESPACE_USING

typedef enum _DateTimeFlag
{
    DateTimeFlag_InVaild        ,
    DateTimeFlag_Year           ,
    DateTimeFlag_ShortYear      ,
    DateTimeFlag_Month          ,
    DateTimeFlag_Day            ,
    DateTimeFlag_Hour           ,
    DateTimeFlag_THour          ,
    DateTimeFlag_Minute         ,
    DateTimeFlag_Second         ,
    DateTimeFlag_Week           ,
    DateTimeFlag_AM             ,
    DateTimeFlag_PM             ,

} DateTimeFlag;

#define IS_LEAP_YEAR(x)   ((x)% 400 == 0 || (!((x) & 3) && (x) % 100 != 0))

Boolean __cdecl _DateTime_IsValid(PDateTime pDateTime)
{
    if (pDateTime->month < 1 || pDateTime->month > 13
        || pDateTime->day < 1 || pDateTime->day > 31
        || pDateTime->hour > 23 || pDateTime->minute > 59
        || pDateTime->second > 59 || pDateTime->dayofweek > 6) {
        return FALSE;
    }

    if ((pDateTime->year % 100 != 0 && pDateTime->year % 4 == 0)
        || pDateTime->year % 400 == 0) {
        if (pDateTime->day > 29 && pDateTime->month == 2) {
            return FALSE;
        }
    }
    else if (pDateTime->day > 28 && pDateTime->month == 2) {
        return FALSE;
    }

    if (pDateTime->day > 30 && (pDateTime->month == 4 || pDateTime->month == 6
            || pDateTime->month == 9 || pDateTime->month == 11)) {
        return FALSE;
    }

    //dayofweek
    int year = pDateTime->year;
    int month = pDateTime->month;

    if (month <= 2) {
        month += 12;
        year -= 1;
    }

    if (pDateTime->dayofweek != ((pDateTime->day + 2 * month
        + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1)
        % 7)) {
        return FALSE;
    }

    return TRUE;
}

ECode __cdecl _Timestamp_ToUniversalDateTime(
        /* [in] */ Int64 timestamp,
        /* [out] */ PDateTime pDateTime)
{
    if (!pDateTime) {
        return E_INVALID_ARGUMENT;
    }

    if ((UInt64)timestamp > (0x3E7FFFFFFFFull)) { //0xFFFFFFFF * 1000 + 999
        return E_INVALID_ARGUMENT;
    }

    UInt8 s_DaysInMonth1[] = {0,31,29,31,30,31,30,31,31,30,31,30,31};
    UInt8 s_DaysInMonth0[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    UInt32 tm = (UInt32)(timestamp / 1000);//unit is second
    pDateTime->second    = (Byte) (tm % 60); tm /= 60;
    pDateTime->minute    = (Byte) (tm % 60); tm /= 60;
    pDateTime->hour      = (Byte) (tm % 24); tm /= 24;   // tm becomes day
    pDateTime->dayofweek = (Byte) ((tm + 4) % 7);        // it's Thursday on 1/1/1970

    // count from 1970. year 2100 will be count as a leap year falsely
    // due to simplify of IS_LEAP_YEAR algorithm.
    //
    UInt32 year = 70;
    while (1) {
        if (IS_LEAP_YEAR(year + 300)) {
            if (tm < 366) break; else tm -= 366;
        }
        else {
            if (tm < 365) break; else tm -= 365;
        }
        year++;
    }
    UInt32 month = 1;
    UInt8* DaysInMonth
        = IS_LEAP_YEAR(year + 300) ? s_DaysInMonth1 : s_DaysInMonth0;
    while (1) {
        if (tm < DaysInMonth[month]) break;
        else tm -= DaysInMonth[month];
        month++;
    }
    pDateTime->year  = year + 1900;
    pDateTime->month = month;
    pDateTime->day   = (Byte)++tm;
    return NOERROR;
}

ECode __cdecl _Timestamp_FromUniversalDateTime(
        /* [in] */ PDateTime pDateTime,
        /* [out] */ PInt64 pTimestamp)
{
    if (!pTimestamp || !pDateTime) {
        return E_INVALID_ARGUMENT;
    }

    if (!_DateTime_IsValid(pDateTime)) {
        return E_INVALID_ARGUMENT;
    }

    int year = pDateTime->year;
    int month = pDateTime->month;

    if ((month -= 2) <= 0) {  // 1..12 -> 11,12,1..10
        month += 12;          // Puts Feb last since it has leap day
        year -= 1;
    }

    Int64 seconds = (((
        (UInt32)(year / 4 - year / 100 + year / 400 + 367 * month / 12
        + pDateTime->day) + year * 365 - 719499
        ) * 24 + pDateTime->hour                  // now have hours
      ) * 60 + pDateTime->minute                  // now have minutes
    ) * 60 + pDateTime->second;                   // finally seconds

    *pTimestamp = seconds * 1000;

    return NOERROR;
}

ECode __cdecl _Timestamp_ToLocalDateTime(
        /* [in] */ Int64 timestamp,
        /* [out] */ PDateTime pDateTime)
{
    if (!pDateTime) {
        return E_INVALID_ARGUMENT;
    }

    // TODO:
    Int32 bias = -480;

    timestamp += (-bias) * 60 * 1000;

    return _Timestamp_ToUniversalDateTime(timestamp, pDateTime);
}

ECode __cdecl _Timestamp_FromLocalDateTime(
        /* [in] */ PDateTime pDateTime,
        /* [out] */ PInt64 pTimestamp)
{
    Int64 timestamp = 0;
    ECode ec = _Timestamp_FromUniversalDateTime(pDateTime, &timestamp);
    if (FAILED(ec)) return ec;

    // TODO:
    Int32 bias = -480;

    timestamp -= (-bias) * 60 * 1000;
    *pTimestamp = timestamp;

    return NOERROR;
}

// String ---------------------------------------------------------------------

static const char  day_name[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

inline DateTimeFlag getFmtA(const char **pstr)
{
    if (!memcmp(*pstr, "YYYY", 4 * sizeof(char))) {
        *pstr += 4;
        return DateTimeFlag_Year;
    }
    else if (!memcmp(*pstr, "YY", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_ShortYear;
    }
    else if (!memcmp(*pstr, "MM", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_Month;
    }
    else if (!memcmp(*pstr, "DD", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_Day;
    }
    else if (!memcmp(*pstr, "HH", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_Hour;
    }
    else if (!memcmp(*pstr, "hh", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_THour;
    }
    else if (!memcmp(*pstr, "mm", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_Minute;
    }
    else if (!memcmp(*pstr, "ss", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_Second;
    }
    else if (!memcmp(*pstr, "www", 3 * sizeof(char))) {
        *pstr += 3;
        return DateTimeFlag_Week;
    }
    else if (!memcmp(*pstr, "AM", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_AM;
    }
    else if (!memcmp(*pstr, "PM", 2 * sizeof(char))) {
        *pstr += 2;
        return DateTimeFlag_PM;
    }

    return DateTimeFlag_InVaild;
}

PCarQuintet __cdecl _StringBuf_Append_DateTime(
    PCarQuintet pCq, const char *fmt, PDateTime pDateTime)
{
    assert(pCq && fmt);
    assert(_String_GetLength(fmt, 128) != -1);
    if (!pCq || !fmt || !pDateTime || _String_GetLength(fmt, 128) == -1)
        return pCq;

    while (*fmt) {
        switch (getFmtA(&fmt)) {
            case DateTimeFlag_Year       :
                _StringBuf_Append_Int16(pCq, pDateTime->year,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 4));
                break;
            case DateTimeFlag_ShortYear  :
                _StringBuf_Append_Int16(pCq, pDateTime->year % 100,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 2));
                break;
            case DateTimeFlag_Month      :
                _StringBuf_Append_Int8(pCq, pDateTime->month,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 2));
                break;
            case DateTimeFlag_Day        :
                _StringBuf_Append_Int8(pCq, pDateTime->day,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 2));
                break;
            case DateTimeFlag_Hour       :
                _StringBuf_Append_Int8(pCq, pDateTime->hour % 12,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 2));
                break;
            case DateTimeFlag_THour      :
                _StringBuf_Append_Int8(pCq, pDateTime->hour,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 2));
                break;
            case DateTimeFlag_Minute     :
                _StringBuf_Append_Int8(pCq, pDateTime->minute,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 2));
                break;
            case DateTimeFlag_Second     :
                _StringBuf_Append_Int8(pCq, pDateTime->second,
                        MakeNumberFormat(NumberFormat_ZeroPrefixed, 2));
                break;
            case DateTimeFlag_Week       :
                _StringBuf_Append_String(pCq, day_name[pDateTime->dayofweek],
                        0, 3);
                break;
            case DateTimeFlag_AM         :
            case DateTimeFlag_PM         :
                if (pDateTime->hour /12) _StringBuf_Append_Char8(pCq, 'P');
                else _StringBuf_Append_Char8(pCq, 'A');
                _StringBuf_Append_Char8(pCq, 'M');
                break;
            case DateTimeFlag_InVaild    :
            default:
                _StringBuf_Append_Char8(pCq, *fmt++);
                break;
        }
    }

    return pCq;
}

inline char* getTimeA(const char** ptime, char* flag, int num)
{
    memcpy(flag, *ptime, num * sizeof(char));
    *ptime += num;
    flag[num] = '\0';
    return flag;
}

PDateTime __cdecl _DateTime_Parse_String(
    PDateTime pDateTime, const char* time, const char* fmt)
{
    assert(pDateTime && time && fmt);
    if (!pDateTime || !time || !fmt || _String_GetLength(fmt, 128) == -1)
        return pDateTime;

    char flag[5];
    Int16 i = 0;
    Boolean isPM = FALSE;
    Boolean hasWeek = FALSE;

    while (*fmt && *time) {
        switch (getFmtA(&fmt)) {
            case DateTimeFlag_Year :
                pDateTime->year = (Int16)_String_ToInt32(getTimeA(&time, flag, 4));
                break;
            case DateTimeFlag_ShortYear  :
                pDateTime->year = 2000 + (Int16)_String_ToInt32(getTimeA(&time, flag, 2));
                break;
            case DateTimeFlag_Month :
                pDateTime->month = (Byte)_String_ToInt32(getTimeA(&time, flag, 2));
                break;
            case DateTimeFlag_Day :
                pDateTime->day = (Byte)_String_ToInt32(getTimeA(&time, flag, 2));
                break;
            case DateTimeFlag_Hour :
                pDateTime->hour = (Byte)_String_ToInt32(getTimeA(&time, flag, 2));
                break;
            case DateTimeFlag_THour :
                pDateTime->hour = (Byte)_String_ToInt32(getTimeA(&time, flag, 2));
                break;
            case DateTimeFlag_Minute :
                pDateTime->minute = (Byte)_String_ToInt32(getTimeA(&time, flag, 2));
                break;
            case DateTimeFlag_Second :
                pDateTime->second = (Byte)_String_ToInt32(getTimeA(&time, flag, 2));
                break;
            case DateTimeFlag_Week :
                hasWeek = TRUE;
                getTimeA(&time, flag, 3);
                pDateTime->dayofweek = 0;
                for (i = 0; i < 7; i++) {
                    if (!_String_Compare(flag, day_name[i], StringCase_Insensitive)) {
                        pDateTime->dayofweek = (Byte)i;
                        break;
                    }
                }
                break;
            case DateTimeFlag_AM :
            case DateTimeFlag_PM :
                getTimeA(&time, flag, 2);
                if (!_String_Compare(flag, "PM", StringCase_Insensitive))
                    isPM = TRUE;
                break;
            case DateTimeFlag_InVaild :
            default:
                fmt++;
                time++;
                break;
        }
    }

    if (isPM && (pDateTime->hour < 12))
        pDateTime->hour += 12;

    if (!hasWeek) {
        int year = pDateTime->year;
        int month = pDateTime->month;

        if (month <= 2) {
            month += 12;
            year -= 1;
        }

        pDateTime->dayofweek = (pDateTime->day + 2 * month + 3 * (month + 1) / 5
            + year + year / 4 - year / 100 + year / 400 + 1) % 7;
    }

    return pDateTime;
}

//------------------------------------------------------------------------------
PDateTime __cdecl _DateTime_Initialize(
    PDateTime pDateTime,
    Int32 year, Int32 month, Int32 day, Int32 hour, Int32 minute, Int32 second)
{
    pDateTime->year = year;
    pDateTime->month = month;
    pDateTime->day = day;
    pDateTime->hour = hour;
    pDateTime->minute = minute;
    pDateTime->second = second;

    year = pDateTime->year;
    month = pDateTime->month;

    if (month <= 2) {
        month += 12;
        year -= 1;
    }

    pDateTime->dayofweek = (day + 2 * month + 3 * (month + 1) / 5
        + year + year / 4 - year / 100 + year / 400 + 1) % 7;

    return pDateTime;
}

//------------------------------------------------------------------------------
PDateTime __cdecl _DateTime_ToLocalTime(
        /* [in] */ PDateTime pSrc,
        /* [out] */ PDateTime pDst)
{
    Int64 timeStamp = 0;
    ECode ec = _Timestamp_FromUniversalDateTime(pSrc, &timeStamp);
    if (FAILED(ec)) return pDst;

    _Timestamp_ToLocalDateTime(timeStamp, pDst);
    return pDst;
}

PDateTime __cdecl _DateTime_ToUniversalTime(
        /* [in] */ PDateTime pSrc,
        /* [out] */ PDateTime pDst)
{
    Int64 timeStamp = 0;
    ECode ec = _Timestamp_FromLocalDateTime(pSrc, &timeStamp);
    if (FAILED(ec)) return pDst;

    ec = _Timestamp_ToUniversalDateTime(timeStamp, pDst);
    return pDst;
}

#ifdef  __cplusplus
}
#endif



