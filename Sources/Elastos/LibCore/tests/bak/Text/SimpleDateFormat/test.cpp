#include "test.h"
#include <stdio.h>

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>


using namespace Elastos;
using namespace Elastos::Text;
using namespace Elastos::Utility;
using namespace Libcore::ICU;

int CTest::test_2DigitYearStartIsCloned(int argc, char* argv[])
{
    AutoPtr<ISimpleDateFormat> sdf;
    printf("%s %d\n", __FILE__, __LINE__);
    CSimpleDateFormat::New((ISimpleDateFormat**)&sdf);
    AutoPtr<IDate> originalDate;
    sdf->Get2DigitYearStart((IDate**)&originalDate);
    // originalDate->SetTime(0);
    AutoPtr<IDate> originalDate2;
    sdf->Get2DigitYearStart((IDate**)&originalDate2);
    PFL_EX("originalDate2 : %p , originalDate :%p" ,  originalDate2.Get() ,originalDate.Get())
    assert(originalDate2 != originalDate);
    Boolean flagequal(FALSE);
    originalDate2->Equals(originalDate , &flagequal);
    assert(flagequal == TRUE);

    AutoPtr<IDate> newDate;
    CDate::New((IDate**)&newDate);
    sdf->Set2DigitYearStart(newDate);
    AutoPtr<IDate> originalDate3;
    sdf->Get2DigitYearStart((IDate**)&originalDate3);
    assert(originalDate3 != newDate);
    newDate->SetTime(0);
    AutoPtr<IDate> originalDate4;
    sdf->Get2DigitYearStart((IDate**)&originalDate4);
    originalDate4->Equals(newDate , &flagequal);

    assert(flagequal != TRUE);
    return 0;
}

String formatDate(ILocale* l, const String& fmt)
{
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(fmt, l, (ISimpleDateFormat**)&sdf);
    AutoPtr<IDate> date;
    CDate::New(0, (IDate**)&date);
    String str("");
    sdf->FormatDate(date, &str);
    return str;
}

AutoPtr<ICalendar> parseDate(ILocale* l, const String& fmt, const String& value)
{
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(fmt, l, (ISimpleDateFormat**)&sdf);
    AutoPtr<IParsePosition> pp;
    CParsePosition::New(0, (IParsePosition**)&pp);
    AutoPtr<IDate> d;
    sdf->ParseEx(value, pp, (IDate**)&d);
    if (d == NULL) {
        printf("fail==================================\n");
    }
    AutoPtr<ITimeZoneHelper> tzh;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    AutoPtr<ITimeZone> tz;
    tzh->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    AutoPtr<ICalendarHelper> ch;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&ch);
    AutoPtr<ICalendar> c;
    ch->GetInstanceEx2(tz, (ICalendar**)&c);
    c->SetDate(d);
    return c;
}

int CTest::test_StandAloneNames(int argc, char* argv[])
{
    AutoPtr<ITimeZoneHelper> tzh;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    AutoPtr<ITimeZone> gmtzone;
    tzh->GetTimeZone(String("GMT"), (ITimeZone**)&gmtzone);
    tzh->SetDefault(gmtzone);

    AutoPtr<ILocale> en;
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochelp);
    lochelp->GetENGLISH((ILocale**)&en);
    AutoPtr<ILocale> pl;
    ECode ec1 = CLocale::New(String("pl"), (ILocale**)&pl);
    AutoPtr<ILocale> ru;
    ECode ec2 = CLocale::New(String("ru"), (ILocale**)&ru);

    assert(String("January") == formatDate(en, String("MMMM")));
    assert(String("January") == formatDate(en, String("LLLL")));
    assert(String("stycznia") == formatDate(pl, String("MMMM")));
    assert(String("stycze\u0144") == formatDate(pl, String("LLLL")));

    assert(String("Thursday") == formatDate(en, String("EEEE")));
    assert(String("Thursday") == formatDate(en, String("cccc")));
    assert(String("\u0447\u0435\u0442\u0432\u0435\u0440\u0433") == formatDate(ru, String("EEEE")));
    assert(String("\u0427\u0435\u0442\u0432\u0435\u0440\u0433") == formatDate(ru, String("cccc")));

    Int32 value(0);
    parseDate(en, String("yyyy-MMMM-dd"), String("1980-June-12"))->Get(ICalendar::MONTH, &value);
    assert(ICalendar::JUNE == value);
    parseDate(en, String("yyyy-LLLL-dd"), String("1980-June-12"))->Get(ICalendar::MONTH, &value);
    assert(ICalendar::JUNE == value);
    parseDate(pl, String("yyyy-MMMM-dd"), String("1980-czerwca-12"))->Get(ICalendar::MONTH, &value);
    assert(ICalendar::JUNE == value);
    parseDate(pl, String("yyyy-LLLL-dd"), String("1980-czerwiec-12"))->Get(ICalendar::MONTH, &value);
    assert(ICalendar::JUNE == value);

    parseDate(en, String("EEEE"), String("Tuesday"))->Get(ICalendar::DAY_OF_WEEK, &value);
    assert(ICalendar::TUESDAY == value);
    parseDate(en, String("cccc"), String("Tuesday"))->Get(ICalendar::DAY_OF_WEEK, &value);
    assert(ICalendar::TUESDAY == value);
    parseDate(ru, String("EEEE"), String("\u0432\u0442\u043e\u0440\u043d\u0438\u043a"))->Get(ICalendar::DAY_OF_WEEK, &value);
    assert(ICalendar::TUESDAY == value);
    parseDate(ru, String("cccc"), String("\u0412\u0442\u043e\u0440\u043d\u0438\u043a"))->Get(ICalendar::DAY_OF_WEEK, &value);
    assert(ICalendar::TUESDAY == value);

    return 0;
}

