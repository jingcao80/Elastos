#ifndef __ELASTOS_DROID_SERVER_WM_TASK_H__
#define __ELASTOS_DROID_SERVER_WM_TASK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/wm/AppWindowToken.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

#define To_Task(obj) ((Task*)IObject::Probe(obj))

class TaskStack;
class DisplayContent;

class Task : public Object
{
public:
    Task(
        /* [in] */ AppWindowToken* wtoken,
        /* [in] */ TaskStack* stack,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<DisplayContent>) GetDisplayContent();

    CARAPI_(void) AddAppToken(
        /* [in] */ Int32 addPos,
        /* [in] */ AppWindowToken* wtoken);

    CARAPI_(Boolean) RemoveAppToken(
        /* [in] */ AppWindowToken* wtoken);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<TaskStack> mStack;
    AutoPtr<IArrayList> mAppTokens;
    Int32 mTaskId;
    Int32 mUserId;
    Boolean mDeferRemoval;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_TASK_H__
