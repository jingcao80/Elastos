#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWROTATIONWATCHER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWROTATIONWATCHER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneWindowRotationWatcher.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IWindow;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPhoneWindowRotationWatcher)
{
public:
    class MyRunnable : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CPhoneWindowRotationWatcher* host);

        CARAPI Run();
    private:
        CPhoneWindowRotationWatcher* mHost;
    };

    CPhoneWindowRotationWatcher();

    //CARAPI constructor();

    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

    CARAPI AddWindow(
        /* [in] */ IWindow* phoneWindow);

    CARAPI RemoveWindow(
        /* [in] */ IWindow* phoneWindow);

protected:
    CARAPI DispatchRotationChanged();

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<MyRunnable> mRotationChanged;
    List<AutoPtr<IWeakReference> > mWindows;
    Boolean mIsWatching;
    Object mLock;
};


}//namespace Impl
}// namespace Policy
}// namespace Internal
}// namespace Droid
}// namespace Elastos
#endif
