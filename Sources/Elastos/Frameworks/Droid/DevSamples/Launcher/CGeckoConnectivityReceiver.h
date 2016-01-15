
#ifndef __CGECKOCONNECTIVITYRECEIVER_H__
#define __CGECKOCONNECTIVITYRECEIVER_H__

#include "_CGeckoConnectivityReceiver.h"
//#include "BroadcastReceiver.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

CarClass(CGeckoConnectivityReceiver)//, public BroadcastReceiver
{
public:

protected:
    CARAPI OnReceive(
    /* [in] */ IContext *pContext,
    /* [in] */ IIntent *pIntent);

private:
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CGECKOCONNECTIVITYRECEIVER_H__

