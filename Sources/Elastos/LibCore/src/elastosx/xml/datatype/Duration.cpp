#include "Duration.h"
#include "CGregorianCalendar.h"
#include "CDatatypeConstants.h"
#include "CBigDecimal.h"
#include "StringBuilder.h"

using Elastos::Core::ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Math::CBigDecimal;
using Elastos::Utility::CGregorianCalendar;

namespace Elastosx {
namespace Xml {
namespace Datatype {

CAR_INTERFACE_IMPL(Duration, Object, IDuration)

ECode Duration::GetXMLSchemaType(
    /* [out] */ IQName** type)
{

    Boolean yearSet;
    IsSet(CDatatypeConstants::YEARS, &yearSet);
    Boolean monthSet;
    IsSet(CDatatypeConstants::MONTHS, &monthSet);
    Boolean daySet;
    IsSet(CDatatypeConstants::DAYS, &daySet);
    Boolean hourSet;
    IsSet(CDatatypeConstants::HOURS, &hourSet);
    Boolean minuteSet;
    IsSet(CDatatypeConstants::MINUTES, &minuteSet);
    Boolean secondSet;
    IsSet(CDatatypeConstants::SECONDS, &secondSet);

    // DURATION
    if (yearSet
            && monthSet
            && daySet
            && hourSet
            && minuteSet
            && secondSet) {
        *type = CDatatypeConstants::DURATION;
        REFCOUNT_ADD(*type);
        return NOERROR;
    }

    // DURATION_DAYTIME
    if (!yearSet
            && !monthSet
            && daySet
            && hourSet
            && minuteSet
            && secondSet) {
        *type = CDatatypeConstants::DURATION_DAYTIME;
        REFCOUNT_ADD(*type);
        return NOERROR;
    }

    // DURATION_YEARMONTH
    if (yearSet
            && monthSet
            && !daySet
            && !hourSet
            && !minuteSet
            && !secondSet) {
        *type = CDatatypeConstants::DURATION_YEARMONTH;
        REFCOUNT_ADD(*type);
        return NOERROR;
    }

    // nothing matches
    // throw new IllegalStateException(
    //         "javax.xml.datatype.Duration#getXMLSchemaType():"
    //         + " this Duration does not match one of the XML Schema date/time datatypes:"
    //         + " year set = " + yearSet
    //         + " month set = " + monthSet
    //         + " day set = " + daySet
    //         + " hour set = " + hourSet
    //         + " minute set = " + minuteSet
    //         + " second set = " + secondSet
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode Duration::GetYears(
    /* [out] */ Int32* years)
{
    *years = GetFieldValueAsInt(CDatatypeConstants::YEARS);
    return NOERROR;
}

ECode Duration::GetMonths(
    /* [out] */ Int32* months)
{
    *months = GetFieldValueAsInt(CDatatypeConstants::MONTHS);
    return NOERROR;
}

ECode Duration::GetDays(
    /* [out] */ Int32* days)
{
    *days = GetFieldValueAsInt(CDatatypeConstants::DAYS);
    return NOERROR;
}

ECode Duration::GetHours(
    /* [out] */ Int32* hours)
{
    *hours = GetFieldValueAsInt(CDatatypeConstants::HOURS);
    return NOERROR;
}

ECode Duration::GetMinutes(
    /* [out] */ Int32* minutes)
{
    *minutes = GetFieldValueAsInt(CDatatypeConstants::MINUTES);
    return NOERROR;
}

ECode Duration::GetSeconds(
    /* [out] */ Int32* seconds)
{
    *seconds = GetFieldValueAsInt(CDatatypeConstants::SECONDS);
    return NOERROR;
}

ECode Duration::GetTimeInMillis(
    /* [in] */ ICalendar* startInstant,
    /* [out] */ Int64* time)
{
    AutoPtr<ICloneable> cloneable = ICloneable::Probe(startInstant);
    AutoPtr<IInterface> tmp;
    cloneable->Clone((IInterface**)&tmp);
    AutoPtr<ICalendar> cal = ICalendar::Probe(tmp);
    AddTo(cal.Get());
    *time = GetCalendarTimeInMillis(cal) - GetCalendarTimeInMillis(startInstant);
    return NOERROR;
}

ECode Duration::GetTimeInMillis(
    /* [in] */ IDate* startInstant,
    /* [out] */ Int64* time)
{
    AutoPtr<ICalendar> cal;
    CGregorianCalendar::New((ICalendar**)&cal);
    AddTo(cal);
    Int64 timeValue;
    startInstant->GetTime(&timeValue);
    *time = GetCalendarTimeInMillis(cal) - timeValue;
    return NOERROR;
}

ECode Duration::AddTo(
    /* [in] */ IDate* date)
{
    // check data parameter
    if (date == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ICalendar> cal;
    CGregorianCalendar::New((ICalendar**)&cal);
    cal->SetTime(date);
    AddTo(cal);
    date->SetTime(GetCalendarTimeInMillis(cal));
    return NOERROR;
}

ECode Duration::Subtract(
    /* [in] */ IDuration* rhs,
    /* [out] */ IDuration** duration)
{
    AutoPtr<IDuration> negate;
    rhs->Negate((IDuration**)&negate);
    return Add(negate, duration);
}

ECode Duration::Multiply(
    /* [in] */ Int32 factor,
    /* [out] */ IDuration** duration)
{
    AutoPtr<IBigDecimal> decimal;
    CBigDecimal::ValueOf((Int64)factor, (IBigDecimal**)&decimal);
    return Multiply(decimal, duration);
}

ECode Duration::IsLongerThan(
    /* [in] */ IDuration* rhs,
    /* [out] */ Boolean* rst)
{
    Int32 cpr;
    Compare(rhs, &cpr);
    return cpr == IDatatypeConstants::GREATER;
}

ECode Duration::IsShorterThan(
    /* [in] */ IDuration* rhs,
    /* [out] */ Boolean* rst)
{
    Int32 cpr;
    Compare(rhs, &cpr);
    return cpr == IDatatypeConstants::LESSER;
}

ECode Duration::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    if (other == (IDuration*)this) {
        return TRUE;
    }
    AutoPtr<IDuration> rhs = IDuration::Probe(other);
    if (rhs != NULL) {
        Int32 cpr;
        Compare(rhs, &cpr);
        return cpr == IDatatypeConstants::EQUAL;
    }
    return FALSE;
}

ECode Duration::ToString(
    /* [out] */ String* info)
{
    StringBuilder buf;

    Int32 sign;
    GetSign(&sign);
    if (sign < 0) {
        buf += '-';
    }
    buf += 'P';

    AutoPtr<INumber> nbr;
    GetField(CDatatypeConstants::YEARS, (INumber**)&nbr);
    AutoPtr<IBigInteger> years = IBigInteger::Probe(nbr);
    if (years != NULL) {
        String strYear;
        IObject::Probe(years)->ToString(&strYear);
        buf += strYear;
        buf += 'Y';
    }

    nbr = NULL;
    GetField(CDatatypeConstants::MONTHS, (INumber**)&nbr);
    AutoPtr<IBigInteger> months = IBigInteger::Probe(nbr);
    if (months != NULL) {
        String strMonths;
        IObject::Probe(months)->ToString(&strMonths);
        buf += strMonths;
        buf += 'M';
    }

    nbr = NULL;
    GetField(CDatatypeConstants::DAYS, (INumber**)&nbr);
    AutoPtr<IBigInteger> days = IBigInteger::Probe(nbr);
    if (days != NULL) {
        String strDays;
        IObject::Probe(days)->ToString(&strDays);
        buf += strDays;
        buf += 'D';
    }

    nbr = NULL;
    GetField(CDatatypeConstants::HOURS, (INumber**)&nbr);
    AutoPtr<IBigInteger> hours = IBigInteger::Probe(nbr);

    nbr = NULL;
    GetField(CDatatypeConstants::MINUTES, (INumber**)&nbr);
    AutoPtr<IBigInteger> minutes = IBigInteger::Probe(nbr);

    nbr = NULL;
    GetField(CDatatypeConstants::SECONDS, (INumber**)&nbr);
    AutoPtr<IBigInteger> seconds = IBigInteger::Probe(nbr);


    if (hours != NULL || minutes != NULL || seconds != NULL) {
        buf += 'T';
        if (hours != NULL) {
            String strHours;
            IObject::Probe(hours)->ToString(&strHours);
            buf += strHours;
            buf += 'H';
        }
        if (minutes != NULL) {
            String strMinutes;
            IObject::Probe(minutes)->ToString(&strMinutes);
            buf += strMinutes;
            buf += 'M';
        }
        if (seconds != NULL) {
            String strSeconds;
            IObject::Probe(seconds)->ToString(&strSeconds);
            buf += strSeconds;
            buf += 'S';
        }
    }

    return buf.ToString(info);
}

Int32 Duration::GetFieldValueAsInt(
    /* [in] */ IDatatypeConstantsField* field)
{
    AutoPtr<INumber> n;
    GetField(field, (INumber**)&n);
    Int32 rst = 0;
    if (n != NULL) {
        n->Int32Value(&rst);
    }
    return rst;
}

String Duration::ToString(
    /* [in] */ IBigDecimal* bd)
{
    AutoPtr<IBigInteger> unscaledValue;
    bd->GetUnscaledValue((IBigInteger**)&unscaledValue);
    String intString;
    IObject::Probe(unscaledValue)->ToString(&intString);
    Int32 scale;
    bd->GetScale(&scale);

    if (scale == 0) {
        return intString;
    }

    /* Insert decimal point */
    StringBuilder* buf;
    Int32 insertionPoint = intString.GetLength() - scale;
    if (insertionPoint == 0) { /* Point goes right before intVal */
        return String("0.") + intString;
    }
    else if (insertionPoint > 0) { /* Point goes inside intVal */
        buf = new StringBuilder(intString);
        buf->Insert(insertionPoint, '.');
    }
    else { /* We must insert zeros between point and intVal */
        buf = new StringBuilder(3 - insertionPoint + intString.GetLength());
        buf->Append("0.");
        for (int i = 0; i < -insertionPoint; i++) {
            buf->AppendChar('0');
        }
        buf->Append(intString);
    }
    return buf->ToString();
}

Int64 Duration::GetCalendarTimeInMillis(
    /* [in] */ ICalendar* cal)
{
    AutoPtr<IDate> date;
    cal->GetTime((IDate**)&date);
    Int64 time;
    date->GetTime(&time);
    return time;
}

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx