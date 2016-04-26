#ifndef  __ELASTOS_DROID_LAUNCHER2_CPACKAGECHANGEDRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CPACKAGECHANGEDRECEIVER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "_Elastos_Droid_Launcher2_CPackageChangedReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CPackageChangedReceiver)
    , public BroadcastReceiver
{
public:
    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CPACKAGECHANGEDRECEIVER_H__