#include "DatatypeFactory.h"
#include "utility/logging/Slogger.h"
#include "Math.h"
#include "CBigDecimal.h"
#include "CBigInteger.h"
#include "Thread.h"
#include "CPathClassLoader.h"
#include "FactoryFinder.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::Core::Math;
using Elastos::Core::Thread;
using Elastos::Core::CPathClassLoader;
using Elastos::Math::CBigDecimal;
using Elastos::Math::CBigInteger;

namespace Elastosx {
namespace Xml {
namespace Datatype {

CAR_INTERFACE_IMPL(DatatypeFactory, Object, IDatatypeFactory)

ECode DatatypeFactory::NewInstance(
    /* [out] */ IDatatypeFactory** factory)
{
    AutoPtr<IInterface> tmp;
    FactoryFinder::Find(
            /* The default property name according to the JAXP spec */
            DATATYPEFACTORY_PROPERTY,
            /* The fallback implementation class name */
            DATATYPEFACTORY_IMPLEMENTATION_CLASS,
            (IInterface**)&tmp);
    *factory = IDatatypeFactory::Probe(tmp);
    REFCOUNT_ADD(*factory)
    return NOERROR;
}

ECode DatatypeFactory::NewInstance(
    /* [in] */ String factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ IDatatypeFactory** factory)
{
    *factory = NULL;
    if (factoryClassName == NULL) {
        SLOGGERE("DatatypeFactory", "factoryClassName == null")
        return E_DATATYPE_CONFIGURATION_EXCEPTION;
        // throw new DatatypeConfigurationException("factoryClassName == null");
    }

    AutoPtr<IClassLoader> pLoader = classLoader;
    if (pLoader == NULL) {
        FAIL_RETURN(Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&pLoader))
    }
    // try {
        if (pLoader == NULL) {
            String path = factoryClassName.Substring(0, factoryClassName.LastIndexOf('.'));
            FAIL_RETURN(CPathClassLoader::New(path, (IClassLoader**)&pLoader))
        }

