#include "test.h"

using namespace Elastos::Text;

CTest::CTest() {
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&cdh);
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lch);
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    tzh->GetTimeZone(String("America/Sao_Paulo") , (ITimeZone**)&AMERICA_SAO_PAULO);
    tzh->GetTimeZone(String("Australia/Lord_Howe") , (ITimeZone**)&AUSTRALIA_LORD_HOWE);
    tzh->GetTimeZone(String("Asia/Kuala_Lumpur") , (ITimeZone**)&ASIA_KUALA_LUMPUR);
    tzh->GetTimeZone(String("Asia/Seoul"),(ITimeZone**)&ASIA_SEOUL);
}

void assertCalendarEquals(AutoPtr<ICalendar> calendar, int year, int month, int day, int hour, int minute) {
    Int32 value = 0;
    calendar->Get(ICalendar::YEAR , &value);
    assert(year == value);
    calendar->Get(ICalendar::MONTH , &value);
    assert(month == value);
    calendar->Get(ICalendar::DATE , &value);
    assert(day == value);
    calendar->Get(ICalendar::HOUR_OF_DAY , &value);
    assert(hour == value);
    calendar->Get(ICalendar::MINUTE , &value);
    assert(minute == value);
}

double hoursSinceEpoch(AutoPtr<ICalendar> c) {
    double ONE_HOUR = 3600 * 1000;
    Int64 miltime = 0;
    c->GetTimeInMillis(&miltime);

    return miltime / ONE_HOUR;
}

void testSetSelfConsistent(ITimeZone * timeZone, int year, int month, int day) {
    int hour = 0;
    int minute = 0;
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(timeZone , (IGregorianCalendar**)&calendar);
    calendar->Clear();
    calendar->SetEx2(year, month, day, hour, minute);
    Int32 value = 0;
    calendar->Get(ICalendar::YEAR , &value);
    assert(year == value);
    calendar->Get(ICalendar::MONTH , &value);
    assert(month == value);
    calendar->Get(ICalendar::DAY_OF_MONTH , &value);
    assert(day == value);
    calendar->Get(ICalendar::HOUR_OF_DAY , &value);
    assert(hour == value);
    calendar->Get(ICalendar::MINUTE , &value);
    assert(minute == value);
}


int CTest::test_setTimeZone(int argc, char* argv[])
{
    // The specific time zones don't matter; they just have to be different so we can see that
    // get(Calendar.ZONE_OFFSET) returns the zone offset of the time zone passed to setTimeZone.
    AutoPtr<ICalendar> cal;
    AutoPtr<ILocale> locus;
    lch->GetUS((ILocale**)&locus);
    AutoPtr<ITimeZone> utczone;
    tzh->GetTimeZone(String("UTC") , (ITimeZone**)&utczone);
    cdh->GetInstanceEx3(utczone , locus ,(ICalendar**)&cal);
    Int32 value = 0;
    cal->Get(ICalendar::ZONE_OFFSET , &value);
    assert(0 == value);
    AutoPtr<ITimeZone> tz;
    tzh->GetTimeZone(String("GMT+7") , (ITimeZone**)&tz);
    cal->SetTimeZone(tz);
    cal->Get(ICalendar::ZONE_OFFSET , &value);
    assert(25200000 == value);

    return 0;
}

int CTest::testAddOneDayOverDstForwardAdds23HoursAt0100(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_SAO_PAULO , (IGregorianCalendar**)&calendar);
    Int32 offset = 0;
    AMERICA_SAO_PAULO->GetRawOffset(&offset);
    PFL_EX("offset : %d" , offset)
    calendar->SetEx2(2011, 9, 15, 1, 0); // 01:00 GMT-3
    Int32 value = 0;
    calendar->Get(ICalendar::HOUR , &value);
    PFL_EX("hour : %d" , value)
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);
    PFL_EX("hoursSinceEpoch1 : %f" , hoursSinceEpoch1)
    calendar->Add(ICalendar::DATE, 1);
    PFL_EX("hoursSinceEpoch2 : %f" , hoursSinceEpoch(calendar))
    PFL_EX("23.0 == %f" , (hoursSinceEpoch(calendar) - hoursSinceEpoch1))
    // assert(23.0 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    assertCalendarEquals(calendar, 2011, 9, 16, 1, 0); // 01:00 GMT-2; +23 hours

    return 0;
}

/**
 * At their daylight savings time switch, Sao Paulo changes from
 * "00:00 GMT-3" to "01:00 GMT-2". When adding time across this boundary,
 * drop an hour to keep the hour+minute constant unless that prevents the
 * date field from being incremented.
 * http://code.google.com/p/android/issues/detail?id=17502
 */
