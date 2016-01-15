
#include "CThreadGroup.h"
#include "Thread.h"

namespace Elastos {
namespace Core {

AutoPtr<IThreadGroup> CThreadGroup::sSystem;
AutoPtr<IThreadGroup> CThreadGroup::sMain;

CAR_OBJECT_IMPL(CThreadGroup)

AutoPtr<IThreadGroup> CThreadGroup::GetSystemThreadGroup()
{
    if (sSystem == NULL) {
        AutoPtr<CThreadGroup> group;
        ASSERT_SUCCEEDED(CThreadGroup::NewByFriend((CThreadGroup**)&group));
        sSystem = (IThreadGroup*)group.Get();
    }
    return sSystem;
}

AutoPtr<IThreadGroup> CThreadGroup::GetMainThreadGroup()
{
    if (sMain == NULL) {
        if (sSystem == NULL) {
            AutoPtr<CThreadGroup> group;
            ASSERT_SUCCEEDED(CThreadGroup::NewByFriend((CThreadGroup**)&group));
            sSystem = (IThreadGroup*)group.Get();
        }
        AutoPtr<CThreadGroup> group;
        ASSERT_SUCCEEDED(CThreadGroup::NewByFriend(sSystem, String("main"), (CThreadGroup**)&group));
        sMain = (IThreadGroup*)group.Get();
    }
    return sMain;
}

} // namespace Core
} // namespace Elastos
