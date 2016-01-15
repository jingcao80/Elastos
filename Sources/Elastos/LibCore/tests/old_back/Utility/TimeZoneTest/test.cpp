#include "test.h"

CTest::CTest()
{
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper **)&tzh);
    CLocaleHelper::AcquireSingleton((ILocaleHelper **)&lh);
}

int CTest::test_useDaylightTime_Taiwan(int argc, char* argv[])
{
    AutoPtr<ITimeZone> asiaTaipei;
    tzh->GetTimeZone(String("Asia/Taipei") , (ITimeZone **)&asiaTaipei);
    Boolean flag = FALSE;
    asiaTaipei->UseDaylightTime(&flag);
    assert(!flag);
    return 0;
}

int CTest::test_useDaylightTime_Iceland(int argc, char* argv[])
{
    AutoPtr<ITimeZone> atlanticReykjavik;
    tzh->GetTimeZone(String("Atlantic/Reykjavik") , (ITimeZone **)&atlanticReykjavik);
    Boolean flag = FALSE;
    atlanticReykjavik->UseDaylightTime(&flag);
    assert(flag);
    return 0;
}

int CTest::test_clone_SimpleTimeZone(int argc, char* argv[])
{
    AutoPtr<ISimpleTimeZone> stz;
    CSimpleTimeZone::New(21600000, String("Central Standard Time") , (ISimpleTimeZone **)&stz);
    stz->SetStartYear(1000);
    AutoPtr<IDate> now;
    CDate::New((IDate **)&now);
    Boolean flag =  FALSE;
    stz->InDaylightTime(now , &flag);
    AutoPtr<ISimpleTimeZone> stzclone;
    stz->Clone((ITimeZone **)&stzclone);
    return 0;
}

int CTest::testCustomTimeZoneDisplayNames(int argc, char* argv[])
{
    AutoPtr<ITimeZone> tz0001;
    CSimpleTimeZone::New(60000, String("ONE MINUTE") , (ISimpleTimeZone **)&tz0001);
    AutoPtr<ITimeZone> tz0130;
    CSimpleTimeZone::New(5400000, String("ONE HOUR, THIRTY") , (ISimpleTimeZone **)&tz0130);
    AutoPtr<ITimeZone> tzMinus0130;
    CSimpleTimeZone::New(-5400000, String("NEG ONE HOUR, THIRTY") , (ISimpleTimeZone **)&tzMinus0130);
    AutoPtr<ILocale> usloc;
    lh->GetUS((ILocale **)&usloc);
    String outstr;
    tz0001->GetDisplayNameEx3(FALSE , ITimeZone::SHORT , usloc , &outstr);
    assert(String("GMT+00:01") == outstr);
    tz0130->GetDisplayNameEx3(FALSE , ITimeZone::SHORT , usloc , &outstr);
    assert(String("GMT+01:30") == outstr);
    tzMinus0130->GetDisplayNameEx3(FALSE , ITimeZone::SHORT , usloc , &outstr);
    assert(String("GMT-01:30") == outstr);
    return 0;
}

int CTest::testPreHistoricInDaylightTime(int argc, char* argv[])
{
    AutoPtr<ITimeZone> tz;
    tzh->GetTimeZone(String("America/Los_Angeles") , (ITimeZone **)&tz);
    tzh->SetDefault(tz);
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm:ss.SSSZ") , (ISimpleDateFormat **)&sdf);
    AutoPtr<IDate> date;
    sdf->Parse(String("1902-11-01T00:00:00.000+0800") , (IDate **)&date);
    Int64 nowtime;
    date->GetTime(&nowtime);
    assert(-2119680000000L == nowtime);
    Int32 offset;
    tz->GetOffset(nowtime , &offset);
    assert(-28800000 == offset);
    Boolean flag = FALSE;
    tz->InDaylightTime(date , &flag);
    assert(!flag);
    String outstr;
    date->ToString(&outstr);
    PFL_EX(" Fri Oct 31 08:00:00 PST 1902 == %s" , outstr.string())
    // assert(String("Fri Oct 31 08:00:00 PST 1902") == outstr);
    date->ToGMTString(&outstr);
    PFL_EX("31 Oct 1902 16:00:00 GMT == %s" , outstr.string())
    // assert(String("31 Oct 1902 16:00:00 GMT") == outstr);
    // Any time before we have transition data is considered non-daylight, even in summer.
    sdf->Parse(String("1902-06-01T00:00:00.000+0800") , (IDate **)&date);
    date->GetTime(&nowtime);
    tz->GetOffset(nowtime , &offset);
    assert(-28800000 == offset);
    tz->InDaylightTime(date , &flag);
    assert(!flag);
    return 0;
}

