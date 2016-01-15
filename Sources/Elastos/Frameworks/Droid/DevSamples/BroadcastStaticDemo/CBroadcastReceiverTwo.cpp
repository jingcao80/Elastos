#include <stdio.h>
#include "CBroadcastReceiverTwo.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BroadcastStaticDemo {

ECode CBroadcastReceiverTwo::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // TODO: Add your code here
    printf("\n\n===CBroadcastReceiverTwo Get Broadcast message!===\n\n");
    return NOERROR;
}

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
