#ifndef __ELASTOS_DROID_INTERNAL_VIEW_CWINDOWMANAGERPOLICYTHREAD_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_CWINDOWMANAGERPOLICYTHREAD_H__

#include "_Elastos_Droid_Internal_View_CWindowManagerPolicyThread.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Core::IThread;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CarClass(CWindowManagerPolicyThread)
    , public Singleton
    , public IWindowManagerPolicyThread
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Set(
        /* [in] */ IThread* thread,
        /* [in] */ ILooper* looper);

    CARAPI GetThread(
        /* [out] */ IThread** thd);

    CARAPI GetLooper(
        /* [out] */ ILooper** lpr);
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_CWINDOWMANAGERPOLICYTHREAD_H__
