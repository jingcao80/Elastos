#ifndef __ELASTOS_DROID_VIEW_CABSSAVEDSTATEHELPER_H__
#define __ELASTOS_DROID_VIEW_CABSSAVEDSTATEHELPER_H__

#include "_Elastos_Droid_View_CAbsSavedStateHelper.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::IAbsSavedStateHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CAbsSavedStateHelper)
    , public Singleton
    , public IAbsSavedStateHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEMPTY_STATE(
        /* [out] */ IAbsSavedState** emptyState);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CABSSAVEDSTATEHELPER_H__
