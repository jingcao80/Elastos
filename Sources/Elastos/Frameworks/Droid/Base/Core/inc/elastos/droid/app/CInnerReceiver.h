#ifndef __ELASTOS_DROID_APP_CINNERRECEIVER_H__
#define __ELASTOS_DROID_APP_CINNERRECEIVER_H__

#include "_Elastos_Droid_App_CInnerReceiver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CInnerReceiver)
    , public Object
    , public IIntentReceiver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IReceiverDispatcher* rd,
        /* [in] */ Boolean strong);

    CARAPI PerformReceive(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI ToString(
        /* [out] */ String* str);
private:
    AutoPtr<IWeakReference> mDispatcher; // LoadedPkg::ReceiverDispatcher
    AutoPtr<IReceiverDispatcher> mStrongRefRef;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CINNERRECEIVER_H__
