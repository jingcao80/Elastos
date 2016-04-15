#include "test.h"
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;

CTest::CTest()
{
    CICUUtil::AcquireSingleton((IICUUtil**)&icuhep);
}

void assertNotNull(String str)
{
    if (!str.IsNull())
    {
        PFL_EX("expected~~~~")
    } else {
        PFL_EX("Unexpected~~~~")
    }

}

int CTest::test_getISOLanguages(int argc, char* argv[])
{
    // Check that corrupting our array doesn't affect other callers.
    AutoPtr<ArrayOf<String> > languages ;
    icuhep->GetISOLanguages((ArrayOf<String> **)&languages);

    assertNotNull((*languages)[0]);
    (*languages)[0] = String(NULL);
    icuhep->GetISOLanguages((ArrayOf<String> **)&languages);
    assertNotNull((*languages)[0]);
    return 0;
}

int CTest::test_getISOCountries(int argc, char* argv[])
{
    // Check that corrupting our array doesn't affect other callers.
    AutoPtr<ArrayOf<String> > countries ;
    icuhep->GetISOCountries((ArrayOf<String> **)&countries);
    assertNotNull((*countries)[0]);
    (*countries)[0] = String(NULL);
    icuhep->GetISOCountries((ArrayOf<String> **)&countries);
    assertNotNull((*countries)[0]);
    return 0;
}

int CTest::test_getAvailableLocales(int argc, char* argv[])
{
    // Check that corrupting our array doesn't affect other callers.
    AutoPtr<ArrayOf<ILocale*> > locales;
    icuhep->GetAvailableLocales((ArrayOf<ILocale *> **)&locales);
    if ((*locales)[0])
    {
        PFL_EX("expected ~~~")
    } else {
        PFL_EX("Unexpected ~~~")
    }
    (*locales)[0] = NULL;
    icuhep->GetAvailableLocales((ArrayOf<ILocale *> **)&locales);
    if ((*locales)[0])
    {
        PFL_EX("expected ~~~")
    } else {
        PFL_EX("Unexpected ~~~")
    }
    return 0;
}

void assertEqualsLocale(ILocale * one ,ILocale * two)
{
    Boolean flag = FALSE;
    one->Equals(two,&flag);
    PFL_EX("flag : %d " , flag)
    // assert(flag);
}

int CTest::test_localeFromString(int argc, char* argv[])
{
    // localeFromString is pretty lenient. Some of these can't be round-tripped
    // through Locale.toString.
    AutoPtr<ILocaleHelper> lochep;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochep);
    AutoPtr<ILocale> enloc;
    AutoPtr<ILocale> usloc;
    lochep->GetENGLISH((ILocale**)&enloc);
    lochep->GetUS((ILocale**)&usloc);

    AutoPtr<ILocale> enloc2;
    AutoPtr<ILocale> usloc2;
    icuhep->LocaleFromString(String("en") , (ILocale**)&enloc2);
    assertEqualsLocale(enloc ,  enloc2);
    icuhep->LocaleFromString(String("en_") , (ILocale**)&enloc2);
    assertEqualsLocale(enloc ,  enloc2);
    icuhep->LocaleFromString(String("en__") , (ILocale**)&enloc2);
    assertEqualsLocale(enloc ,  enloc2);

    icuhep->LocaleFromString(String("en_US") , (ILocale**)&usloc2);
    assertEqualsLocale(usloc ,  usloc2);
    icuhep->LocaleFromString(String("en_US_") , (ILocale**)&usloc2);
    assertEqualsLocale(usloc ,  usloc2);

    CLocale::New(String(""),String("US") , String("") ,(ILocale**)&usloc);
    icuhep->LocaleFromString(String("_US") , (ILocale**)&usloc2);
    assertEqualsLocale(usloc , usloc2);
    icuhep->LocaleFromString(String("_US_") , (ILocale**)&usloc2);
    assertEqualsLocale(usloc , usloc2);

    CLocale::New(String(""),String("") , String("POSIX"), (ILocale**)&usloc);
    icuhep->LocaleFromString(String("__POSIX") , (ILocale**)&usloc2);
    assertEqualsLocale(usloc , usloc2);
    CLocale::New(String("aa"),String("BB") , String("CC"), (ILocale**)&usloc);
    icuhep->LocaleFromString(String("aa_BB_CC") , (ILocale**)&usloc2);
    assertEqualsLocale(usloc , usloc2);

    return 0;
}

void assertEquals(String one, String two , AutoPtr<IICUUtil> icuhep)
{
    String addstr , scrstr;
    icuhep->AddLikelySubtags(String("en_US") , &addstr);
    icuhep->GetScript(addstr , &scrstr);
    Boolean flag = one.Equals(scrstr);
    PFL_EX("flag : %d " , flag)
    // assert( flag );
}

int CTest::test_getScript_addLikelySubtags(int argc, char* argv[])
{
    assertEquals(String("Latn"), String("en_US"),icuhep);
    assertEquals(String("Hebr"), String("he") , icuhep);
    assertEquals(String("Hebr"), String("he_IL") , icuhep);
    assertEquals(String("Hebr"), String("iw") , icuhep);
    assertEquals(String("Hebr"), String("iw_IL") , icuhep);
    return 0;
}

int CTest::test_11152(int argc, char* argv[])
{
    AutoPtr<ILocale> locale;
    String name;
    ECode ec = CLocale::New(String("en"), String("US"), String("POSIX"), (ILocale**)&locale);
    assert(ec == NOERROR);
    ec = locale->GetDisplayName(&name);
    assert(ec == NOERROR);
    PFL_EX("name : %s", name.string())

}
