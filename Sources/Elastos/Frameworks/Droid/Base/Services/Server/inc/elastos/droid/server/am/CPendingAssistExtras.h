
#ifndef __ELASTOS_DROID_SERVER_AM_CPENDINGASSISTEXTRAS_H__
#define __ELASTOS_DROID_SERVER_AM_CPENDINGASSISTEXTRAS_H__

#include "_Elastos_Droid_Server_Am_CPendingAssistExtras.h"
#include "elastos/droid/server/am/ActivityRecord.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CPendingAssistExtras)
    , public Object
    , public IBinder
    , public IRunnable
{
public:
    CPendingAssistExtras();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IActivityRecord* activity,
        /* [in] */ IBundle* extras,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& hint,
        /* [in] */ Int32 userHandle);

    // @Override
    CARAPI Run();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<ActivityRecord> mActivity;
    AutoPtr<IBundle> mExtras;
    AutoPtr<IIntent> mIntent;
    String mHint;
    Int32 mUserHandle;
    Boolean mHaveResult;
    AutoPtr<IBundle> result;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_CPENDINGASSISTEXTRAS_H__
