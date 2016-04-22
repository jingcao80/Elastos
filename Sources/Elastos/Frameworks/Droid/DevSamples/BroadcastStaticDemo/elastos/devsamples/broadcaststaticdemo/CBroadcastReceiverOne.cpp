#include "CBroadcastReceiverOne.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastStaticDemo {

static const String TAG("BroadcastStaticDemo::CBroadcastReceiverOne");

CAR_OBJECT_IMPL(CBroadcastReceiverOne)

ECode CBroadcastReceiverOne::constructor()
{
    return NOERROR;
}

ECode CBroadcastReceiverOne::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String msg;
    AutoPtr<IBundle> bundle;
    GetResultExtras(TRUE, (IBundle**)&bundle);
    if (bundle) {
        bundle->GetString(String("msg"), &msg);
    }

    Logger::I(TAG, "Get Broadcast: %s, msg: %s", TO_CSTR(intent), msg.string());
    return NOERROR;
}

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Elastos
