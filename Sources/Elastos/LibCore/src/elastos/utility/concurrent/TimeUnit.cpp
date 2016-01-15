
#include "TimeUnit.h"
#include "Thread.h"

using Elastos::Core::Thread;

namespace Elastos {
namespace Utility {
namespace Concurrent {

const Int64 TimeUnit::C0;
const Int64 TimeUnit::C1;
const Int64 TimeUnit::C2;
const Int64 TimeUnit::C3;
const Int64 TimeUnit::C4;
const Int64 TimeUnit::C5;
const Int64 TimeUnit::C6;
const Int64 TimeUnit::MAX = Elastos::Core::Math::INT64_MAX_VALUE;

//=============================================================
// TimeUnit
//=============================================================
CAR_INTERFACE_IMPL(TimeUnit, Object, ITimeUnit);

Int64 TimeUnit::X(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [in] */ Int64 over)
{
    if (d >  over) return Elastos::Core::Math::INT64_MAX_VALUE;
    if (d < -over) return Elastos::Core::Math::INT64_MIN_VALUE;
    return d * m;
}

ECode TimeUnit::TimedWait(
    /* [in] */ ISynchronize* obj,
    /* [in] */ Int64 timeout)
{
    if (timeout > 0) {
        Int64 ms;
        ToMillis(timeout, &ms);
        Int32 ns;
        FAIL_RETURN(ExcessNanos(timeout, ms, &ns));
        return obj->Wait(ms, ns);
    }
    return NOERROR;
}

ECode TimeUnit::TimedJoin(
    /* [in] */ IThread* thread,
    /* [in] */ Int64 timeout)
{
    if (timeout > 0) {
        Int64 ms;
        ToMillis(timeout, &ms);
        Int32 ns;
        FAIL_RETURN(ExcessNanos(timeout, ms, &ns));
        return thread->Join(ms, ns);
    }
    return NOERROR;
}

ECode TimeUnit::Sleep(
    /* [in] */ Int64 timeout)
{
    if (timeout > 0) {
        Int64 ms;
        ToMillis(timeout, &ms);
        Int32 ns;
        FAIL_RETURN(ExcessNanos(timeout, ms, &ns));
        return Thread::Sleep(ms, ns);
    }
    return NOERROR;
}

AutoPtr<ITimeUnit> TimeUnit::GetNANOSECONDS()
{
    AutoPtr<ITimeUnit> unit = new NANOSECONDS();
    return unit;
}

AutoPtr<ITimeUnit> TimeUnit::GetMICROSECONDS()
{
    AutoPtr<ITimeUnit> unit = new MICROSECONDS();
    return unit;
}

AutoPtr<ITimeUnit> TimeUnit::GetMILLISECONDS()
{
    AutoPtr<ITimeUnit> unit = new MILLISECONDS();
    return unit;
}

AutoPtr<ITimeUnit> TimeUnit::GetSECONDS()
{
    AutoPtr<ITimeUnit> unit = new SECONDS();
    return unit;
}

AutoPtr<ITimeUnit> TimeUnit::GetMINUTES()
{
    AutoPtr<ITimeUnit> unit = new MINUTES();
    return unit;
}

AutoPtr<ITimeUnit> TimeUnit::GetHOURS()
{
    AutoPtr<ITimeUnit> unit = new HOURS();
    return unit;
}

AutoPtr<ITimeUnit> TimeUnit::GetDAYS()
{
    AutoPtr<ITimeUnit> unit = new DAYS();
    return unit;
}


//=============================================================
// NANOSECONDS
//=============================================================

ECode NANOSECONDS::Convert(
    /* [in] */ Int64 sourceDuration,
    /* [in] */ ITimeUnit* sourceUnit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return sourceUnit->ToNanos(sourceDuration, result);
}

ECode NANOSECONDS::ToNanos(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration;
    return NOERROR;
}

ECode NANOSECONDS::ToMicros(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C1 / C0);
    return NOERROR;
}

ECode NANOSECONDS::ToMillis(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C2 / C0);
    return NOERROR;
}

ECode NANOSECONDS::ToSeconds(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C3 / C0);
    return NOERROR;
}

ECode NANOSECONDS::ToMinutes(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C4 / C0);
    return NOERROR;
}

ECode NANOSECONDS::ToHours(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C5 / C0);
    return NOERROR;
}

ECode NANOSECONDS::ToDays(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C6 / C0);
    return NOERROR;
}

ECode NANOSECONDS::ExcessNanos(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int32)(d - (m * C2));
    return NOERROR;
}

//=============================================================
// MICROSECONDS
//=============================================================

ECode MICROSECONDS::Convert(
    /* [in] */ Int64 sourceDuration,
    /* [in] */ ITimeUnit* sourceUnit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return sourceUnit->ToMicros(sourceDuration, result);
}

ECode MICROSECONDS::ToNanos(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C1 / C0, MAX / (C1 / C0));
    return NOERROR;
}

ECode MICROSECONDS::ToMicros(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration;
    return NOERROR;
}

ECode MICROSECONDS::ToMillis(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C2 / C1);
    return NOERROR;
}

ECode MICROSECONDS::ToSeconds(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C3 / C1);
    return NOERROR;
}

ECode MICROSECONDS::ToMinutes(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C4 / C1);
    return NOERROR;
}

ECode MICROSECONDS::ToHours(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C5 / C1);
    return NOERROR;
}

ECode MICROSECONDS::ToDays(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C6 / C1);
    return NOERROR;
}

