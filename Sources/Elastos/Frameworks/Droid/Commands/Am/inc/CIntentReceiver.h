
#ifndef __CINTENTRECEIVER_H__
#define __CINTENTRECEIVER_H__

#include "_CIntentReceiver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CIntentReceiver)
{
public:
    CIntentReceiver();
    CARAPI PerformReceive(
        /* [in] */ IIntent * intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle * extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI WaitForFinish();

private:
    Object mLock;
    Boolean mFinished;
};

}
}
}
}

#endif // __CINTENTRECEIVER_H__
