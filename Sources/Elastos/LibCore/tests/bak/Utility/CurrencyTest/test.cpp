#include "test.h"

int CTest::test_getSymbol_fallback(int argc, char* argv[])
{
    PEL("CTest::test_getSymbol_fallback")
    // This assumes that AED never becomes a currency important enough to
    // Canada that Canadians give it a localized (to Canada) symbol.
    AutoPtr<ICurrencyHelper> ch;
    CCurrencyHelper::AcquireSingleton((ICurrencyHelper**)&ch);
    AutoPtr<ICurrency> cur;
    ch->GetInstance(String("AED"), (ICurrency**)&cur);
    AutoPtr<ILocale> canda;
    AutoPtr<ILocaleHelper> lh;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lh);
    lh->GetCANADA((ILocale**)&canda);
    String str;
    cur->GetSymbolEx(canda, &str);
    assert(String("AED") == str);
    return 0;
}

int CTest::test_getInstance_illegal_currency_code(int argc, char* argv[])
{
    PEL("CTest::test_getInstance_illegal_currency_code")
    AutoPtr<ICurrencyHelper> ch;
    CCurrencyHelper::AcquireSingleton((ICurrencyHelper**)&ch);
    AutoPtr<ICurrency> cur;
    ch->GetInstance(String("USD"), (ICurrency**)&cur);
    // try {
    ECode ec = ch->GetInstance(String("BOGO-DOLLARS"), (ICurrency**)&cur);
    if (ec != NOERROR) {
        printf("expected IllegalArgumentException for invalid ISO currency code\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }
    return 0;
}

int CTest::testGetAvailableCurrencies(int argc, char* argv[])
{
    PEL("CTest::testGetAvailableCurrencies")
    // Set<Currency> all = Currency.getAvailableCurrencies();

    // // Confirm that a few well-known stable currencies are present.
    // assertTrue(all.toString(), all.contains(Currency.getInstance("CHF")));
    // assertTrue(all.toString(), all.contains(Currency.getInstance("EUR")));
    // assertTrue(all.toString(), all.contains(Currency.getInstance("GBP")));
    // assertTrue(all.toString(), all.contains(Currency.getInstance("JPY")));
    // assertTrue(all.toString(), all.contains(Currency.getInstance("USD")));
    return 0;
}

int CTest::test_getDisplayName(int argc, char* argv[])
{
    PEL("CTest::test_getDisplayName")
    AutoPtr<ICurrencyHelper> ch;
    CCurrencyHelper::AcquireSingleton((ICurrencyHelper**)&ch);
    AutoPtr<ICurrency> cur;
    ch->GetInstance(String("CHF"), (ICurrency**)&cur);
    AutoPtr<ILocale> US;
    AutoPtr<ILocaleHelper> lh;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lh);
    lh->GetUS((ILocale**)&US);
    String str;
    cur->GetDisplayNameEx(US, &str);
    assert(String("Swiss Franc") == str);

    AutoPtr<ILocale> dech;
    CLocale::New(String("de"), String("CH"), (ILocale**)&dech);
    cur->GetDisplayNameEx(dech, &str);
    assert(String("Schweizer Franken") == str);

    CLocale::New(String("fr"), String("CH"), (ILocale**)&dech);
    cur->GetDisplayNameEx(dech, &str);
    assert(String("franc suisse") == str);

    CLocale::New(String("it"), String("CH"), (ILocale**)&dech);
    cur->GetDisplayNameEx(dech, &str);
    assert(String("Franco Svizzero") == str);

    // Test behavior with unknown locales; should return the currency code.
    CLocale::New(String("xx"), String("YY"), (ILocale**)&dech);
    cur->GetDisplayNameEx(dech, &str);
    assert(String("CHF") == str);
    return 0;
}

int CTest::test_getDefaultFractionDigits(int argc, char* argv[])
{
    PEL("CTest::test_getDefaultFractionDigits")
    AutoPtr<ICurrencyHelper> ch;
    CCurrencyHelper::AcquireSingleton((ICurrencyHelper**)&ch);
    AutoPtr<ICurrency> cur;
    ch->GetInstance(String("USD"), (ICurrency**)&cur);
    Int32 value = 0;
    cur->GetDefaultFractionDigits(&value);
    assert(2 == value);

    ch->GetInstance(String("JPY"), (ICurrency**)&cur);
    cur->GetDefaultFractionDigits(&value);
    assert(0 == value);

    ch->GetInstance(String("XXX"), (ICurrency**)&cur);
    cur->GetDefaultFractionDigits(&value);
    assert(-1 == value);

    return 0;
}
