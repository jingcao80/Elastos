#include "test.h"
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;

CTest::CTest()
{
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper **)&locdatahep);
    CLocaleHelper::AcquireSingleton((ILocaleHelper **)&lochep );
}

int CTest::testAll(int argc, char* argv[]) {
    // Test that we can get the locale data for all known locales.
    AutoPtr<ArrayOf<ILocale*> > arrloc;
    lochep->GetAvailableLocales((ArrayOf<ILocale*> **)&arrloc);
    for (int i = 0; i < arrloc->GetLength() ; i++ ) {

        AutoPtr<ILocaleData> d;
        locdatahep->Get((*arrloc)[i] , (ILocaleData **)&d);
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
    lochep->GetUS((ILocale **)&usloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(usloc , (ILocaleData **)&l);
    AutoPtr<ArrayOf<String> > amPm;
    l->GetAmPm((ArrayOf<String> **)&amPm);
    assert(String("AM").Equals((*amPm)[0]));

    l->GetEras((ArrayOf<String> **)&amPm);
    assert(String("BC").Equals((*amPm)[0]));

    l->GetLongMonthNames((ArrayOf<String> **)&amPm);
    assert(String("January").Equals((*amPm)[0]));

    l->GetShortMonthNames((ArrayOf<String> **)&amPm);
    assert(String("Jan").Equals((*amPm)[0]));

    l->GetTinyMonthNames((ArrayOf<String> **)&amPm);
    assert(String("J").Equals((*amPm)[0]));

    l->GetLongStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert(String("January").Equals((*amPm)[0]));

    l->GetShortStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert(String("Jan").Equals((*amPm)[0]));

    l->GetTinyStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert(String("J").Equals((*amPm)[0]));

    l->GetLongWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("Sunday").Equals((*amPm)[1]));

    l->GetShortWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("Sun").Equals((*amPm)[1]));

    l->GetTinyWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("S").Equals((*amPm)[1]));

    l->GetLongStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("Sunday").Equals((*amPm)[1]));

    l->GetShortStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("Sun").Equals((*amPm)[1]));

    l->GetTinyStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("S").Equals((*amPm)[1]));

    String yesterday, today,tomorrow;
    l->GetYesterday(&yesterday);
    l->GetToday(&today);
    l->GetTomorrow(&tomorrow);
    assert(String("Yesterday").Equals(yesterday));
    assert(String("Today").Equals(today));
    assert(String("Tomorrow").Equals(tomorrow));
}

int CTest::test_de_DE(int argc, char* argv[]) {
    AutoPtr<ILocale> deloc;
    CLocale::New(String("de") , String("DE") , (ILocale **)&deloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(deloc , (ILocaleData **)&l);
    String yesterday, today,tomorrow;
    l->GetYesterday(&yesterday);
    l->GetToday(&today);
    l->GetTomorrow(&tomorrow);
    assert(String("Gestern").Equals(yesterday));
    assert(String("Heute").Equals(today));
    assert(String("Morgen").Equals(tomorrow));
}

int CTest::test_cs_CZ(int argc, char* argv[]) {
    AutoPtr<ILocale> deloc;
    CLocale::New(String("cs") , String("CZ") , (ILocale **)&deloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(deloc , (ILocaleData **)&l);

    AutoPtr<ArrayOf<String> > amPm;
    l->GetLongMonthNames((ArrayOf<String> **)&amPm);
    assert(String("ledna").Equals((*amPm)[0]));
    l->GetShortMonthNames((ArrayOf<String> **)&amPm);
    assert(String("Led").Equals((*amPm)[0]));
    l->GetTinyMonthNames((ArrayOf<String> **)&amPm);
    assert(String("1").Equals((*amPm)[0]));

    l->GetLongStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert(String("leden").Equals((*amPm)[0]));
    l->GetShortStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert(String("1.").Equals((*amPm)[0]));
    l->GetTinyStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert(String("l").Equals((*amPm)[0]));
}

int CTest::test_ru_RU(int argc, char* argv[]) {
    AutoPtr<ILocale> deloc;
    CLocale::New(String("ru") , String("RU") , (ILocale **)&deloc);
    AutoPtr<ILocaleData> l;
    locdatahep->Get(deloc , (ILocaleData **)&l);

    AutoPtr<ArrayOf<String> > amPm;
    l->GetLongWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("胁芯褋泻褉械褋械薪褜械").Equals((*amPm)[1]));
    l->GetShortWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("胁褋").Equals((*amPm)[1]));
    l->GetTinyWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("袙").Equals((*amPm)[1]));

    // // Russian stand-alone weekday names get an initial capital.
    l->GetLongStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("袙芯褋泻褉械褋械薪褜械").Equals((*amPm)[1]));
    l->GetShortStandAloneWeekdayNames((ArrayOf<String> **)&amPm);
    assert(String("胁褋").Equals((*amPm)[1]));
    l->GetTinyStandAloneMonthNames((ArrayOf<String> **)&amPm);
    assert(String("袙").Equals((*amPm)[1]));
}
