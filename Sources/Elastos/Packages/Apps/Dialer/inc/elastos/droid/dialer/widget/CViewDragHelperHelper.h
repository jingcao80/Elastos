#ifndef __ELASTOS_DROID_DIALER_WIDGET_CVIEWDRAGHELPERHELPER_H__
#define __ELASTOS_DROID_DIALER_WIDGET_CVIEWDRAGHELPERHELPER_H__

#include "_Elastos_Droid_Dialer_Widget_CViewDragHelperHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Widget {

CarClass(CViewDragHelperHelper)
    , public Singleton
    , public IViewDragHelperHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Factory method to create a new ViewDragHelper.
     *
     * @param forParent Parent view to monitor
     * @param cb Callback to provide information and receive events
     * @return a new ViewDragHelper instance
     */
    CARAPI Create(
        /* [in] */ IViewGroup* forParent,
        /* [in] */ IViewDragHelperCallback* cb,
        /* [out] */ IViewDragHelper** result);

    /**
     * Factory method to create a new ViewDragHelper.
     *
     * @param forParent Parent view to monitor
     * @param sensitivity Multiplier for how sensitive the helper should be about detecting
     *                    the start of a drag. Larger values are more sensitive. 1.0f is normal.
     * @param cb Callback to provide information and receive events
     * @return a new ViewDragHelper instance
     */
    CARAPI Create(
        /* [in] */ IViewGroup* forParent,
        /* [in] */ Float sensitivity,
        /* [in] */ IViewDragHelperCallback* cb,
        /* [out] */ IViewDragHelper** result);
};


} // Widget
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_WIDGET_CVIEWDRAGHELPERHELPER_H__
