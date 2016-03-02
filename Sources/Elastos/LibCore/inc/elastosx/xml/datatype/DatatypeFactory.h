#ifndef __ELASTOSX_XML_DATATYPE_DATATYPEFACTORY_H__
#define __ELASTOSX_XML_DATATYPE_DATATYPEFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::Math::IBigInteger;
using Elastos::Math::IBigDecimal;

namespace Elastosx {
namespace Xml {
namespace Datatype {

class DatatypeFactory
    : public Object
    , public IDatatypeFactory
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI NewInstance(
        /* [out] */ IDatatypeFactory** factory);

    static CARAPI NewInstance(
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ IDatatypeFactory** factory);

    CARAPI NewDuration(
        /* [in] */ Boolean isPositive,
        /* [in] */ Int32 years,
        /* [in] */ Int32 months,
        /* [in] */ Int32 days,
        /* [in] */ Int32 hours,
        /* [in] */ Int32 minutes,
        /* [in] */ Int32 seconds,
        /* [out] */ IDuration** duration);

    using IDatatypeFactory::NewDuration;

    CARAPI NewDurationDayTime(
        /* [in] */ const String& lexicalRepresentation,
        /* [out] */ IDuration** duration);

    CARAPI NewDurationDayTime(
        /* [in] */ Int64 durationInMilliseconds,
        /* [out] */ IDuration** duration);

    CARAPI NewDurationDayTime(
        /* [in] */ Boolean isPositive,
        /* [in] */ IBigInteger* day,
        /* [in] */ IBigInteger* hour,
        /* [in] */ IBigInteger* minute,
        /* [in] */ IBigInteger* second,
        /* [out] */ IDuration** duration);

    CARAPI NewDurationDayTime(
        /* [in] */ Boolean isPositive,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [out] */ IDuration** duration);

    CARAPI NewDurationYearMonth(
        /* [in] */ const String& lexicalRepresentation,
        /* [out] */ IDuration** duration);

    CARAPI NewDurationYearMonth(
        /* [in] */ Int64 durationInMilliseconds,
        /* [out] */ IDuration** duration);

    CARAPI NewDurationYearMonth(
        /* [in] */ Boolean isPositive,
        /* [in] */ IBigInteger* year,
        /* [in] */ IBigInteger* month,
        /* [out] */ IDuration** duration);

    CARAPI newDurationYearMonth(
        /* [in] */ Boolean isPositive,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [out] */ IDuration** duration);

    CARAPI NewXMLGregorianCalendar(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ Int32 millisecond,
        /* [in] */ Int32 timezone,
        /* [out] */ IXMLGregorianCalendar** cdr);

    using IDatatypeFactory::NewXMLGregorianCalendar;

    CARAPI NewXMLGregorianCalendarDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 timezone,
        /* [out] */ IXMLGregorianCalendar** cdr);

    CARAPI NewXMLGregorianCalendarTime(
        /* [in] */ Int32 hours,
        /* [in] */ Int32 minutes,
        /* [in] */ Int32 seconds,
        /* [in] */ Int32 timezone,
        /* [out] */ IXMLGregorianCalendar** cdr);

    CARAPI NewXMLGregorianCalendarTime(
        /* [in] */ Int32 hours,
        /* [in] */ Int32 minutes,
        /* [in] */ Int32 seconds,
        /* [in] */ IBigDecimal* fractionalSecond,
        /* [in] */ Int32 timezone,
        /* [out] */ IXMLGregorianCalendar** cdr);

    CARAPI NewXMLGregorianCalendarTime(
        /* [in] */ Int32 hours,
        /* [in] */ Int32 minutes,
        /* [in] */ Int32 seconds,
        /* [in] */ Int32 milliseconds,
        /* [in] */ Int32 timezone,
        /* [out] */ IXMLGregorianCalendar** cdr);

protected:
    DatatypeFactory(){}
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx
#endif
