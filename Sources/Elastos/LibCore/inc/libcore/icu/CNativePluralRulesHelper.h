
#ifndef __LIBCORE_ICU_CNATIVEPLURALRULESHELPER_H__
#define __LIBCORE_ICU_CNATIVEPLURALRULESHELPER_H__

#include <Singleton.h>
#include "_Libcore_ICU_CNativePluralRulesHelper.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CNativePluralRulesHelper)
    , public Singleton
    , public INativePluralRulesHelper
{
    CAR_SINGLETON_DECL()

public:
    CAR_INTERFACE_DECL()

    CARAPI ForLocale(
        /* [in] */ ILocale * locale,
        /* [out] */ INativePluralRules ** outrules);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CNATIVEPLURALRULESHELPER_H__
