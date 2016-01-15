
#include "CDateFormatSymbolsHelper.h"
#include "DateFormatSymbols.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CDateFormatSymbolsHelper, Singleton, IDateFormatSymbolsHelper)

CAR_SINGLETON_IMPL(CDateFormatSymbolsHelper)

ECode CDateFormatSymbolsHelper::GetInstance(
    /* [out] */ IDateFormatSymbols** instance)
{
    return DateFormatSymbols::GetInstance(instance);
}

ECode CDateFormatSymbolsHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormatSymbols** instance)
{
    return DateFormatSymbols::GetInstance(locale,instance);
}

ECode CDateFormatSymbolsHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** arrayOfLocales)
{
    return DateFormatSymbols::GetAvailableLocales(arrayOfLocales);
}

} // namespace Text
} // namespace Elastos