int CTest::testPreHistoricOffsets(int argc, char* argv[])
{
    // The "Asia/Saigon" time zone has just a few transitions, and hasn't changed in a
    // long time, which is convenient for testing:
    //
    // libcore.util.ZoneInfo[Asia/Saigon,mRawOffset=25200000,mUseDst=false]
    // 0 : time=-2005974400 Fri Jun 08 16:53:20 1906 GMT+00:00 = Fri Jun 08 23:59:40 1906 ICT isDst=0 offset=  380 gmtOffset=25580
    // 1 : time=-1855983920 Fri Mar 10 16:54:40 1911 GMT+00:00 = Fri Mar 10 23:54:40 1911 ICT isDst=0 offset=    0 gmtOffset=25200
    // 2 : time=-1819954800 Tue Apr 30 17:00:00 1912 GMT+00:00 = Wed May 01 01:00:00 1912 ICT isDst=0 offset= 3600 gmtOffset=28800
    // 3 : time=-1220428800 Thu Apr 30 16:00:00 1931 GMT+00:00 = Thu Apr 30 23:00:00 1931 ICT isDst=0 offset=    0 gmtOffset=25200
    AutoPtr<ITimeZone> tz;
    tzh->GetTimeZone(String("Asia/Saigon") , (ITimeZone **)&tz);

    // Times before our first transition should assume we're still following that transition.
    // Note: the RI reports 25600 here because it has more transitions than we do.
    assertNonDaylightOffset(25580, -2005975000L, tz);

    assertNonDaylightOffset(25580, -2005974400L, tz); // 0
    assertNonDaylightOffset(25580, -2005974000L, tz);

    assertNonDaylightOffset(25200, -1855983920L, tz); // 1
    assertNonDaylightOffset(25200, -1855983900L, tz);

    assertNonDaylightOffset(28800, -1819954800L, tz); // 2
    assertNonDaylightOffset(28800, -1819954000L, tz);

    assertNonDaylightOffset(25200, -1220428800L, tz); // 3

    // Times after out last transition should assume we're still following that transition.
    assertNonDaylightOffset(25200, -1220428000L, tz);

    // There are plenty more examples. "Africa/Bissau" is one:
    //
    // libcore.util.ZoneInfo[Africa/Bissau,mRawOffset=0,mUseDst=false]
    // 0 : time=-1849388260 Fri May 26 01:02:20 1911 GMT+00:00 = Fri May 26 00:02:20 1911 GMT isDst=0 offset=-3600 gmtOffset=-3600
    // 1 : time=  157770000 Wed Jan 01 01:00:00 1975 GMT+00:00 = Wed Jan 01 01:00:00 1975 GMT isDst=0 offset=    0 gmtOffset=0
    tzh->GetTimeZone(String("Africa/Bissau"),(ITimeZone **)&tz);
    assertNonDaylightOffset(-3600, -1849388300L, tz);
    assertNonDaylightOffset(-3600, -1849388260L, tz); // 0
    assertNonDaylightOffset(-3600, -1849388200L, tz);
    assertNonDaylightOffset(0, 157770000L, tz); // 1
    assertNonDaylightOffset(0, 157780000L, tz);
    return 0;
}

int CTest::testZeroTransitionZones(int argc, char* argv[])
{
    // Zones with no transitions historical or future seem ideal for testing.
    String ids[4] ;
    ids[0] = String("Africa/Bujumbura");
    ids[1] = String("Indian/Cocos");
    ids[2] = String("Pacific/Wake");
    ids[3] = String("UTC");
    for (int i = 0 ; i < 4 ; ++i) {
        String id = ids[i];
        AutoPtr<ITimeZone> tz;
        tzh->GetTimeZone(id , (ITimeZone **)&tz);
        Boolean flag;
        tz->UseDaylightTime(&flag);
        assert(!flag);
        AutoPtr<IDate> mindate , zerodate , maxdate;
        Int32 MIN_VALUE = 0x80000000;
        Int32 MAX_VALUE = 0x7FFFFFFF;
        CDate::New(MIN_VALUE , (IDate **)&mindate);
        tz->InDaylightTime(mindate , &flag);
        assert(!flag);
        CDate::New(0,(IDate **)&zerodate);
        tz->InDaylightTime(zerodate , &flag);
        assert(!flag);
        CDate::New(MAX_VALUE , (IDate **)&maxdate);
        tz->InDaylightTime(maxdate , &flag);
        assert(!flag);
        Int32 currentOffset;
        Int64 nowtime;
        zerodate->GetTime(&nowtime);
        tz->GetOffset(nowtime , &currentOffset);
        Int64 mintime, maxtime;
        Int32 minvalue, maxvalue;
        mindate->GetTime(&mintime);
        maxdate->GetTime(&maxtime);
        tz->GetOffset(mintime , &minvalue);
        tz->GetOffset(maxtime , &maxvalue);
        assert(currentOffset == minvalue);
        assert(currentOffset == maxvalue);
    }
    return 0;
}

