#ifndef __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWWIDGETHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWWIDGETHELPER_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CPagedViewWidgetHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CPagedViewWidgetHelper)
    , public Singleton
    , public IPagedViewWidgetHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetDeletePreviewsWhenDetachedFromWindow(
        /* [in] */ Boolean value);

    CARAPI SetRecyclePreviewsWhenDetachedFromWindow(
        /* [in] */ Boolean value);

    CARAPI ResetShortPressTarget();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWWIDGETHELPER_H__