int CTest::test2038(int argc, char* argv[]) {
    AutoPtr<ISimpleDateFormat> format ;
    AutoPtr<ILocale> usloc;
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochelp);
    lochelp->GetUS((ILocale **) &usloc);
    CSimpleDateFormat::New(String("EEE MMM dd HH:mm:ss yyyy") , usloc , (ISimpleDateFormat**)&format);

    AutoPtr<ITimeZone> utczone;
    AutoPtr<ITimeZoneHelper> zonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&zonehelp);
    zonehelp->GetTimeZone(String("UTC") , (ITimeZone **) &utczone);
    format->SetTimeZone(utczone);

    Int32 MIN_VALUE = 0x80000000;
    AutoPtr<IDate> longdate;
    CDate::New(((Int64) MIN_VALUE + MIN_VALUE) * 1000L , (IDate**)&longdate);
    String outstr;
    format->FormatDate(longdate,&outstr);
    assert(String("Sun Nov 24 17:31:44 1833") == outstr);

    CDate::New(((Int64) MIN_VALUE) * 1000L , (IDate**)&longdate);
    format->FormatDate(longdate,&outstr);
    assert(String("Fri Dec 13 20:45:52 1901") == outstr);

    CDate::New((Int64)0L , (IDate**)&longdate);
    format->FormatDate(longdate,&outstr);
    assert(String("Thu Jan 01 00:00:00 1970") == outstr);

    Int32 MAX_VALUE = 0x7FFFFFFF;
    CDate::New((Int64)MAX_VALUE  * 1000L, (IDate**)&longdate);
    format->FormatDate(longdate,&outstr);
    assert(String("Tue Jan 19 03:14:07 2038") == outstr);

    CDate::New(((Int64)2L + MAX_VALUE + MAX_VALUE) * 1000L, (IDate**)&longdate);
    format->FormatDate(longdate,&outstr);
    assert(String("Sun Feb 07 06:28:16 2106") == outstr);
}

// http://code.google.com/p/android/issues/detail?id=13420
int CTest::testParsingUncommonTimeZoneAbbreviations(int argc, char* argv[]) {
    String fmt = String("yyyy-MM-dd HH:mm:ss.SSS z");
    String date = String("2010-12-23 12:44:57.0 CET");
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lochelp);
    AutoPtr<ILocale> ukloc;
    AutoPtr<ILocale> usloc;
    lochelp->GetUK((ILocale **) &ukloc);
    lochelp->GetUS((ILocale **) &usloc);
    // ICU considers "CET" (Central European Time) to be common in Britain...
    Int64 miltime = 0 ;
    parseDate(ukloc, fmt, date)->GetTimeInMillis(&miltime);
    assert(1293104697000L == miltime );
    // ...but not in the US. Check we can parse such a date anyway.
    parseDate(usloc, fmt, date)->GetTimeInMillis(&miltime);
    assert(1293104697000L == miltime );
}

