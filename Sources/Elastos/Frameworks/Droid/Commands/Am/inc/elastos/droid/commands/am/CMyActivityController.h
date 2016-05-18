
#ifndef __ELASTOS_DROID_COMMANDS_AM_CMYACTIVITYCONTROLLER_H__
#define __ELASTOS_DROID_COMMANDS_AM_CMYACTIVITYCONTROLLER_H__

#include "_Elastos_Droid_Commands_Am_CMyActivityController.h"
#include "Am.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CMyActivityController)
    , public Am::MyActivityController
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IIActivityManager* am,
        /* [in] */ const String& gdbPort);

    CARAPI ActivityResuming(
            /* [in] */ const String& pkg,
            /* [out] */ Boolean* result);

    CARAPI ActivityStarting(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* result);

    CARAPI AppCrashed(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ Int64 timeMillis,
        /* [in] */ const String& stackTrace,
        /* [out] */ Boolean* result);

    CARAPI AppEarlyNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& annotation,
        /* [out] */ Int32* result);

    CARAPI AppNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& processStats,
        /* [out] */ Int32* result);

    CARAPI SystemNotResponding(
        /* [in] */ const String& message,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI Run();
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CMYACTIVITYCONTROLLER_H__
