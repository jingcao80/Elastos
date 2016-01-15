#include "test.h"

CTest::CTest()
{
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper **)&tzh);
    CLocaleHelper::AcquireSingleton((ILocaleHelper **)&lh);
}

int CTest::testGetHour(int argc, char* argv[])
{
    // Test for method java.util.GregorianCalendar(java.util.TimeZone, java.util.Locale)
    AutoPtr<IDate> now;
    CDate::New((IDate **)&now);
    AutoPtr<ITimeZone> gmt1;
    tzh->GetTimeZone(String("GMT+1") , (ITimeZone **)&gmt1);
    AutoPtr<ILocale> frloc;
    lh->GetFRANCE((ILocale **)&frloc);
    AutoPtr<IGregorianCalendar> gc1;
    CGregorianCalendar::New(gmt1 , frloc ,(IGregorianCalendar **)&gc1);
    gc1->SetDate(now);

    AutoPtr<ITimeZone> gmt2;
    tzh->GetTimeZone(String("GMT+2") , (ITimeZone **)&gmt2);
    AutoPtr<ILocale> usloc;
    lh->GetUS((ILocale **)&usloc);
    AutoPtr<IGregorianCalendar> gc2;
    CGregorianCalendar::New(gmt2 , usloc , (IGregorianCalendar **)&gc2);
    gc2->SetDate(now);

    // CST is 1 hour before EST, add 1 to the CST time and convert to 0-12 value
    Int32 value1 ,value2;
    gc2->Get(ICalendar::HOUR , &value2);
    gc1->Get(ICalendar::HOUR , &value1);
    assert(value2 == ((value1 + 1) % 12));

    return 0;
}

int CTest::test_computeTime(int argc, char* argv[])
{
    AutoPtr<ITimeZone> em;
    tzh->GetTimeZone(String("Europe/Moscow") , (ITimeZone **)&em);
    AutoPtr<IGregorianCalendar> g1;
    CGregorianCalendar::New(em , (IGregorianCalendar **)&g1);
    g1->Clear();
    g1->SetEx3(2006, -9, 29, 02, 50, 00); // transition from DST
    g1->SetLenient(FALSE);

    Int64 mil;
    ECode ec = g1->GetTimeInMillis(&mil);
    if (ec != NOERROR)
    {
        printf("expected ~~~~~~~~~\n");
    }

    return 0;
}

int CTest::test_computeFields(int argc, char* argv[])
{
    AutoPtr<ITimeZone> eltz;
    tzh->GetTimeZone(String("Europe/London"),(ITimeZone **)&eltz);
    AutoPtr<ILocale> enloc;
    CLocale::New(String("en") , String("GB") , (ILocale **)&enloc);
    AutoPtr<IGregorianCalendar> g;
    CGregorianCalendar::New(eltz , enloc , (IGregorianCalendar **)&g);

    g->Clear();
    g->SetTimeInMillis(1222185600225L);
    Int32 value;
    g->Get(ICalendar::ERA , &value);
    assert(1 == value);
    g->Get(ICalendar::YEAR , &value);
    assert(2008 == value);
    g->Get(ICalendar::MONTH , &value);
    assert(ICalendar::SEPTEMBER == value);
    g->Get(ICalendar::DAY_OF_MONTH , &value);
    assert(23 == value);
    g->Get(ICalendar::HOUR_OF_DAY , &value);
PFL_EX("17 == %d " , value)
    // assert(17 == value);
    g->Get(ICalendar::MINUTE , &value);
PFL_EX("0 == %d " , value)
    assert(0 == value);
    return 0;
}

int CTest::test_hashCode(int argc, char* argv[])
{
    // GregorianCalendar g = new GregorianCalendar(
    //         TimeZone.getTimeZone("Europe/London"),
    //         new Locale("en", "GB"));
    // g.clear();
    // g.setTimeInMillis(1222185600225L);

    // GregorianCalendar g1 = new GregorianCalendar(
    //         TimeZone.getTimeZone("Europe/Moscow"));
    // g1.clear();
    // g1.set(2008, Calendar.SEPTEMBER, 23, 18, 0, 0);
    // assertNotSame(g.hashCode(), g1.hashCode());
    return 0;
}

int CTest::test_setFirstDayOfWeekI(int argc, char* argv[])
{
    AutoPtr<ITimeZone> elloc;
    tzh->GetTimeZone(String("Europe/London") , (ITimeZone **)&elloc);
    AutoPtr<ILocale> enloc;
    CLocale::New(String("en") , String("GB") , (ILocale **)&enloc);
    AutoPtr<IGregorianCalendar> g;
    CGregorianCalendar::New(elloc , enloc , (IGregorianCalendar **)&g);

    for (int i = 0; i < 10; i++) {
        g->SetFirstDayOfWeek(i);
        Int32 value;
        g->GetFirstDayOfWeek(&value);
        assert(i == value);
    }
    g->SetLenient(FALSE);
    g->SetFirstDayOfWeek(10);
    g->SetFirstDayOfWeek(-10);
    return 0;
}

int CTest::test_setMinimalDaysInFirstWeekI(int argc, char* argv[])
{
    AutoPtr<ITimeZone> elloc;
    tzh->GetTimeZone(String("Europe/London") , (ITimeZone **)&elloc);
    AutoPtr<ILocale> enloc;
    CLocale::New(String("en") , String("GB") , (ILocale **)&enloc);
    AutoPtr<IGregorianCalendar> g;
    CGregorianCalendar::New(elloc , enloc , (IGregorianCalendar **)&g);

    for (int i = 0; i < 10; i++) {
        g->SetMinimalDaysInFirstWeek(i);
        Int32 value;
        g->GetMinimalDaysInFirstWeek(&value);
        assert(i == value);
    }
    g->SetLenient(FALSE);
    g->SetMinimalDaysInFirstWeek(10);
    g->SetMinimalDaysInFirstWeek(-10);
    return 0;
}

