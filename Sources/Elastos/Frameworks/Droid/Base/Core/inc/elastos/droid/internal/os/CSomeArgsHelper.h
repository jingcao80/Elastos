
#ifndef __ELASTOS_DROID_INTERNAL_OS_CSOMEARGSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CSOMEARGSHELPER_H__

#include "_Elastos_Droid_Internal_Os_CSomeArgsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CSomeArgsHelper)
    , public Singleton
    , public ISomeArgsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [out] */ ISomeArgs** args);
};

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CSOMEARGSHELPER_H__
