
#ifndef __ELASTOS_UTILITY_CURRENCY_H__
#define __ELASTOS_UTILITY_CURRENCY_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::ILocale;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::ISerializable;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Utility::ILocale)

namespace Elastos{
namespace Utility{

/**
 * A currency corresponding to an <a href="http://en.wikipedia.org/wiki/ISO_4217">ISO 4217</a>
 * currency code such as "EUR" or "USD".
 */
class Currency
    : public Object
    , public ICurrency
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetCurrencyCode(
        /* [out] */ String* str);

    CARAPI GetDisplayName(
        /* [out] */ String* str);

    CARAPI GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* str);

    CARAPI GetSymbol(
        /* [out] */ String* str);

    CARAPI GetSymbol(
        /* [in] */ ILocale* locale,
        /* [out] */ String* str);

    CARAPI GetDefaultFractionDigits(
        /* [out] */ Int32* value);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI GetInstance(
        /* [in] */ const String& currencyCode,
        /* [out] */ ICurrency** instance);

    static CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ ICurrency** instance);

    static CARAPI GetAvailableCurrencies(
        /* [out] */ ISet** currencies);

private:
    Currency();

    CARAPI constructor(
        /* [in] */ const String& currencyCode);

    CARAPI_(AutoPtr<IInterface>) ReadResolve();

private:
    static HashMap<String, AutoPtr<ICurrency> > sCodesToCurrencies; // = new HashMap<String, Currency>();
    static HashMap<AutoPtr<ILocale>, AutoPtr<ICurrency> > sLocalesToCurrencies; // = new HashMap<Locale, Currency>();

    static Object sCodesLock;
    static Object sLocalesLock;

    String mCurrencyCode;

};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CURRENCY_H__
