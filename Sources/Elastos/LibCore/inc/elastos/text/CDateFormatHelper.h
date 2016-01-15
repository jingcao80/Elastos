
#ifndef __ELASTOS_TEXT_CDATEFORMATHELPER_H__
#define __ELASTOS_TEXT_CDATEFORMATHELPER_H__

#include "_Elastos_Text_CDateFormatHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CDateFormatHelper)
    , public Singleton
    , public IDateFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*> ** locales);

    CARAPI GetDateInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateInstance(
        /* [in] */ Int32 style,
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateInstance(
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateTimeInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateTimeInstance(
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle,
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateTimeInstance(
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    CARAPI Set24HourTimePref(
        /* [in] */ Boolean bval);

    CARAPI GetInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetTimeInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetTimeInstance(
        /* [in] */ Int32 style,
        /* [out] */ IDateFormat** instance);

    CARAPI GetTimeInstance(
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDATEFORMATHELPER_H__