int CTest::testFormattingUncommonTimeZoneAbbreviations(int argc, char* argv[]) {
    // In Honeycomb, only one Olson id was associated with CET (or any
    // other "uncommon" abbreviation).
    String fmt = String("yyyy-MM-dd HH:mm:ss.SSS z");
    String date = String("1970-01-01 01:00:00.000 CET");
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lochelp);
    AutoPtr<ILocale> usloc;
    lochelp->GetUS((ILocale **) &usloc);

    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(fmt, usloc , (ISimpleDateFormat**)&sdf);
    AutoPtr<ITimeZoneHelper> zonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&zonehelp);
    AutoPtr<ITimeZone> ebzone;
    zonehelp->GetTimeZone(String("Europe/Berlin"),(ITimeZone**)&ebzone);
    sdf->SetTimeZone(ebzone);
    AutoPtr<IDate> adate;
    CDate::New(0,(IDate**)&adate);
    String outstr;
    sdf->FormatDate(adate ,&outstr);
    assert(date  == outstr);

    CSimpleDateFormat::New(fmt, usloc , (ISimpleDateFormat**)&sdf);
    zonehelp->GetTimeZone(String("Europe/Zurich"),(ITimeZone**)&ebzone);
    sdf->SetTimeZone(ebzone);
    sdf->FormatDate(adate ,&outstr);
    assert(date == outstr);
}

// http://code.google.com/p/android/issues/detail?id=8258
int CTest::testTimeZoneFormatting(int argc, char* argv[]) {
    AutoPtr<IDate> epoch;
    CDate::New(0, (IDate **) &epoch);

    // Create a SimpleDateFormat that defaults to America/Chicago...
    AutoPtr<ITimeZoneHelper> zonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&zonehelp);
    AutoPtr<ITimeZone> aczone;
    zonehelp->GetTimeZone(String("America/Chicago"),(ITimeZone**)&aczone);
    zonehelp->SetDefault(aczone);

    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss Z"),(ISimpleDateFormat**)&sdf);
    // We should see something appropriate to America/Chicago...
    String result;
    sdf->FormatDate(epoch,&result);
    assert(String("1969-12-31 18:00:00 -0600") == result);
    // We can set any TimeZone we want:

    AutoPtr<ITimeZone> AMERICA_LOS_ANGELES;
    zonehelp->GetTimeZone(String("America/Los_Angeles"), (ITimeZone**)&AMERICA_LOS_ANGELES);
    sdf->SetTimeZone(AMERICA_LOS_ANGELES);
    sdf->FormatDate(epoch,&result);
    assert(String("1969-12-31 16:00:00 -0800") == result);

    AutoPtr<ITimeZone> utczone;
    zonehelp->GetTimeZone(String("UTC") , (ITimeZone**)&utczone);
    sdf->SetTimeZone(utczone);
    sdf->FormatDate(epoch, &result);
    assert(String("1970-01-01 00:00:00 +0000") == result);

    // A new SimpleDateFormat will default to America/Chicago...
    CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss Z") , (ISimpleDateFormat**)&sdf);
    // ...and parsing an America/Los_Angeles time will *not* change that...
    AutoPtr<IDate> outdate;
    sdf->Parse(String("2010-12-03 00:00:00 -0800"),(IDate**)&outdate);
    // ...so our time zone here is "America/Chicago":
    sdf->FormatDate(epoch,&result);
    assert(String("1969-12-31 18:00:00 -0600") == result);
    // We can set any TimeZone we want:
    sdf->SetTimeZone(AMERICA_LOS_ANGELES);
    sdf->FormatDate(epoch , &result);
    assert(String("1969-12-31 16:00:00 -0800") == result);

    sdf->SetTimeZone(utczone);
    sdf->FormatDate(epoch , &result);
    assert(String("1970-01-01 00:00:00 +0000") == result);

    CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss") , (ISimpleDateFormat**)&sdf);
    sdf->SetTimeZone(utczone);
    AutoPtr<IDate> date;
    sdf->Parse(String("2010-07-08 02:44:48") , (IDate**)&date);
    Int64 outtime;
    date->GetTime(&outtime);
    assert(1278557088000L == outtime );

    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm:ssZ") , (ISimpleDateFormat**)&sdf);
    sdf->SetTimeZone(AMERICA_LOS_ANGELES);
    Int32 offset = 0;
    AMERICA_LOS_ANGELES->GetRawOffset(&offset);
    PFL_EX("offset : %d", offset)
    sdf->FormatDate(date,&result);
    PFL_EX("expect: 2010-07-07T19:44:48-0700 , result :%s" , result.string())
    assert(String("2010-07-07T19:44:48-0700") == result);

    sdf->SetTimeZone(utczone);
    sdf->FormatDate(date , &result);
    PFL_EX("expect: 2010-07-08T02:44:48+0000 , result : %s " ,result.string())
    assert(String("2010-07-08T02:44:48+0000") == result);
}

/**
 * Africa/Cairo standard time is EET and daylight time is EEST. They no
 * longer use their DST zone but we should continue to parse it properly.
 */
