//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __ELTIME_H__
#define __ELTIME_H__

#ifdef __cplusplus
_ELASTOS_NAMESPACE_BEGIN
#endif

/** @addtogroup CARTypesRef
  *   @{
  */
typedef struct DateTime{
    Int16 year;
    Byte month;
    Byte dayofweek;
    Byte day;
    Byte hour;
    Byte minute;
    Byte second;
#ifdef __cplusplus
    Boolean IsValid();
    DateTime& Parse(String time, String fmt);
    DateTime& ToLocalTime(const DateTime& dateTime);
    DateTime& ToUniversalTime(const DateTime& dateTime);
    DateTime(Int32 year = 1970, Int32 month = 1, Int32 day = 1,
        Int32 hour = 0, Int32 minute = 0, Int32 second = 0);
#endif
} DateTime, *PDateTime;
/** @} */

#ifdef __cplusplus
_ELASTOS_NAMESPACE_END
#endif

#ifdef __cplusplus
extern "C" {
#endif
    _ELASTOS Boolean __cdecl _DateTime_IsValid(_ELASTOS PDateTime pDataTime);
    _ELASTOS PDateTime __cdecl _DateTime_Parse_String(_ELASTOS PDateTime pDateTime,
        const char* time, const char* fmt);
    _ELASTOS PDateTime __cdecl _DateTime_Initialize(
        _ELASTOS PDateTime pDateTime,
        _ELASTOS Int32 year, _ELASTOS Int32 month, _ELASTOS Int32 day,
        _ELASTOS Int32 hour, _ELASTOS Int32 minute, _ELASTOS Int32 second);
    _ELASTOS PDateTime __cdecl _DateTime_ToLocalTime(_ELASTOS PDateTime pSrc,
        _ELASTOS PDateTime pDst);
    _ELASTOS PDateTime __cdecl _DateTime_ToUniversalTime(_ELASTOS PDateTime pSrc,
        _ELASTOS PDateTime pDst);

    _ELASTOS ECode __cdecl _Timestamp_ToUniversalDateTime(
        _ELASTOS Int64 timestamp, _ELASTOS PDateTime pDateTime);
    _ELASTOS ECode __cdecl _Timestamp_FromUniversalDateTime(
        _ELASTOS PDateTime pDateTime, _ELASTOS PInt64 pTimestamp);
    _ELASTOS ECode __cdecl _Timestamp_ToLocalDateTime(
        _ELASTOS Int64 timestamp, _ELASTOS PDateTime pDateTime);
    _ELASTOS ECode __cdecl _Timestamp_FromLocalDateTime(
        _ELASTOS PDateTime pDateTime, _ELASTOS PInt64 pTimestamp);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

inline DateTime::DateTime(
    Int32 year, Int32 month, Int32 day, Int32 hour, Int32 minute, Int32 second)
{
    _DateTime_Initialize(this, year, month, day, hour, minute, second);
}

inline Boolean DateTime::IsValid()
{
    return _DateTime_IsValid(this);
}

inline DateTime& DateTime::Parse(String time, String fmt)
{
    _DateTime_Parse_String(this, time, fmt);
    return *this;
}

inline DateTime& DateTime::ToLocalTime(const DateTime& dateTime)
{
    _DateTime_ToLocalTime(this, (PDateTime)&dateTime);
    return (DateTime&)dateTime;
}

inline DateTime& DateTime::ToUniversalTime(const DateTime& dateTime)
{
    _DateTime_ToUniversalTime(this, (PDateTime)&dateTime);
    return (DateTime&)dateTime;
}

/** @addtogroup CARTypesRef
  *   @{
  */
class Timestamp64
{
public:
    Timestamp64() : m_timestamp(0) {};

    Timestamp64(Int64 timestamp) : m_timestamp(timestamp) {};

    ECode ToUniversalDateTime(PDateTime pDateTime) {
        return _Timestamp_ToUniversalDateTime(m_timestamp, pDateTime);
    }

    ECode FromUniversalDateTime(const DateTime& dateTime) {
        return _Timestamp_FromUniversalDateTime((PDateTime)&dateTime, &m_timestamp);
    }

    ECode ToLocalDateTime(PDateTime pDateTime) {
        return _Timestamp_ToLocalDateTime(m_timestamp, pDateTime);
    }

    ECode FromLocalDateTime(const DateTime& dateTime) {
        return _Timestamp_FromLocalDateTime((PDateTime)&dateTime, &m_timestamp);
    }

    operator Int64 () const{
        return m_timestamp;
    }

    void operator=(const Timestamp64& timestamp) {
        m_timestamp = timestamp.m_timestamp;
    }

    void operator=(Int64 timestamp) {
        m_timestamp = timestamp;
    }

private:
    Int64  m_timestamp;
};
/** @} */

_ELASTOS_NAMESPACE_END

#else //!__cplusplus

typedef const Int64 Timestamp64;

#endif //__cplusplus

#endif //__ELTIME_H__
