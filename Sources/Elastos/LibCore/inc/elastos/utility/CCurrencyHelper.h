
#ifndef __ELASTOS_UTILITY_CCURRENCYHELPER_H__
#define __ELASTOS_UTILITY_CCURRENCYHELPER_H__

#include "_Elastos_Utility_CCurrencyHelper.h"
#include "Singleton.h"

using Elastos::Utility::ILocale;

namespace Elastos{
namespace Utility{

CarClass(CCurrencyHelper)
    , public Singleton
    , public ICurrencyHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& currencyCode,
        /* [out] */ ICurrency** outcur);

    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ ICurrency** outcur);

    CARAPI GetAvailableCurrencies(
        /* [out] */ ISet** currencies);
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CCURRENCYHELPER_H__
