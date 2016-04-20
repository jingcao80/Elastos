#include "CBroadcastReceiverOne.h"
#include <Elastos.Droid.Content.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastDynamicDemo {

static const String TAG("BroadcastDynamicDemo::CBroadcastReceiverOne");

CAR_OBJECT_IMPL(CBroadcastReceiverOne)

ECode CBroadcastReceiverOne::constructor()
{
    return NOERROR;
}

ECode CBroadcastReceiverOne::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Logger::I(TAG, "Get Broadcast message: %s", TO_CSTR(intent));
    return NOERROR;
}

} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Elastos
