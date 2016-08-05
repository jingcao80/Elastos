
#ifndef __ELASTOS_DROID_TRANSITION_CFADEHELPER_H__
#define __ELASTOS_DROID_TRANSITION_CFADEHELPER_H__

#include "_Elastos_Droid_Transition_CFadeHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CFadeHelper)
    , public Singleton
    , public IFadeHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetIN(
        /* [out] */ Int32* _in);

    CARAPI GetOUT(
        /* [out] */ Int32* _out);
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CFADEHELPER_H__