ECode MICROSECONDS::ExcessNanos(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int32)((d * C1) - (m * C2));
    return NOERROR;
}

//=============================================================
// MILLISECONDS
//=============================================================

ECode MILLISECONDS::Convert(
    /* [in] */ Int64 sourceDuration,
    /* [in] */ ITimeUnit* sourceUnit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return sourceUnit->ToMillis(sourceDuration, result);
}

ECode MILLISECONDS::ToNanos(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C2 / C0, MAX / (C2 / C0));
    return NOERROR;
}

ECode MILLISECONDS::ToMicros(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C2 / C1, MAX / (C2 / C1));
    return NOERROR;
}

ECode MILLISECONDS::ToMillis(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration;
    return NOERROR;
}

ECode MILLISECONDS::ToSeconds(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C3 / C2);
    return NOERROR;
}

ECode MILLISECONDS::ToMinutes(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C4 / C2);
    return NOERROR;
}

ECode MILLISECONDS::ToHours(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C5 / C2);
    return NOERROR;
}

ECode MILLISECONDS::ToDays(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C6 / C2);
    return NOERROR;
}

ECode MILLISECONDS::ExcessNanos(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

//=============================================================
// SECONDS
//=============================================================

ECode SECONDS::Convert(
    /* [in] */ Int64 sourceDuration,
    /* [in] */ ITimeUnit* sourceUnit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return sourceUnit->ToSeconds(sourceDuration, result);
}

ECode SECONDS::ToNanos(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C3 / C0, MAX / (C3 / C0));
    return NOERROR;
}

ECode SECONDS::ToMicros(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C3 / C1, MAX / (C3 / C1));
    return NOERROR;
}

ECode SECONDS::ToMillis(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C3 / C2, MAX / (C3 / C2));
    return NOERROR;
}

ECode SECONDS::ToSeconds(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration;
    return NOERROR;
}

ECode SECONDS::ToMinutes(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C4 / C3);
    return NOERROR;
}

ECode SECONDS::ToHours(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C5 / C3);
    return NOERROR;
}

ECode SECONDS::ToDays(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C6 / C3);
    return NOERROR;
}

ECode SECONDS::ExcessNanos(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

//=============================================================
// MINUTES
//=============================================================

ECode MINUTES::Convert(
    /* [in] */ Int64 sourceDuration,
    /* [in] */ ITimeUnit* sourceUnit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return sourceUnit->ToMinutes(sourceDuration, result);
}

ECode MINUTES::ToNanos(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C4 / C0, MAX / (C4 / C0));
    return NOERROR;
}

ECode MINUTES::ToMicros(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C4 / C1, MAX / (C4 / C1));
    return NOERROR;
}

ECode MINUTES::ToMillis(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C4 / C2, MAX / (C4 / C2));
    return NOERROR;
}

ECode MINUTES::ToSeconds(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C4 / C3, MAX / (C4 / C3));
    return NOERROR;
}

ECode MINUTES::ToMinutes(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration;
    return NOERROR;
}

ECode MINUTES::ToHours(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C5 / C4);
    return NOERROR;
}

ECode MINUTES::ToDays(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C6 / C4);
    return NOERROR;
}

ECode MINUTES::ExcessNanos(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

//=============================================================
// HOURS
//=============================================================

ECode HOURS::Convert(
    /* [in] */ Int64 sourceDuration,
    /* [in] */ ITimeUnit* sourceUnit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return sourceUnit->ToHours(sourceDuration, result);
}

ECode HOURS::ToNanos(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C5 / C0, MAX / (C5 / C0));
    return NOERROR;
}

ECode HOURS::ToMicros(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C5 / C1, MAX / (C5 / C1));
    return NOERROR;
}

ECode HOURS::ToMillis(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C5 / C2, MAX / (C5 / C2));
    return NOERROR;
}

ECode HOURS::ToSeconds(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C5 / C3, MAX / (C5 / C3));
    return NOERROR;
}

ECode HOURS::ToMinutes(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C5 / C4, MAX / (C5 / C4));
    return NOERROR;
}

ECode HOURS::ToHours(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration;
    return NOERROR;
}

ECode HOURS::ToDays(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration / (C6 / C5);
    return NOERROR;
}

ECode HOURS::ExcessNanos(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

//=============================================================
// DAYS
//=============================================================

ECode DAYS::Convert(
    /* [in] */ Int64 sourceDuration,
    /* [in] */ ITimeUnit* sourceUnit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return sourceUnit->ToDays(sourceDuration, result);
}

ECode DAYS::ToNanos(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C6 / C0, MAX / (C6 / C0));
    return NOERROR;
}

ECode DAYS::ToMicros(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C6 / C1, MAX / (C6 / C1));
    return NOERROR;
}

ECode DAYS::ToMillis(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C6 / C2, MAX / (C6 / C2));
    return NOERROR;
}

ECode DAYS::ToSeconds(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C6 / C3, MAX / (C6 / C3));
    return NOERROR;
}

ECode DAYS::ToMinutes(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C6 / C4, MAX / (C6 / C4));
    return NOERROR;
}

ECode DAYS::ToHours(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = X(duration, C6 / C5, MAX / (C6 / C5));
    return NOERROR;
}

ECode DAYS::ToDays(
    /* [in] */ Int64 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = duration;
    return NOERROR;
}

ECode DAYS::ExcessNanos(
    /* [in] */ Int64 d,
    /* [in] */ Int64 m,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
