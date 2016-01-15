
#ifndef __ELASTOS_DROID_VIEW_CFOCUSFINDERHELPER_H__
#define __ELASTOS_DROID_VIEW_CFOCUSFINDERHELPER_H__

#include "_Elastos_Droid_View_CFocusFinderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CFocusFinderHelper)
    , public Singleton
    , public IFocusFinderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI constructor();

    CARAPI GetInstance(
        /* [out] */ IFocusFinder** finder);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CFOCUSFINDERHELPER_H__
