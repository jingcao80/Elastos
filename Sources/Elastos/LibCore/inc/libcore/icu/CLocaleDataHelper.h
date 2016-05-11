
#ifndef __LIBCORE_ICU_ICU_CLOCALEDATAHELPER_H__
#define __LIBCORE_ICU_ICU_CLOCALEDATAHELPER_H__

#include "_Libcore_ICU_CLocaleDataHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CLocaleDataHelper)
    , public Singleton
    , public ILocaleDataHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI MapInvalidAndNullLocales(
        /* [in] */ ILocale* locale,
        /* [out] */ ILocale** rst);

    /**
     * Returns a shared LocaleData for the given locale.
     */
    CARAPI Get(
        /* [in] */ ILocale* locale,
        /* [out] */ ILocaleData** localeData);
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_ICU_CLOCALEDATAHELPER_H__
