
#include <ASN1Time.h>
#include <cmdef.h>
#include <Org.Apache.Harmony.h>

using Elastos::Utility::IGregorianCalendar;
using Elastos::Utility::CGregorianCalendar;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastos::Utility::ITimeZone;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1Time::ASN1Time(
    /* [in] */ Int32 tagNumber)
    : ASN1StringType(tagNumber)
{}

ECode ASN1Time::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    AutoPtr<IGregorianCalendar> c;
    AutoPtr<ITimeZoneHelper> tzh;
    AutoPtr<ITimeZone> tz;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    tzh->GetTimeZone(String("GMT"), (ITimeZone**)&tz);
    CGregorianCalendar::New(tz, (IGregorianCalendar**)&c);
    AutoPtr<ArrayOf<Int32> > times;
    bis->GetTimes((ArrayOf<Int32>**)&times);
    c->Set(ICalendar::YEAR, (*times)[0]);
    c->Set(ICalendar::MONTH, (*times)[1] - 1);
    c->Set(ICalendar::DAY_OF_MONTH, (*times)[2]);
    c->Set(ICalendar::HOUR_OF_DAY, (*times)[3]);
    c->Set(ICalendar::MINUTE, (*times)[4]);
    c->Set(ICalendar::SECOND, (*times)[5]);
    c->Set(ICalendar::MILLISECOND, (*times)[6]);
    AutoPtr<IDate> date;
    c->GetDate((IDate**)&date);
    *object = date.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