int CTest::testAddOneDayOverDstForwardAdds24HoursAt0000(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_SAO_PAULO , (IGregorianCalendar**)&calendar);
    calendar->SetEx2(2011, 9, 15, 0, 0); // 00:00 GMT-3
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);
    calendar->Add(ICalendar::DATE, 1);
    assert(24.0 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    // assertCalendarEquals(calendar, 2011, 9, 16, 1, 0); // 01:00 GMT-2; +24 hours

    return 0;
}

int CTest::testAddOneDayOverDstBackAdds25HoursAt0000(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_SAO_PAULO , (IGregorianCalendar**)&calendar);
    calendar->SetEx2(2011, 1, 19, 0, 0); // 00:00 GMT-2
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);

    calendar->Add(ICalendar::DATE, 1);
    PFL_EX("25.0 == %f" , (hoursSinceEpoch(calendar) - hoursSinceEpoch1))
    // assert(25.0 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    assertCalendarEquals(calendar, 2011, 1, 20, 0, 0); // 00:00 GMT-3; +25 hours

    return 0;
}

int CTest::testAddOneDayOverDstBackAdds25HoursAt0100(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_SAO_PAULO, (IGregorianCalendar**)&calendar);
    calendar->SetEx2(2011, 1, 19, 1, 0); // 00:00 GMT-2
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);
    calendar->Add(ICalendar::DATE, 1);
    PFL_EX("25.0 == %f" , (hoursSinceEpoch(calendar) - hoursSinceEpoch1))
    // assert(25.0 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    assertCalendarEquals(calendar, 2011, 1, 20, 1, 0); // 00:00 GMT-3; +25 hours

    return 0;
}

int CTest::testAddTwoHalfDaysOverDstForwardAdds23HoursAt0100(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_SAO_PAULO , (IGregorianCalendar**)&calendar);
    calendar->SetEx2(2011, 9, 15, 1, 0); // 01:00 GMT-3
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);
    calendar->Add(ICalendar::AM_PM, 2);
    PFL_EX("23.0 == %f" , (hoursSinceEpoch(calendar) - hoursSinceEpoch1))
    // assert(23.0 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    assertCalendarEquals(calendar, 2011, 9, 16, 1, 0); // 01:00 GMT-2; +23 hours

    return 0;
}

int CTest::testAdd24HoursOverDstForwardAdds24Hours(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AMERICA_SAO_PAULO , (IGregorianCalendar**)&calendar);
    calendar->SetEx2(2011, 9, 15, 1, 0); // 01:00 GMT-3
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);
    calendar->Add(ICalendar::HOUR, 24);
    PFL_EX("25.0 == %f" , (hoursSinceEpoch(calendar) - hoursSinceEpoch1))
    // assert(24.0 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    assertCalendarEquals(calendar, 2011, 9, 16, 2, 0); // 02:00 GMT-2; +24 hours

    return 0;
}

int CTest::testAddOneDayAndOneDayOver30MinuteDstForwardAdds48Hours(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AUSTRALIA_LORD_HOWE , (IGregorianCalendar**)&calendar);
    calendar->SetEx2(2011, 9, 1, 2, 10); // 02:10 GMT+10:30
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);
    calendar->Add(ICalendar::DATE, 1);
    calendar->Add(ICalendar::DATE, 1);
    // The RI fails this test by returning 47.0. It adjusts for DST on both of the add() calls!
    PFL_EX("48.0 == %f" , (hoursSinceEpoch(calendar) - hoursSinceEpoch1))
    // assert(48.0 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    assertCalendarEquals(calendar, 2011, 9, 3, 2, 40); // 02:40 GMT+11:00; +48.0 hours

    return 0;
}

int CTest::testAddTwoDaysOver30MinuteDstForwardAdds47AndAHalfHours(int argc, char* argv[])
{
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(AUSTRALIA_LORD_HOWE , (IGregorianCalendar**)&calendar);
    calendar->SetEx2(2011, 9, 1, 2, 10); // 02:10 GMT+10:30
    double hoursSinceEpoch1 = hoursSinceEpoch(calendar);
    calendar->Add(ICalendar::DATE, 2);
    PFL_EX("47.5 == %f" , (hoursSinceEpoch(calendar) - hoursSinceEpoch1))
    // assert(47.5 == (hoursSinceEpoch(calendar) - hoursSinceEpoch1));
    assertCalendarEquals(calendar, 2011, 9, 3, 2, 10); // 02:10 GMT+11:00; +47.5 hours

    return 0;
}

int CTest::testNewCalendarKoreaIsSelfConsistent(int argc, char* argv[])
{
    testSetSelfConsistent(ASIA_SEOUL, 1921, 0, 1);
    testSetSelfConsistent(ASIA_SEOUL, 1955, 0, 1);
    testSetSelfConsistent(ASIA_SEOUL, 1962, 0, 1);
    testSetSelfConsistent(ASIA_SEOUL, 2065, 0, 1);

    return 0;
}

