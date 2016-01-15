#ifndef __ELASTOS_DROID_SERVER_WM_TASKGROUP_H__
#define __ELASTOS_DROID_SERVER_WM_TASKGROUP_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IApplicationToken;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class TaskGroup : public Object
{
public:
    TaskGroup()
        : mTaskId(-1)
    {}

    CARAPI ToString(
        /* [out] */ String* str);

public:
    Int32 mTaskId;
    List<AutoPtr<IApplicationToken> > mTokens;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_TASKGROUP_H__
