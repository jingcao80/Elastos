
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_WINDOWMANAGERPOLICYTHREAD_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_WINDOWMANAGERPOLICYTHREAD_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IThread;
using Elastos::Droid::Os::ILooper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class WindowManagerPolicyThread
{
public:
    static CARAPI Set(
        /* [in] */ IThread* thread,
        /* [in] */ ILooper* looper);

    static CARAPI_(AutoPtr<IThread>) GetThread();

    static CARAPI_(AutoPtr<ILooper>) GetLooper();

private:
    WindowManagerPolicyThread();

private:
    static AutoPtr<IThread> sThread;
    static AutoPtr<ILooper> sLooper;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_WINDOWMANAGERPOLICYTHREAD_H__
