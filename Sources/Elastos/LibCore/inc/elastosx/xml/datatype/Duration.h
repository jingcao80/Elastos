#ifndef __ELASTOSX_XML_DATATYPE_DURATION_H__
#define __ELASTOSX_XML_DATATYPE_DURATION_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::INumber;
using Elastos::Math::IBigDecimal;
using Elastos::Math::IBigInteger;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastosx::Xml::Namespace::IQName;

namespace Elastosx {
namespace Xml {
namespace Datatype {

class Duration
    : public Object
    , public IDuration
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetXMLSchemaType(
        /* [out] */ IQName** type);

    CARAPI GetYears(
        /* [out] */ Int32* years);

    CARAPI GetMonths(
        /* [out] */ Int32* months);

    CARAPI GetDays(
        /* [out] */ Int32* days);

    CARAPI GetHours(
        /* [out] */ Int32* hours);

    CARAPI GetMinutes(
        /* [out] */ Int32* minutes);

    CARAPI GetSeconds(
        /* [out] */ Int32* seconds);

    CARAPI GetTimeInMillis(
        /* [in] */ ICalendar* startInstant,
        /* [out] */ Int64* time);

    CARAPI GetTimeInMillis(
        /* [in] */ IDate* startInstant,
        /* [out] */ Int64* time);

    CARAPI AddTo(
        /* [in] */ IDate* date);

    using IDuration::AddTo;

    CARAPI Subtract(
        /* [in] */ IDuration* rhs,
        /* [out] */ IDuration** duration);

    CARAPI Multiply(
        /* [in] */ Int32 factor,
        /* [out] */ IDuration** duration);

    using IDuration::Multiply;

    CARAPI Compare(
        /* [in] */ IDuration* duration,
        /* [out] */ Int32* rst) = 0;

    CARAPI IsLongerThan(
        /* [in] */ IDuration* rhs,
        /* [out] */ Boolean* rst);

    CARAPI IsShorterThan(
        /* [in] */ IDuration* rhs,
        /* [out] */ Boolean* rst);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    Int32 GetFieldValueAsInt(
        /* [in] */ IDatatypeConstantsField* field);

    String ToString(
        /* [in] */ IBigDecimal* bd);

    static Int64 GetCalendarTimeInMillis(
        /* [in] */ ICalendar* cal);
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx
#endif
