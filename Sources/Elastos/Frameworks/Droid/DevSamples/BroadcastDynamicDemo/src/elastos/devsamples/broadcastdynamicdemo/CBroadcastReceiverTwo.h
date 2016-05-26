
#ifndef __Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverTwo_H__
#define __Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverTwo_H__

#include "_Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverTwo.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace DevSamples {
namespace BroadcastDynamicDemo {

CarClass(CBroadcastReceiverTwo)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("BroadcastDynamicDemo::CBroadcastReceiverTwo")

    CARAPI constructor();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverTwo_H__
