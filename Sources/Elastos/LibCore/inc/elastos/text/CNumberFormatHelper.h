
#ifndef __ELASTOS_TEXT_CNUMBERFORMATHELPER_H__
#define __ELASTOS_TEXT_CNUMBERFORMATHELPER_H__

#include "_Elastos_Text_CNumberFormatHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CNumberFormatHelper)
    , public Singleton
    , public INumberFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetCurrencyInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetCurrencyInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetIntegerInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetIntegerInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetNumberInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetNumberInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetPercentInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetPercentInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CNUMBERFORMATHELPER_H__
