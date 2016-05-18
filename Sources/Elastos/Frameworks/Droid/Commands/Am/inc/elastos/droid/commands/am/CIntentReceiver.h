
#ifndef __ELASTOS_DROID_COMMANDS_AM_CINTENTRECEIVER_H__
#define __ELASTOS_DROID_COMMANDS_AM_CINTENTRECEIVER_H__

#include "_Elastos_Droid_Commands_Am_CIntentReceiver.h"
#include "Am.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Commands::Am::Am;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CIntentReceiver)
    , public Am::IntentReceiver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI PerformReceive(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI WaitForFinish();
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CINTENTRECEIVER_H__
