#include "test.h"

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"
#include <elastos/Math.h>

using namespace Elastos;
using namespace Elastos::Utility;
using namespace Elastos::Core;

int CTest::test_toString(int argc, char* argv[])
{
    // Ensure that no matter where this is run, we know what time zone
    // to expect. (Though we still assume an "en" locale.)
    AutoPtr<ITimeZoneHelper> tzh;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    AutoPtr<ITimeZone> tz;
    tzh->GetTimeZone(String("America/Chicago") , (ITimeZone**)&tz);
    Int32 value ;
    tz->GetRawOffset(&value);
    PFL_EX("value : %d" , value)
    ECode ec = tzh->SetDefault(tz);
    PFL_EX("ec : %p" , ec)
    String str;
    AutoPtr<IDate> date;
    CDate::New(0 ,(IDate**)&date);
    date->ToString(&str);
    PFL_EX("Wed Dec 31 18:00:00 CST 1969 == %s" ,str.string())
    assert(str.Equals("Wed Dec 31 18:00:00 CST 1969"));
    return 0;
}

int CTest::test_toGMTString(int argc, char* argv[])
{
    // Based on https://issues.apache.org/jira/browse/HARMONY-501
    AutoPtr<ITimeZoneHelper> tzh;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    AutoPtr<ITimeZone> tz;
    tzh->GetTimeZone(String("America/Los_Angeles") , (ITimeZone**)&tz);
    tzh->SetDefault(tz);

    AutoPtr<ICalendarHelper> cdh;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&cdh);
    AutoPtr<ICalendar> c;
    cdh->GetInstance((ICalendar**)&c);

    c->Clear();
    c->Set(ICalendar::YEAR, 21);
    AutoPtr<IDate> date;
    String outstr;
    c->GetDate((IDate**)&date);
    date->ToString(&outstr);
    PFL_EX("Wed Jan 01 00:00:00 PST 21 == %s " , outstr.string())
    assert(outstr.Equals("Wed Jan 01 00:00:00 PST 21"));

    date->ToGMTString(&outstr);
    PFL_EX("1 Jan 21 08:00:00 GMT == %s" , outstr.string())
    assert(outstr.Equals("1 Jan 21 08:00:00 GMT"));
    c->Set(ICalendar::YEAR, 321);
    c->GetDate((IDate**)&date);
    date->ToString(&outstr);
    PFL_EX("Sun Jan 01 00:00:00 PST 321 == %s " , outstr.string())
    assert(outstr.Equals("Sun Jan 01 00:00:00 PST 321"));
    date->ToGMTString(&outstr);
    PFL_EX("1 Jan 321 08:00:00 GMT == %s" , outstr.string())
    assert(outstr.Equals("1 Jan 321 08:00:00 GMT"));
    return 0;
}

int CTest::test_10583(int argc, char* argv[])
{
    AutoPtr<IDate> date = NULL;
    Int32 year = 0;
    Int32 hour = 0;
    ECode ec;
    ec = CDate::New(String("Jan 1, 1970 11:59:00 AM"), (IDate**)&date);
    assert(ec == NOERROR);
    ec = date->GetHours(&hour);
    assert(ec == NOERROR);
    PFL_EX("hour: %d", hour)
    assert(hour == 11);
    ec = CDate::New(String("Jan 1, 1970"), (IDate**)&date);
    assert(ec == NOERROR);
    ec = date->GetYear(&year);
    assert(ec == NOERROR);
    assert(year == 70);
    return 0;
}

int CTest::test_10581(int argc, char* argv[])
{
    AutoPtr<IDate> date = NULL;
    Int32 year = 0;
    Int32 month = 0;
    Int32 day = 0;
    Int32 hour = 0;
    Int32 minute = 0;
    ECode ec;
    //2046-12-31 00:00
    ec = CDate::New(146, 11, 31, 0, 0, (IDate**)&date);
    assert(ec == NOERROR);
    ec = date->GetYear(&year);
    assert(ec == NOERROR);
    assert(year == 146);
    ec = date->GetMonth(&month);
    assert(ec == NOERROR);
    assert(month == 11);
    ec = date->GetDate(&day);
    assert(ec == NOERROR);
    assert(day == 31);
    ec = date->GetHours(&hour);
    assert(ec == NOERROR);
    assert(hour == 0);
    ec = date->GetMinutes(&minute);
    assert(ec == NOERROR);
    assert(minute == 0);
    PFL_EX("year: %d, month: %d, day: %d, hour: %d, minute: %d", year, month, day, hour, minute)
    return 0;
}

int CTest::test_11090(int argc, char* argv[])
{
    AutoPtr<ISimpleTimeZone> timezone = NULL;
    Boolean isUsed;
    ECode ec;
    ec = CSimpleTimeZone::New(1000, String("TEST"), (ISimpleTimeZone**)&timezone);
    assert(ec == NOERROR);
    ec = timezone->SetStartRuleEx2(10, 8, 7, 1, false);
    assert(ec == NOERROR);
    ec = timezone->SetEndRuleEx2(10, 15, 7, 1, true);
    assert(ec == NOERROR);
    ec = timezone->UseDaylightTime(&isUsed);
    assert(ec == NOERROR);
    assert(isUsed);
    //abnormal
    ec = timezone->SetEndRuleEx2(20, 15, 7, 1, true);
    if (NOERROR == ec) {
        PFL_EX("ERR:the month is wrong.\n");
    }
    //abnormal
    ec = timezone->SetEndRuleEx2(10, 35, 7, 1, true);
    if (NOERROR == ec) {
        PFL_EX("ERR:the day is wrong.\n");
    }
    //abnormal
    ec = timezone->SetEndRuleEx2(10, 15, 12, 1, true);
    if (NOERROR == ec) {
        PFL_EX("ERR:the dayOfWeek is wrong.\n");
    }
    //abnormal
    ec = timezone->SetEndRuleEx2(10, 15, 7, -1, true);
    if (NOERROR == ec) {
        PFL_EX("ERR:the time is wrong.\n");
    }
    return 0;
}

int CTest::test_11094(int argc, char* argv[])
{
    AutoPtr<IDate> date = NULL;
    Int32 year = 0;
    ECode ec;
    ec = CDate::New((IDate**)&date);
    assert(ec == NOERROR);
    ec = date->GetYear(&year);
    assert(ec == NOERROR);
    PFL_EX("114 == %d", year)
    assert(year == 114);
    return 0;
}

int CTest::test_11129(int argc, char* argv[])
{
    AutoPtr<ITimeZoneHelper> helper = NULL;
    AutoPtr<ITimeZone> timezone = NULL;
    String name;
    ECode ec;

    ec = CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    assert(ec == NOERROR);
    ec = helper->GetDefault((ITimeZone**)&timezone);
    assert(ec == NOERROR);
    ec = timezone->GetDisplayName(&name);
    assert(ec == NOERROR);
    if (strcmp(name, String("GMT+00:00"))) {
        PFL_EX("name = %s\n", (const char *)name);
    }
    return 0;
}
