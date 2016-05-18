
#ifndef __ELASTOS_DROID_VIEW_CWINDOWMANAGERLAYOUTPARAMSHELPER_H__
#define __ELASTOS_DROID_VIEW_CWINDOWMANAGERLAYOUTPARAMSHELPER_H__

#include "_Elastos_Droid_View_CWindowManagerLayoutParamsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CWindowManagerLayoutParamsHelper)
    , public Singleton
    , public IWindowManagerLayoutParamsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI MayUseInputMethod(
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* mayUse);
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CWINDOWMANAGERLAYOUTPARAMSHELPER_H__
