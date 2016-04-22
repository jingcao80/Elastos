#include "CBroadcastReceiverTwo.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastStaticDemo {

static const String TAG("BroadcastStaticDemo::CBroadcastReceiverTwo");

CAR_OBJECT_IMPL(CBroadcastReceiverTwo)

ECode CBroadcastReceiverTwo::constructor()
{
    return NOERROR;
}

ECode CBroadcastReceiverTwo::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String msg;
    intent->GetStringExtra(String("msg"), &msg);
    Logger::I(TAG, "Get Broadcast: %s, msg: %s", TO_CSTR(intent), msg.string());

    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(String("msg"), msg + "@ReceiverTwo");
    return SetResultExtras(bundle);
}

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Elastos
