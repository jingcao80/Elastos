
#include "CDecimalFormatSymbolsHelper.h"
#include "DecimalFormatSymbols.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CDecimalFormatSymbolsHelper, Singleton, IDecimalFormatSymbolsHelper)

CAR_SINGLETON_IMPL(CDecimalFormatSymbolsHelper)

ECode CDecimalFormatSymbolsHelper::GetInstance(
    /* [out] */ IDecimalFormatSymbols** instance)
{
    return DecimalFormatSymbols::GetInstance(instance);
}

ECode CDecimalFormatSymbolsHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IDecimalFormatSymbols** instance)
{
    return DecimalFormatSymbols::GetInstance(locale,instance);
}

ECode CDecimalFormatSymbolsHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return DecimalFormatSymbols::GetAvailableLocales(locales);
}

} // namespace Text
} // namespace Elastos