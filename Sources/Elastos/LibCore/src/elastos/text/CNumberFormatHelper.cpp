#include "CNumberFormatHelper.h"
#include "NumberFormat.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CNumberFormatHelper, Singleton, INumberFormatHelper)

CAR_SINGLETON_IMPL(CNumberFormatHelper)

ECode CNumberFormatHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return NumberFormat::GetAvailableLocales(locales);
}

ECode CNumberFormatHelper::GetCurrencyInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetCurrencyInstance(instance);
}

ECode CNumberFormatHelper::GetCurrencyInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetCurrencyInstance(locale,instance);
}

ECode CNumberFormatHelper::GetIntegerInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetIntegerInstance(instance);
}

ECode CNumberFormatHelper::GetIntegerInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetIntegerInstance(locale,instance);
}

ECode CNumberFormatHelper::GetInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetInstance(instance);
}

ECode CNumberFormatHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetInstance(locale,instance);
}

ECode CNumberFormatHelper::GetNumberInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetNumberInstance(instance);
}

ECode CNumberFormatHelper::GetNumberInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetNumberInstance(locale,instance);
}

ECode CNumberFormatHelper::GetPercentInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetPercentInstance(instance);
}

ECode CNumberFormatHelper::GetPercentInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetPercentInstance(locale,instance);
}

} // namespace Text
} // namespace Elastos