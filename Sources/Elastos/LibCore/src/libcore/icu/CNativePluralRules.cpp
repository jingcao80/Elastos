
#include "Elastos.CoreLibrary.Utility.h"
#include "CNativePluralRules.h"
#include <unicode/plurrule.h>

namespace Libcore {
namespace ICU {

CAR_OBJECT_IMPL(CNativePluralRules)

CAR_INTERFACE_IMPL(CNativePluralRules, Object, INativePluralRules)

CNativePluralRules::~CNativePluralRules()
{
    FinalizeImpl(mAddress);
}

ECode CNativePluralRules::constructor(
    /* [in] */ Int64 address)
{
    mAddress = address;
    return NOERROR;
}

AutoPtr<INativePluralRules> CNativePluralRules::ForLocale(
    /* [in] */ ILocale* locale)
{
    String s;
    locale->ToString(&s);
    AutoPtr<INativePluralRules> cres;
    CNativePluralRules::New(ForLocaleImpl(s), (INativePluralRules **)&cres);
    return cres;
}

ECode CNativePluralRules::QuantityForInt(
    /* [in] */ Int32 value,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)

    *result = QuantityForIntImpl(mAddress, value);
    return NOERROR;
}

static PluralRules* toPluralRules(
    /* [in] */ Int64 address)
{
    return reinterpret_cast<PluralRules*>(static_cast<uintptr_t>(address));
}

extern ECode maybeThrowIcuException(const char* provider, UErrorCode errorCode);

void CNativePluralRules::FinalizeImpl(
    /* [in] */ Int64 address)
{
    delete toPluralRules(address);
}

Int64 CNativePluralRules::ForLocaleImpl(
    /* [in] */ String& localeName)
{
    // The icu4c PluralRules returns a "other: n" default rule for the deprecated locales Java uses.
    // Work around this by translating back to the current language codes.
    if (localeName.StartWith("iw")) {
        localeName = String("he") + localeName.Substring(2);
    } else if (localeName.StartWith("in")) {
        localeName = String("id") + localeName.Substring(2);
    } else if (localeName.StartWith("ji")) {
        localeName = String("yi") + localeName.Substring(2);
    }

    Locale locale = Locale::createFromName(localeName.string());
    UErrorCode status = U_ZERO_ERROR;
    PluralRules* result = PluralRules::forLocale(locale, status);
    ASSERT_SUCCEEDED(maybeThrowIcuException("PluralRules::forLocale", status));
    return reinterpret_cast<uintptr_t>(result);
}

Int32 CNativePluralRules::QuantityForIntImpl(
    /* [in] */ Int64 address,
    /* [in] */ Int32 value)
{
    UnicodeString keyword = toPluralRules(address)->select(value);
    if (keyword == "zero") {
        return 0;
    }
    else if (keyword == "one") {
        return 1;
    }
    else if (keyword == "two") {
        return 2;
    }
    else if (keyword == "few") {
        return 3;
    }
    else if (keyword == "many") {
        return 4;
    }
    else {
        return 5;
    }
}

} // namespace ICU
} // namespace Libcore
