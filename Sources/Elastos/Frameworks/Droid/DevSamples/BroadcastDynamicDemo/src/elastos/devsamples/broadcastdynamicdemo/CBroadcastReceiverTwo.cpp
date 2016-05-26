#include "CBroadcastReceiverTwo.h"
#include "CActivityOne.h"
#include <Elastos.Droid.Content.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastDynamicDemo {

static const String TAG("BroadcastDynamicDemo::CBroadcastReceiverTwo");

CAR_OBJECT_IMPL(CBroadcastReceiverTwo)

ECode CBroadcastReceiverTwo::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode CBroadcastReceiverTwo::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String msg;
    intent->GetStringExtra(String("msg"), &msg);
    Logger::I(TAG, " == Get Broadcast: %s, msg: %s", TO_CSTR(intent), msg.string());

    return NOERROR;
}

} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Elastos