        AutoPtr<IClassInfo> clsInfo;
        FAIL_RETURN(pLoader->LoadClass(factoryClassName, (IClassInfo**)&clsInfo))
        if (clsInfo == NULL) {
            return E_CLASS_NOT_FOUND_EXCEPTION;
        }
        ClassID id;
        clsInfo->GetId(&id);
        AutoPtr<IInterface> tmp;
        FAIL_RETURN(CObject::CreateInstance(id, RGM_SAME_DOMAIN, EIID_IInterface, (IInterface**)&tmp))
        *factory = IDatatypeFactory::Probe(tmp);
        REFCOUNT_ADD(*factory)
        return NOERROR;
        // Class<?> type = classLoader != null
        //         ? classLoader.loadClass(factoryClassName)
        //         : Class.forName(factoryClassName);
        // return (DatatypeFactory) type.newInstance();
//     } catch (ClassNotFoundException e) {
//         throw new DatatypeConfigurationException(e);
//     } catch (InstantiationException e) {
//         throw new DatatypeConfigurationException(e);
//     } catch (IllegalAccessException e) {
//         throw new DatatypeConfigurationException(e);
//     }
}

ECode DatatypeFactory::NewDuration(
    /* [in] */ Boolean isPositive,
    /* [in] */ Int32 years,
    /* [in] */ Int32 months,
    /* [in] */ Int32 days,
    /* [in] */ Int32 hours,
    /* [in] */ Int32 minutes,
    /* [in] */ Int32 seconds,
    /* [out] */ IDuration** duration)
{
    // years may not be set
    AutoPtr<IBigInteger> realYears;
    if (years != IDatatypeConstants::FIELD_UNDEFINED){
        CBigInteger::ValueOf((Int64) years, (IBigInteger**)&realYears);
    } else {
        realYears = NULL;
    }

    // months may not be set
    AutoPtr<IBigInteger> realMonths;
    if (months != IDatatypeConstants::FIELD_UNDEFINED){
        CBigInteger::ValueOf((Int64) months, (IBigInteger**)&realMonths);
    } else {
        realMonths = NULL;
    }

    // days may not be set
    AutoPtr<IBigInteger> realDays;
    if (days != IDatatypeConstants::FIELD_UNDEFINED){
        CBigInteger::ValueOf((Int64) days, (IBigInteger**)&realDays);
    } else {
        realDays = NULL;
    }

    // hours may not be set
    AutoPtr<IBigInteger> realHours;
    if (hours != IDatatypeConstants::FIELD_UNDEFINED){
        CBigInteger::ValueOf((Int64) hours, (IBigInteger**)&realHours);
    } else {
        realHours = NULL;
    }

    // minutes may not be set
    AutoPtr<IBigInteger> realMinutes;
    if (minutes != IDatatypeConstants::FIELD_UNDEFINED){
        CBigInteger::ValueOf((Int64) minutes, (IBigInteger**)&realMinutes);
    } else {
        realMinutes = NULL;
    }

    // seconds may not be set
    AutoPtr<IBigDecimal> realSeconds;
    if (seconds != IDatatypeConstants::FIELD_UNDEFINED){
        CBigDecimal::ValueOf((Int64) seconds, (IBigDecimal**)&realSeconds);
    } else {
        realSeconds = NULL;
    }

    return NewDuration(
            isPositive,
            realYears,
            realMonths,
            realDays,
            realHours,
            realMinutes,
            realSeconds,
            duration);
}

ECode DatatypeFactory::NewDurationDayTime(
    /* [in] */ String lexicalRepresentation,
    /* [out] */ IDuration** duration)
{
    *duration = NULL;
    if (lexicalRepresentation == NULL) {
        SLOGGERE("DatatypeFactory", "lexicalRepresentation == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    // The lexical representation must match the pattern [^YM]*(T.*)?
    Int32 pos = lexicalRepresentation.IndexOf('T');
    Int32 length = (pos >= 0) ? pos : lexicalRepresentation.GetLength();
    for (Int32 i = 0; i < length; ++i) {
        Char32 c = lexicalRepresentation.GetChar(i);
        if (c == 'Y' || c == 'M') {
            SLOGGERE("DatatypeFactory", String("Invalid dayTimeDuration value: ") + lexicalRepresentation)
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NewDuration(lexicalRepresentation, duration);
}

ECode DatatypeFactory::NewDurationDayTime(
    /* [in] */ Int64 durationInMilliseconds,
    /* [out] */ IDuration** duration)
{
    Int64 _durationInMilliseconds = durationInMilliseconds;
    if (_durationInMilliseconds == 0) {
        return NewDuration(TRUE, IDatatypeConstants::FIELD_UNDEFINED,
                IDatatypeConstants::FIELD_UNDEFINED, 0, 0, 0, 0, duration);
    }
    Boolean tooLong = FALSE;
    Boolean isPositive;
    if (_durationInMilliseconds < 0) {
        isPositive = FALSE;
        if (_durationInMilliseconds == Elastos::Core::Math::INT64_MIN_VALUE) {
            _durationInMilliseconds++;
            tooLong = TRUE;
        }
        _durationInMilliseconds *= -1;
    }
    else {
        isPositive = TRUE;
    }

    Int64 val = _durationInMilliseconds;
    Int32 milliseconds = (Int32) (val % 60000L); // 60000 milliseconds per minute
    if (tooLong) {
        ++milliseconds;
    }
    if (milliseconds % 1000 == 0) {
        Int32 seconds = milliseconds / 1000;
        val = val / 60000L;
        Int32 minutes = (Int32) (val % 60L); // 60 minutes per hour
        val = val / 60L;
        Int32 hours = (Int32) (val % 24L); // 24 hours per day
        Int64 days = val / 24L;
        if (days <= ((Int64) Elastos::Core::Math::INT32_MAX_VALUE)) {
            return NewDuration(isPositive, IDatatypeConstants::FIELD_UNDEFINED,
                    IDatatypeConstants::FIELD_UNDEFINED, (Int32) days, hours, minutes, seconds, duration);
        }
        else {
            AutoPtr<IBigInteger> bDays;
            AutoPtr<IBigInteger> bHours;
            AutoPtr<IBigInteger> bMinutes;
            AutoPtr<IBigDecimal> bMilliseconds;

            CBigInteger::ValueOf((Int64) days, (IBigInteger**)&bDays);
            CBigInteger::ValueOf((Int64) hours, (IBigInteger**)&bHours);
            CBigInteger::ValueOf((Int64) minutes, (IBigInteger**)&bMinutes);
            CBigDecimal::ValueOf((Int64) milliseconds, 3, (IBigDecimal**)&bMilliseconds);

            return NewDuration(isPositive, NULL, NULL,bDays, bHours, bMinutes, bMilliseconds, duration);
        }
    }

    AutoPtr<IBigDecimal> seconds;
    CBigDecimal::ValueOf((Int64) milliseconds, 3, (IBigDecimal**)&seconds);
    val = val / 60000L;

    AutoPtr<IBigInteger> minutes;
    CBigInteger::ValueOf((Int64) val % 60L, (IBigInteger**)&minutes); // 60 minutes per hour
    val = val / 60L;

    AutoPtr<IBigInteger> hours;
    CBigInteger::ValueOf((Int64) val % 24L, (IBigInteger**)&hours); // 24 hours per day
    val = val / 24L;

    AutoPtr<IBigInteger> days;
    CBigInteger::ValueOf((Int64) val, (IBigInteger**)&days); // 24 hours per day
    return NewDuration(isPositive, NULL, NULL, days, hours, minutes, seconds, duration);
}

ECode DatatypeFactory::NewDurationDayTime(
    /* [in] */ Boolean isPositive,
    /* [in] */ IBigInteger* day,
    /* [in] */ IBigInteger* hour,
    /* [in] */ IBigInteger* minute,
    /* [in] */ IBigInteger* second,
    /* [out] */ IDuration** duration)
{
    AutoPtr<IBigDecimal> secondDecimal;
    if (second != NULL) {
        CBigDecimal::New(second, (IBigDecimal**)&secondDecimal);
    }
    return NewDuration(
            isPositive,
            NULL,  // years
            NULL, // months
            day,
            hour,
            minute,
            secondDecimal,
            duration
    );
}

ECode DatatypeFactory::NewDurationDayTime(
    /* [in] */ Boolean isPositive,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [out] */ IDuration** duration)
{
    return NewDuration(isPositive,
            IDatatypeConstants::FIELD_UNDEFINED, IDatatypeConstants::FIELD_UNDEFINED,
            day, hour, minute, second, duration);
}

ECode DatatypeFactory::NewDurationYearMonth(
    /* [in] */ String lexicalRepresentation,
    /* [out] */ IDuration** duration)
{
    *duration = NULL;
    if (lexicalRepresentation == NULL) {
        SLOGGERE("DatatypeFactory", "lexicalRepresentation == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // The lexical representation must match the pattern [^DT]*.
    Int32 length = lexicalRepresentation.GetLength();
    for (Int32 i = 0; i < length; ++i) {
        Char32 c = lexicalRepresentation.GetChar(i);
        if (c == 'D' || c == 'T') {
            SLOGGERE("DatatypeFactory", String("Invalid yearMonthDuration value: ") + lexicalRepresentation);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NewDuration(lexicalRepresentation, duration);
}

ECode DatatypeFactory::NewDurationYearMonth(
    /* [in] */ Int64 durationInMilliseconds,
    /* [out] */ IDuration** duration)
{
    return NewDuration(durationInMilliseconds, duration);
}

ECode DatatypeFactory::NewDurationYearMonth(
    /* [in] */ Boolean isPositive,
    /* [in] */ IBigInteger* year,
    /* [in] */ IBigInteger* month,
    /* [out] */ IDuration** duration)
{
    return NewDuration(
            isPositive,
            year,
            month,
            NULL, // days
            NULL, // hours
            NULL, // minutes
            NULL,  // seconds
            duration
    );
}

ECode DatatypeFactory::newDurationYearMonth(
    /* [in] */ Boolean isPositive,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [out] */ IDuration** duration)
{
    return NewDuration(isPositive, year, month,
            IDatatypeConstants::FIELD_UNDEFINED, IDatatypeConstants::FIELD_UNDEFINED,
            IDatatypeConstants::FIELD_UNDEFINED, IDatatypeConstants::FIELD_UNDEFINED, duration);
}

ECode DatatypeFactory::NewXMLGregorianCalendar(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [in] */ Int32 millisecond,
    /* [in] */ Int32 timezone,
    /* [out] */ IXMLGregorianCalendar** cdr)
{
    // year may be undefined
    AutoPtr<IBigInteger> realYear;
    if (year != IDatatypeConstants::FIELD_UNDEFINED) {
        CBigInteger::ValueOf((Int64) year, (IBigInteger**)&realYear);
    }

    // millisecond may be undefined
    // millisecond must be >= 0 millisecond <= 1000
    AutoPtr<IBigDecimal> realMillisecond = NULL; // undefined value
    if (millisecond != IDatatypeConstants::FIELD_UNDEFINED) {
        if (millisecond < 0 || millisecond > 1000) {
            // throw new IllegalArgumentException(
            //         "javax.xml.datatype.DatatypeFactory#newXMLGregorianCalendar("
            //         + "int year, int month, int day, int hour, int minute, int second, int millisecond, int timezone)"
            //         + "with invalid millisecond: " + millisecond
            // );
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        CBigDecimal::ValueOf((Int64) millisecond, 3, (IBigDecimal**)&realMillisecond);
    }

    return NewXMLGregorianCalendar(
            realYear,
            month,
            day,
            hour,
            minute,
            second,
            realMillisecond,
            timezone,
            cdr
    );
}

ECode DatatypeFactory::NewXMLGregorianCalendarDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 timezone,
    /* [out] */ IXMLGregorianCalendar** cdr)
{
    return NewXMLGregorianCalendar(
            year,
            month,
            day,
            IDatatypeConstants::FIELD_UNDEFINED, // hour
            IDatatypeConstants::FIELD_UNDEFINED, // minute
            IDatatypeConstants::FIELD_UNDEFINED, // second
            IDatatypeConstants::FIELD_UNDEFINED, // millisecond
            timezone,
            cdr);
}

ECode DatatypeFactory::NewXMLGregorianCalendarTime(
    /* [in] */ Int32 hours,
    /* [in] */ Int32 minutes,
    /* [in] */ Int32 seconds,
    /* [in] */ Int32 timezone,
    /* [out] */ IXMLGregorianCalendar** cdr)
{
    return NewXMLGregorianCalendar(
            IDatatypeConstants::FIELD_UNDEFINED, // Year
            IDatatypeConstants::FIELD_UNDEFINED, // Month
            IDatatypeConstants::FIELD_UNDEFINED, // Day
            hours,
            minutes,
            seconds,
            IDatatypeConstants::FIELD_UNDEFINED, //Millisecond
            timezone,
            cdr);
}

ECode DatatypeFactory::NewXMLGregorianCalendarTime(
    /* [in] */ Int32 hours,
    /* [in] */ Int32 minutes,
    /* [in] */ Int32 seconds,
    /* [in] */ IBigDecimal* fractionalSecond,
    /* [in] */ Int32 timezone,
    /* [out] */ IXMLGregorianCalendar** cdr)
{
    return NewXMLGregorianCalendar(
            NULL, // year
            IDatatypeConstants::FIELD_UNDEFINED, // month
            IDatatypeConstants::FIELD_UNDEFINED, // day
            hours,
            minutes,
            seconds,
            fractionalSecond,
            timezone,
            cdr);
}

ECode DatatypeFactory::NewXMLGregorianCalendarTime(
    /* [in] */ Int32 hours,
    /* [in] */ Int32 minutes,
    /* [in] */ Int32 seconds,
    /* [in] */ Int32 milliseconds,
    /* [in] */ Int32 timezone,
    /* [out] */ IXMLGregorianCalendar** cdr)
{
    // millisecond may be undefined
    // millisecond must be >= 0 millisecond <= 1000
    AutoPtr<IBigDecimal> realMilliseconds; // undefined value
    if (milliseconds != IDatatypeConstants::FIELD_UNDEFINED) {
        if (milliseconds < 0 || milliseconds > 1000) {
            // throw new IllegalArgumentException(
            //         "javax.xml.datatype.DatatypeFactory#newXMLGregorianCalendarTime("
            //         + "int hours, int minutes, int seconds, int milliseconds, int timezone)"
            //         + "with invalid milliseconds: " + milliseconds
            // );
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        CBigDecimal::ValueOf((Int64) milliseconds, 3, (IBigDecimal**)&realMilliseconds);
    }

    return NewXMLGregorianCalendarTime(
            hours,
            minutes,
            seconds,
            realMilliseconds,
            timezone,
            cdr
    );
}
} // namespace Datatype
} // namespace Xml
} // namespace Elastosx