int CTest::testObsoleteDstZoneName(int argc, char* argv[]) {
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lochelp);
    AutoPtr<ILocale> usloc;
    lochelp->GetUS((ILocale **) &usloc);

    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm zzzz"), usloc , (ISimpleDateFormat**)&format);

    AutoPtr<IDate> normal;
    format->Parse(String("1970-01-01T00:00 EET") , (IDate**)&normal);
    AutoPtr<IDate> dst;
PFL
    ECode ec = format->Parse(String("1970-01-01T00:00 EEST") , (IDate**)&dst);
    Int64 nortime , dsttime;
    PFL_EX("normal: %p , dst:%p, ec :%p", normal.Get(), dst.Get(), ec)
    normal->GetTime(&nortime);
    dst->GetTime(&dsttime);
    PFL_EX("expect: % lld , result : %lld", (Int64)(60 * 60 * 1000) , (nortime - dsttime))
    assert((Int64)60 * 60 * 1000 == (nortime - dsttime));
}

int CTest::testDstZoneNameWithNonDstTimestamp(int argc, char* argv[]) {
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lochelp);
    AutoPtr<ILocale> usloc;
    lochelp->GetUS((ILocale **) &usloc);
    AutoPtr<ITimeZoneHelper> zonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&zonehelp);
    AutoPtr<ITimeZone> AMERICA_LOS_ANGELES;
    zonehelp->GetTimeZone(String("America/Los_Angeles"), (ITimeZone**)&AMERICA_LOS_ANGELES);

    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm zzzz"), usloc ,(ISimpleDateFormat**)&format);

    AutoPtr<IGregorianCalendar> igc;
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_LOS_ANGELES , (IGregorianCalendar**)&igc);
    calendar = igc;
    AutoPtr<IDate> adate;
    format->Parse(String("2011-06-21T10:00 Pacific Standard Time") , (IDate**)&adate);
    calendar->SetDate(adate); // 18:00 GMT-8
    Int32 hod;
    calendar->Get(ICalendar::HOUR_OF_DAY, &hod);
    PFL_EX("11 == %d", hod)
    assert(11 == hod); // 18:00 GMT-7
    calendar->Get(ICalendar::MINUTE , &hod);
    assert(0 == hod);
}

int CTest::testNonDstZoneNameWithDstTimestamp(int argc, char* argv[]) {
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lochelp);
    AutoPtr<ILocale> usloc;
    lochelp->GetUS((ILocale **) &usloc);
    AutoPtr<ITimeZoneHelper> zonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&zonehelp);
    AutoPtr<ITimeZone> AMERICA_LOS_ANGELES;
    zonehelp->GetTimeZone(String("America/Los_Angeles"), (ITimeZone**)&AMERICA_LOS_ANGELES);

    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm zzzz"), usloc , (ISimpleDateFormat**)&format);
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_LOS_ANGELES , (IGregorianCalendar**)&calendar);
    AutoPtr<IDate> adate;
    format->Parse(String("2010-12-21T10:00 Pacific Daylight Time") , (IDate**)&adate);
    calendar->SetDate(adate); // 17:00 GMT-7
    Int32 hod;
    calendar->Get(ICalendar::HOUR_OF_DAY,&hod);
    PFL_EX("9 == %d " , hod)
    assert(9 == hod); // 17:00 GMT-8
    calendar->Get(ICalendar::MINUTE , &hod);
    assert(0 == hod);
}

// http://b/4723412
int CTest::testDstZoneWithNonDstTimestampForNonHourDstZone(int argc, char* argv[]) {
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lochelp);
    AutoPtr<ILocale> usloc;
    lochelp->GetUS((ILocale **) &usloc);
    AutoPtr<ITimeZoneHelper> zonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&zonehelp);
    AutoPtr<ITimeZone> AUSTRALIA_LORD_HOWE;
    zonehelp->GetTimeZone(String("Australia/Lord_Howe"), (ITimeZone**)&AUSTRALIA_LORD_HOWE);

    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm zzzz"), usloc , (ISimpleDateFormat**)&format);

    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AUSTRALIA_LORD_HOWE , (IGregorianCalendar**)&calendar);
    AutoPtr<IDate> adate;
    format->Parse(String("2011-06-21T20:00 Lord Howe Daylight Time"), (IDate**)&adate);
    calendar->SetDate(adate);  // 9:00 GMT+11
    Int32 hod;
    calendar->Get(ICalendar::HOUR_OF_DAY , &hod);
    PFL_EX("19 == %d " , hod)
    assert(19 == hod); // 9:00 GMT+10:30
    calendar->Get(ICalendar::MINUTE, &hod);
    PFL_EX("30 == %d" , hod)
    assert(30 == hod);
}

