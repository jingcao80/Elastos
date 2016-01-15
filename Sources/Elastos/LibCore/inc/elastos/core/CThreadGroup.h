
#ifndef __ELASTOS_CORE_CTHREADGROUP_H__
#define __ELASTOS_CORE_CTHREADGROUP_H__

#include "_Elastos_Core_CThreadGroup.h"
#include "ThreadGroup.h"

namespace Elastos {
namespace Core {

CarClass(CThreadGroup)
    , public ThreadGroup
{
public:
    CAR_OBJECT_DECL()

    static CARAPI_(AutoPtr<IThreadGroup>) GetSystemThreadGroup();

    static CARAPI_(AutoPtr<IThreadGroup>) GetMainThreadGroup();

private:
    // BEGIN android-added
    /* the VM uses these directly; do not rename */
    static AutoPtr<IThreadGroup> sSystem;
    static AutoPtr<IThreadGroup> sMain;
    // END android-added
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CTHREADGROUP_H__
