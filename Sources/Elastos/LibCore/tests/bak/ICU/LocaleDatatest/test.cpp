#include "test.h"
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;

CTest::CTest()
{
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&locdatahep);
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochep );
}

int CTest::testAll(int argc, char* argv[]) {
    // Test that we can get the locale data for all known locales.
    AutoPtr<ArrayOf<ILocale*> > arrloc;
    lochep->GetAvailableLocales((ArrayOf<ILocale*> **)&arrloc);
    for (int i = 0; i < arrloc->GetLength() ; i++ ) {

        AutoPtr<ILocaleData> d;
        locdatahep->Get((*arrloc)[i] , (ILocaleData**)&d);
        String yesterday, today,tomorrow;
        d->GetYesterday(&yesterday);
        d->GetToday(&today);
        d->GetTomorrow(&tomorrow);
        PFL_EX("yesterday : %s , today : %s , tomorrow : %s " , yesterday.string() ,  today.string() ,  tomorrow.string())
    }
    return 0;
}

int CTest::test_en_US(int argc, char* argv[]) {
    AutoPtr<ILocale> usloc;
    lochep->GetUS((ILocale**)&usloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(usloc , (ILocaleData**)&l);
    AutoPtr<ArrayOf<String> > amPm;
    l->GetAmPm((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("AM"));

    l->GetEras((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("BC"));

    l->GetLongMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("January"));

    l->GetShortMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("Jan"));

    l->GetTinyMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("J"));

    l->GetLongStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("January"));

    l->GetShortStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("Jan"));

    l->GetTinyStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("J"));

    l->GetLongWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("Sunday"));

    l->GetShortWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("Sun"));

    l->GetTinyWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("S"));

    l->GetLongStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("Sunday"));

    l->GetShortStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("Sun"));

    l->GetTinyStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("S"));

    String yesterday, today,tomorrow;
    l->GetYesterday(&yesterday);
    l->GetToday(&today);
    l->GetTomorrow(&tomorrow);
    assert(yesterday.Equals("Yesterday"));
    assert(today.Equals("Today"));
    assert(tomorrow.Equals("Tomorrow"));
}

int CTest::test_de_DE(int argc, char* argv[]) {
    AutoPtr<ILocale> deloc;
    CLocale::New(String("de") , String("DE") , (ILocale**)&deloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(deloc , (ILocaleData**)&l);
    String yesterday, today,tomorrow;
    l->GetYesterday(&yesterday);
    l->GetToday(&today);
    l->GetTomorrow(&tomorrow);
    assert(yesterday.Equals("Gestern"));
    assert(today.Equals("Heute"));
    assert(tomorrow.Equals("Morgen"));
}

int CTest::test_cs_CZ(int argc, char* argv[]) {
    AutoPtr<ILocale> deloc;
    CLocale::New(String("cs") , String("CZ") , (ILocale**)&deloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(deloc , (ILocaleData**)&l);

    AutoPtr<ArrayOf<String> > amPm;
    l->GetLongMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("ledna"));
    l->GetShortMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("Led"));
    l->GetTinyMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("1"));

    l->GetLongStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("leden"));
    l->GetShortStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("1."));
    l->GetTinyStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[0].Equals("l"));
}

int CTest::test_ru_RU(int argc, char* argv[]) {
    AutoPtr<ILocale> deloc;
    CLocale::New(String("ru") , String("RU") , (ILocale**)&deloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(deloc , (ILocaleData**)&l);

    AutoPtr<ArrayOf<String> > amPm;
    l->GetLongWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("胁芯褋泻褉械褋械薪褜械"));
    l->GetShortWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("胁褋"));
    l->GetTinyWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("袙"));

    // // Russian stand-alone weekday names get an initial capital.
    l->GetLongStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("袙芯褋泻褉械褋械薪褜械"));
    l->GetShortStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("胁褋"));
    l->GetTinyStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert((*amPm)[1].Equals("袙"));
}
