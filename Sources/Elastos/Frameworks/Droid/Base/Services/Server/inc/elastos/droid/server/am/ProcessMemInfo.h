
#ifndef __ELASTOS_DROID_SERVER_AM_PROCESSMEMINFO_H__
#define __ELASTOS_DROID_SERVER_AM_PROCESSMEMINFO_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ProcessMemInfo
    : public Object
    , public IProcessMemInfo
{
public:
    ProcessMemInfo(
        /* [in] */ const String& name,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 oomAdj,
        /* [in] */ Int32 procState,
        /* [in] */ const String& adjType,
        /* [in] */ const String& adjReason);

    CAR_INTERFACE_DECL()

public:
    const String mName;
    const Int32 mPid;
    const Int32 mOomAdj;
    const Int32 mProcState;
    const String mAdjType;
    const String mAdjReason;
    Int64 mPss;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_PROCESSMEMINFO_H__
