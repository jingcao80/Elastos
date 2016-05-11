
#ifndef __LIBCORE_ICU_CNATIVEPLURALRULES_H__
#define __LIBCORE_ICU_CNATIVEPLURALRULES_H__

#include <Object.h>
#include "_Libcore_ICU_CNativePluralRules.h"

using Elastos::Core::Object;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CNativePluralRules)
    , public Object
    , public INativePluralRules
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI QuantityForInt(
        /* [in] */ Int32 value,
        /* [out] */ Int32 * result);

    static CARAPI_(AutoPtr<INativePluralRules>) ForLocale(
        /* [in] */ ILocale* locale);

    CARAPI constructor(
        /* [in] */ Int64 address);

    ~CNativePluralRules();

private:
    static CARAPI_(void) FinalizeImpl(
        /* [in] */ Int64 address);

    static CARAPI_(Int64) ForLocaleImpl(
        /* [in] */ String& localeName);

    static CARAPI_(Int32) QuantityForIntImpl(
        /* [in] */ Int64 address,
        /* [in] */ Int32 value);

private:
    Int64 mAddress;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CNATIVEPLURALRULES_H__
