#include "CBroadcastReceiverOne.h"
#include "CActivityOne.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastDynamicDemo {

static const String TAG("BroadcastDynamicDemo::CBroadcastReceiverOne");

CAR_OBJECT_IMPL(CBroadcastReceiverOne)

ECode CBroadcastReceiverOne::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode CBroadcastReceiverOne::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (CActivityOne::TEST_ORDERED_BROADCAST) {
        String msg;
        intent->GetStringExtra(String("msg"), &msg);
        Logger::I(TAG, "Get Broadcast: %s, msg: %s", TO_CSTR(intent), msg.string());
        return NOERROR;
    }
    else {
        String msg;
        AutoPtr<IBundle> bundle;
        GetResultExtras(TRUE, (IBundle**)&bundle);
        if (bundle) {
            bundle->GetString(String("msg"), &msg);
        }

        Logger::I(TAG, "Get Broadcast: %s, msg: %s", TO_CSTR(intent), msg.string());
        return NOERROR;
    }
}

} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Elastos
