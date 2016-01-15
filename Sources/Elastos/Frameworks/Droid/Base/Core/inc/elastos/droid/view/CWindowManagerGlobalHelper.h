#ifndef __ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBALHELPER_H__
#define __ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBALHELPER_H__

#include "_Elastos_Droid_View_CWindowManagerGlobalHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CWindowManagerGlobalHelper)
    , public Singleton
    , public IWindowManagerGlobalHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IWindowManagerGlobal** wmg);

    CARAPI GetWindowManagerService(
        /* [out] */ IIWindowManager** windowManager);

    CARAPI GetWindowSession(
        /* [out] */ IWindowSession** windowSession);

    CARAPI PeekWindowSession(
        /* [out] */ IWindowSession** windowSession);

    CARAPI ShouldDestroyEglContext(
        /* [in] */ Int32 trimLevel,
        /* [out] */ Boolean* result);

    CARAPI TrimForeground();
};


} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBALHELPER_H__