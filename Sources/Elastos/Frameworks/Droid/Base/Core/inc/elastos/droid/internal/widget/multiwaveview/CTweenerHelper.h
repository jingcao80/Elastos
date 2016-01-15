
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_CTWEENERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_CTWEENERHELPER_H__

#include "_Elastos_Droid_Internal_Widget_Multiwaveview_CTweenerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

CarClass(CTweenerHelper)
    , public Singleton
    , public ITweenerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI To(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ ITweener** result);

    CARAPI Reset();
};

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_CTWEENERHELPER_H__