int CTest::testSetTimeInZoneWhereDstIsNoLongerUsed(int argc, char* argv[])
{
    testSetSelfConsistent(ASIA_KUALA_LUMPUR, 1970, 0, 1);

    return 0;
}

int CTest::testCalendarSerialization(int argc, char* argv[])
{
    String s = String("aced00057372001b6a6176612e7574696c2e477265676f7269616e43616c656e6461728f3dd7d6e")
            + String("5b0d0c103000749000f63757272656e7459656172536b65774a0010677265676f7269616e4375746")
            + String("f7665725a000869734361636865644a00126c6173744d69646e696768744d696c6c697349000c6c6")
            + String("1737459656172536b65774a00126e6578744d69646e696768744d696c6c69735b000c63616368656")
            + String("44669656c64737400025b49787200126a6176612e7574696c2e43616c656e646172e6ea4d1ec8dc5")
            + String("b8e03000b5a000c6172654669656c647353657449000e66697273744461794f665765656b5a00096")
            + String("97354696d655365745a00076c656e69656e744900166d696e696d616c44617973496e46697273745")
            + String("765656b4900096e6578745374616d7049001573657269616c56657273696f6e4f6e53747265616d4")
            + String("a000474696d655b00066669656c647371007e00015b000569735365747400025b5a4c00047a6f6e6")
            + String("57400144c6a6176612f7574696c2f54696d655a6f6e653b787001000000010101000000040000000")
            + String("20000000100000000a15c9800757200025b494dba602676eab2a5020000787000000011000000010")
            + String("00007b20000000100000005000000010000000100000020000000010000000100000000000000000")
            + String("0000000000000000000000000000000fe488c0000000000757200025b5a578f203914b85de202000")
            + String("07870000000110101010101010101010101010101010101737200186a6176612e7574696c2e53696")
            + String("d706c6554696d655a6f6e65fa675d60d15ef5a603001049000a647374536176696e6773490006656")
            + String("e6444617949000c656e644461794f665765656b490007656e644d6f6465490008656e644d6f6e746")
            + String("8490007656e6454696d654900097261774f666673657449001573657269616c56657273696f6e4f6")
            + String("e53747265616d490008737461727444617949000e73746172744461794f665765656b49000973746")
            + String("172744d6f646549000a73746172744d6f6e7468490009737461727454696d6549000973746172745")
            + String("96561725a000b7573654461796c696768745b000b6d6f6e74684c656e6774687400025b427872001")
            + String("26a6176612e7574696c2e54696d655a6f6e6531b3e9f57744aca10200014c000249447400124c6a6")
            + String("176612f6c616e672f537472696e673b7870740009474d542d30383a30300036ee800000000000000")
            + String("001000000000000000000000000fe488c00000000010000000000000001000000000000000000000")
            + String("0000000000000757200025b42acf317f8060854e002000078700000000c1f1c1f1e1f1e1f1f1e1f1")
            + String("e1f7708000000040001000178780000000afffff4e2f964ac0001000000009fa5240000000000000")
            + String("00000a4cb7c187571007e00060000000a000007b2000000010000000100000001fe488c000000000")
            + String("10000000500000001000000200000000178");
    AutoPtr<ICalendar> calendar;
    CGregorianCalendar::New(1970, 1, 1, 0, 0, 0 , (IGregorianCalendar**)&calendar);
    AutoPtr<ITimeZone> gmtzone;
    tzh->GetTimeZone(String("GMT-08:00") , (ITimeZone**)&gmtzone);
    calendar->SetTimeZone(gmtzone);
    // Starting from ICU4.8 release, the default minimalDaysInFirstWeek changed from 4 to 1.
    calendar->SetMinimalDaysInFirstWeek(4);
    // new SerializationTester<Calendar>(calendar, s).test();

    return 0;
}

int CTest::test_timebug(int argc, char* argv[]) {
    AutoPtr<ICalendar> cal;
    AutoPtr<ICalendarHelper> calhelp;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calhelp);
    calhelp->GetInstance((ICalendar**)&cal);
    AutoPtr<ITimeZone> tz;
    cal->GetTimeZone((ITimeZone**)&tz);
    Int32 value;
    tz->GetRawOffset(&value);
    Int64 value2 = Elastos::Core::ISystem::GetCurrentTimeMillis();
    AutoPtr<IDate> adate;
    cal->GetDate((IDate**)&adate);
    String outstr;
    adate->ToString(&outstr);
    String format("yyyy-MM-dd HH:mm");
    AutoPtr<IDateFormat> sdf;
    CSimpleDateFormat::New(format, (ISimpleDateFormat**)&sdf);
    String result;
    sdf->FormatDate(adate, &result);
    PFL_EX("value:%d, value2:%lld, outstr:%s, result:%s", value, value2, outstr.string(), result.string());

}
