
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include "CBroadcastReceiverOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BroadcastDynamicDemo {

class CActivityOne : public Activity
{
public:

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnRestart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IIntentFilter> mFilter;
};

} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
