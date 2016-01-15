
#ifndef __ELASTOS_TEXT_CDECIMALFORMATSYMBOLSHELPER_H__
#define __ELASTOS_TEXT_CDECIMALFORMATSYMBOLSHELPER_H__

#include "_Elastos_Text_CDecimalFormatSymbolsHelper.h"
#include <Elastos.CoreLibrary.h>
#include "Singleton.h"

using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CDecimalFormatSymbolsHelper)
    , public Singleton
    , public IDecimalFormatSymbolsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IDecimalFormatSymbols** instance);

    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ IDecimalFormatSymbols** instance);

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDECIMALFORMATSYMBOLSHELPER_H__
