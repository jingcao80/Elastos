
#ifndef __ELASTOS_TEXT_CDATEFORMATSYMBOLSHELPER_H__
#define __ELASTOS_TEXT_CDATEFORMATSYMBOLSHELPER_H__

#include "_Elastos_Text_CDateFormatSymbolsHelper.h"
#include "Singleton.h"

using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CDateFormatSymbolsHelper)
    , public Singleton
    , public IDateFormatSymbolsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IDateFormatSymbols** instance);

    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormatSymbols** instance);

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** arrayOfLocales);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDATEFORMATSYMBOLSHELPER_H__
