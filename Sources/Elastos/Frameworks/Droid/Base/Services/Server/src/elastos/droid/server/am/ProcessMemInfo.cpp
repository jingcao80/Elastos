
#include "elastos/droid/server/am/ProcessMemInfo.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(ProcessMemInfo, Object, IProcessMemInfo)

ProcessMemInfo::ProcessMemInfo(
    /* [in] */ const String& name,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 oomAdj,
    /* [in] */ Int32 procState,
    /* [in] */ const String& adjType,
    /* [in] */ const String& adjReason)
    : mName(name)
    , mPid(pid)
    , mOomAdj(oomAdj)
    , mProcState(procState)
    , mAdjType(adjType)
    , mAdjReason(adjReason)
    , mPss(0)
{
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
