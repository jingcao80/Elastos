
#ifndef __ELASTOS_DROID_WEBKIT_BASE_POWERSTATUSRECEIVER_H__
#define __ELASTOS_DROID_WEBKIT_BASE_POWERSTATUSRECEIVER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

/**
 * A BroadcastReceiver that listens to changes in power status and notifies
 * PowerMonitor.
 * It's instantiated by the framework via the application intent-filter
 * declared in its manifest.
 */
class PowerStatusReceiver
    //: public Object
    : public IBroadcastReceiver
{
public:
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_BASE_POWERSTATUSRECEIVER_H__
