
#ifndef __ELASTOS_TEXT_CCOLLATORHELPER_H__
#define __ELASTOS_TEXT_CCOLLATORHELPER_H__

#include "_Elastos_Text_CCollatorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CCollatorHelper)
    , public Singleton
    , public ICollatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetInstance(
        /* [out] */ ICollator** instance);

    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ ICollator** instance);
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CCOLLATORHELPER_H__
