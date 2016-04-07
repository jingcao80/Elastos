
#include "CHttpDate.h"
#include "CSimpleDateFormat.h"
#include "CLocale.h"
#include "elastos/utility/Locale.h"
#include "TimeZone.h"

using Elastos::Utility::CLocale;
using Elastos::Utility::Locale;
using Elastos::Utility::TimeZone;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;

namespace Libcore {
namespace Net {
namespace Http {

const String CHttpDate::BROWSER_COMPATIBLE_DATE_FORMATS[14] = {
        /* This list comes from  {@code org.apache.http.impl.cookie.BrowserCompatSpec}. */
        String("EEEE, dd-MMM-yy HH:mm:ss zzz"), // RFC 1036
        String("EEE MMM d HH:mm:ss yyyy"), // ANSI C asctime()
        String("EEE, dd-MMM-yyyy HH:mm:ss z"),
        String("EEE, dd-MMM-yyyy HH-mm-ss z"),
        String("EEE, dd MMM yy HH:mm:ss z"),
        String("EEE dd-MMM-yyyy HH:mm:ss z"),
        String("EEE dd MMM yyyy HH:mm:ss z"),
        String("EEE dd-MMM-yyyy HH-mm-ss z"),
        String("EEE dd-MMM-yy HH:mm:ss z"),
        String("EEE dd MMM yy HH:mm:ss z"),
        String("EEE,dd-MMM-yy HH:mm:ss z"),
        String("EEE,dd-MMM-yyyy HH:mm:ss z"),
        String("EEE, dd-MM-yyyy HH:mm:ss z"),

        /* RI bug 6641315 claims a cookie of this format was once served by www.yahoo.com */
        String("EEE MMM d yyyy HH:mm:ss z"),
};

pthread_key_t CHttpDate::key_tls;

static void ThreadDestructor(void* st)
{
    ISimpleDateFormat* format = reinterpret_cast<ISimpleDateFormat*>(st);
    if (format) {
        format->Release();
    }
}

Boolean CHttpDate::InitTLS()
{
    Int32 UNUSED(result) = pthread_key_create(&key_tls, ThreadDestructor);
    assert(result == 0);
    return TRUE;
}

AutoPtr<IDateFormat> CHttpDate::InitDateFormatValue()
{
    AutoPtr<IDateFormat> rfc1123;
    CSimpleDateFormat::New(String("EEE, dd MMM yyyy HH:mm:ss zzz"), CLocale::US, (IDateFormat**)&rfc1123);
    AutoPtr<ITimeZone> tz;
    TimeZone::GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    rfc1123->SetTimeZone(tz);
    return rfc1123;
}

CAR_INTERFACE_IMPL(CHttpDate, Singleton, IHttpDate)

CAR_SINGLETON_IMPL(CHttpDate)

ECode CHttpDate::Parse(
    /* [in] */ const String& value,
    /* [out] */ IDate** adate)
{
    return _Parse(value, adate);
}

ECode CHttpDate::Format(
    /* [in] */ IDate* value,
    /* [out] */ String* str)
{
    return _Format(value, str);
}

ECode CHttpDate::_Parse(
    /* [in] */ const String& value,
    /* [out] */ IDate** adate)
{
    VALIDATE_NOT_NULL(adate)

    AutoPtr<IDateFormat> df = (IDateFormat*)pthread_getspecific(key_tls);
    if (df == NULL) {
        df = InitDateFormatValue();
        ASSERT_TRUE(pthread_setspecific(key_tls, df.Get()) == 0);
        df->AddRef();
    }
    if (!FAILED(df->Parse(value, adate))) {
        return NOERROR;
    }
    // try {
    //     return STANDARD_DATE_FORMAT.get().parse(value);
    // } catch (ParseException ignore) {
    // }
    for (Int32 i = 0; i < 14; i++) {
        // try {
        AutoPtr<ISimpleDateFormat> sdf;
        ECode ec = CSimpleDateFormat::New(BROWSER_COMPATIBLE_DATE_FORMATS[i], CLocale::US, (ISimpleDateFormat**)&sdf);
        if(ec == NOERROR) {
            IDateFormat::Probe(sdf)->Parse(value, adate);
            return NOERROR;
        }
        // } catch (ParseException ignore) {
        // }
    }

    *adate = NULL;
    return NOERROR;
}

ECode CHttpDate::_Format(
    /* [in] */ IDate* value,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IDateFormat> df = (IDateFormat*)pthread_getspecific(key_tls);
    if (df == NULL) {
        df = InitDateFormatValue();
        ASSERT_TRUE(pthread_setspecific(key_tls, df.Get()) == 0);
        df->AddRef();
    }
    return df->Format(value, str);
}

} // namespace Http
} // namespace Net
} // namespace Libcore
