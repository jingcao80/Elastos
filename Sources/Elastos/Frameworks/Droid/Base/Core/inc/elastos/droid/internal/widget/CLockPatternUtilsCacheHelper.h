
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHEHELPER_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHEHELPER_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternUtilsCacheHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternUtilsCacheHelper)
    , public Singleton
    , public ILockPatternUtilsCacheHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetInstance(
        /* [in] */ IILockSettings* service,
        /* [out] */ ILockPatternUtilsCache** instance);
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHEHELPER_H__