int CTest::testNonDstZoneWithDstTimestampForNonHourDstZone(int argc, char* argv[]) {
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lochelp);
    AutoPtr<ILocale> usloc;
    lochelp->GetUS((ILocale **) &usloc);
    AutoPtr<ITimeZoneHelper> zonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&zonehelp);
    AutoPtr<ITimeZone> AUSTRALIA_LORD_HOWE;
    zonehelp->GetTimeZone(String("Australia/Lord_Howe"), (ITimeZone**)&AUSTRALIA_LORD_HOWE);

    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm zzzz"), usloc , (ISimpleDateFormat**)&format);
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AUSTRALIA_LORD_HOWE, (IGregorianCalendar**)&calendar);
    AutoPtr<IDate> adate;
    format->Parse(String("2010-12-21T19:30 Lord Howe Standard Time"), (IDate**)&adate);

    calendar->SetDate(adate); //9:00 GMT+10:30
    Int32 hod;
    calendar->Get(ICalendar::HOUR_OF_DAY, &hod);
    PFL_EX("20 == %d", hod)
    assert(20 == hod); // 9:00 GMT+11:00
    calendar->Get(ICalendar::MINUTE, &hod);
    PFL_EX("0 == %d" , hod)
    assert(0 == hod);
}

int CTest::testLocales(int argc, char* argv[]) {
    // Just run through them all. Handy as a poor man's benchmark, and a sanity check.
    ArrayOf<ILocale*>* arrloc;
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochelp);
    lochelp->GetAvailableLocales(&arrloc);
    for (int i = 0; i < arrloc->GetLength(); i++) {
        AutoPtr<ILocale> l = (*arrloc)[i];
        AutoPtr<ISimpleDateFormat> sdf;
        CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss zzzz"), l , (ISimpleDateFormat**)&sdf);
        AutoPtr<IDate> adate;
        CDate::New(0,(IDate**)&adate);
        String str;
        sdf->FormatDate(adate,&str);
        PFL_EX("str:%s " ,str.string())
    }
}

// http://code.google.com/p/android/issues/detail?id=14963
int CTest::testParseTimezoneOnly(int argc, char* argv[]) {
    AutoPtr<ISimpleDateFormat> sdf1;
    AutoPtr<ISimpleDateFormat> sdf2;
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochelp);
    AutoPtr<ILocale> frloc, usloc;
    lochelp->GetFRANCE((ILocale**)&frloc);
    lochelp->GetUS((ILocale**)&usloc);
    CSimpleDateFormat::New(String("z"), frloc , (ISimpleDateFormat**)&sdf1);
    CSimpleDateFormat::New(String("z"), usloc , (ISimpleDateFormat**)&sdf2);
    AutoPtr<IDate> adate1;
    AutoPtr<IDate> adate2;
    sdf1->Parse(String("UTC") , (IDate**)&adate1);
    sdf2->Parse(String("UTC") , (IDate**)&adate2);
    PFL_EX("adate1: %s , adate2 :%s " ,adate1.Get() ,adate2.Get())

}

// http://code.google.com/p/android/issues/detail?id=36689
int CTest::testParseArabic(int argc, char* argv[]) {

    AutoPtr<ILocale> egloc;
    CLocale::New(String("ar") , String("EG") , (ILocale**)&egloc);
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss") , egloc ,(ISimpleDateFormat **) &sdf);
    AutoPtr<ITimeZoneHelper> tzh;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    AutoPtr<ITimeZone> tz;
    tzh->GetTimeZone(String("America/Los_Angeles"),(ITimeZone**)&tz);
    sdf->SetTimeZone(tz);

    // Can we parse an ASCII-formatted date in an Arabic locale?
    AutoPtr<IDate> d ;
    sdf->Parse(String("2012-08-29 10:02:45") , (IDate**)&d);
    Int64 miltime;
    d->GetTime(&miltime);
    PFL_EX("1346259765000L == %lld" , miltime)
    assert(1346259765000L == miltime);

    // Can we format a date correctly in an Arabic locale?
    String formatted ;
    sdf->FormatDate(d , &formatted);
    PFL_EX("佗贍佟佗-贍侉-佗侃 佟贍:贍佗:伽佶 == %s " , formatted.string())
    assert(String("佗贍佟佗-贍侉-佗侃 佟贍:贍佗:伽佶") == formatted);

    // Can we parse the Arabic-formatted date in an Arabic locale, and get the same date
    // we started with?
    AutoPtr<IDate> d2;
    sdf->Parse(formatted , (IDate**)&d2);
    PFL_EX("d : %p ,d2 :%p " , d.Get() , d2.Get())
    // assertEquals(d, d2);
}

