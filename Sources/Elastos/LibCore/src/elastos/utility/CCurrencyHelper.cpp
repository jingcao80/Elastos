
#include "CCurrencyHelper.h"
#include "Currency.h"

namespace Elastos{
namespace Utility{

CAR_SINGLETON_IMPL(CCurrencyHelper)

CAR_INTERFACE_IMPL(CCurrencyHelper, Singleton, ICurrencyHelper)

ECode CCurrencyHelper::GetInstance(
    /* [in] */ const String& currencyCode,
    /* [out] */ ICurrency** outcur)
{
    VALIDATE_NOT_NULL(outcur)

    AutoPtr<ICurrency> cur = Currency::GetInstance(currencyCode);
    *outcur = cur;
    REFCOUNT_ADD(*outcur)
    return NOERROR;
}

ECode CCurrencyHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ ICurrency** outcur)
{
    return Currency::GetInstance(locale, outcur);
}

ECode CCurrencyHelper::GetAvailableCurrencies(
    /* [out] */ ISet** currencies)
{
    return Currency::GetAvailableCurrencies(currencies);
}

} // namespace Utility
} // namespace Elastos
