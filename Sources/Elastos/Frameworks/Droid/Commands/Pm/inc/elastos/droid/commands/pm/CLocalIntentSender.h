
#ifndef __ELASTOS_DROID_COMMANDS_PM_CLOCALINTENTSENDER_H__
#define __ELASTOS_DROID_COMMANDS_PM_CLOCALINTENTSENDER_H__

#include "_Elastos_Droid_Commands_Pm_CLocalIntentSender.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Pm.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Pm {

CarClass(CLocalIntentSender)
    , public Object
    , public IIIntentSender
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ISynchronousQueue* result);

    CARAPI Send(
        /* [in] */ Int32 code,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* finishedReceiver,
        /* [in] */ const String& requiredPermission,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [in] */ String* str);

private:
    AutoPtr<ISynchronousQueue> mResult;
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_PM_CLOCALINTENTSENDER_H__