int CTest::testHelsinkiOverflow(int argc, char* argv[])
{
    Int64 time = 2147483648000L;// Tue, 19 Jan 2038 03:14:08 GMT
    AutoPtr<ITimeZone> timeZone;
    tzh->GetTimeZone(String("Europe/Helsinki"), (ITimeZone **)&timeZone);
    Int32 offset;
    timeZone->GetOffset(time , &offset);
    // This might cause us trouble if Europe/Helsinki changes its rules. See the bug for
    // details of the intent of this test.
    assert(7200000 == offset);
    return 0;
}

int CTest::testHasSameRules(int argc, char* argv[])
{
    AutoPtr<ITimeZone> denver;
    tzh->GetTimeZone(String("America/Denver"),(ITimeZone **)&denver);
    AutoPtr<ITimeZone> phoenix;
    tzh->GetTimeZone(String("America/Phoenix") , (ITimeZone **)&phoenix);
    Boolean flag;
    denver->HasSameRules(phoenix , &flag);
    assert(!flag);
    return 0;
}

int CTest::testNullId(int argc, char* argv[])
{
    AutoPtr<ITimeZone> nullzone;
    ECode ec = tzh->GetTimeZone(String(NULL), (ITimeZone **)&nullzone);
    if (ec != NOERROR)
    {
        printf(" expected ~~~~~~~~\n");
    }
    return 0;
}

int CTest::testCustomZoneIds(int argc, char* argv[])
{
    // These are all okay (and equivalent).
    AutoPtr<ITimeZone> gmtzone;
    tzh->GetTimeZone(String("GMT+05:00") , (ITimeZone **)&gmtzone);
    String str;
    gmtzone->GetID(&str);
    assert(String("GMT+05:00") == str);

    tzh->GetTimeZone(String("GMT+5:00") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT+05:00") == str);

    tzh->GetTimeZone(String("GMT+0500") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT+05:00") == str);

    tzh->GetTimeZone(String("GMT+500") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT+05:00") == str);

    tzh->GetTimeZone(String("GMT+5") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT+05:00") == str);

    // These aren't.
    tzh->GetTimeZone(String("GMT+5.5") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5:5") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5:0") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5:005") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5:000") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+005:00") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+05:99") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+28:00") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+05:00.00") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+05:00:00") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5:") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+junk") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5junk") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5:junk") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("GMT+5:00junk") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("junkGMT+5:00") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("junk") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    tzh->GetTimeZone(String("gmt+5:00") , (ITimeZone **)&gmtzone);
    gmtzone->GetID(&str);
    assert(String("GMT") == str);

    return 0;
}

int CTest::test_getDSTSavings(int argc, char* argv[])
{
    Int32 value;
    AutoPtr<ITimeZone> utczone;
    tzh->GetTimeZone(String("UTC") , (ITimeZone **)&utczone);
    utczone->GetDSTSavings(&value);
    assert(0 == value);

    tzh->GetTimeZone(String("America/Los_Angeles") , (ITimeZone **)&utczone);
    utczone->GetDSTSavings(&value);
    assert(3600000 == value);

    tzh->GetTimeZone(String("Australia/Lord_Howe") , (ITimeZone **)&utczone);
    utczone->GetDSTSavings(&value);
    assert(1800000 == value);
    return 0;
}

int CTest::testSimpleTimeZoneDoesNotCallOverrideableMethodsFromConstructor(int argc, char* argv[])
{
    // new SimpleTimeZone(0, "X", Calendar.MARCH, 1, 1, 1, Calendar.SEPTEMBER, 1, 1, 1) {
    //     @Override public void setStartRule(int m, int d, int dow, int time) {
    //         fail();
    //     }
    //     @Override public void setStartRule(int m, int d, int dow, int time, boolean after) {
    //         fail();
    //     }
    //     @Override public void setEndRule(int m, int d, int dow, int time) {
    //         fail();
    //     }
    //     @Override public void setEndRule(int m, int d, int dow, int time, boolean after) {
    //         fail();
    //     }
    // };
    return 0;
}

void CTest::assertNonDaylightOffset(int expectedOffsetSeconds, long epochSeconds, AutoPtr<ITimeZone> tz)
{
    Int32 offset ;
    tz->GetOffset(epochSeconds * 1000 , &offset);
    PFL_EX("%d == %d " , expectedOffsetSeconds * 1000 , offset)
    // assert(expectedOffsetSeconds * 1000 == offset);
    AutoPtr<IDate> nowdate;
    CDate::New(epochSeconds * 1000 , (IDate **)&nowdate);
    Boolean flag;
    tz->InDaylightTime(nowdate , &flag);
    assert(!flag);
}

int CTest::test_11129(int argc, char* argv[])
{
    AutoPtr<ITimeZoneHelper> helper;
    AutoPtr<ITimeZone> timeZone;
    String name;
    ECode ec;
    ec = CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    assert(ec == NOERROR);
    ec = helper->GetDefault((ITimeZone**)&timeZone);
    assert(ec == NOERROR);
    ec = timeZone->GetDisplayName(&name);
    assert(ec == NOERROR);
    PFL_EX("name: %s", name.string())
    return 0;
}
