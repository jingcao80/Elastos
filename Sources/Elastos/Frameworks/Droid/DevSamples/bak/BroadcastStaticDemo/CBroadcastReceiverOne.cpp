#include <stdio.h>
#include "CBroadcastReceiverOne.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BroadcastStaticDemo {

ECode CBroadcastReceiverOne::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // TODO: Add your code here
    printf("\n\n===CBroadcastReceiverOne Get Broadcast message!===\n\n");
    return NOERROR;
}

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
