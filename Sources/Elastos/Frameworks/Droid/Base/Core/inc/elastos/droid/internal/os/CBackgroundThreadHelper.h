
#ifndef __ELASTOS_DROID_INTERNAL_OS_CBACKGROUNDTHREADHELPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBACKGROUNDTHREADHELPER_H__

#include "_Elastos_Droid_Internal_Os_CBackgroundThreadHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CBackgroundThreadHelper)
    , public Singleton
    , public IBackgroundThreadHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IBackgroundThread** instance);

    CARAPI GetHandler(
        /* [out] */ IHandler** handler);
};

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CBACKGROUNDTHREADHELPER_H__
