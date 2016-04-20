
#ifndef __Elastos_DevSamples_BroadcastStaticDemo_CBroadcastReceiverTwo_H__
#define __Elastos_DevSamples_BroadcastStaticDemo_CBroadcastReceiverTwo_H__

#include "_Elastos_DevSamples_BroadcastStaticDemo_CBroadcastReceiverTwo.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace DevSamples {
namespace BroadcastStaticDemo {

CarClass(CBroadcastReceiverTwo)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("BroadcastStaticDemo::CBroadcastReceiverTwo")

    CARAPI constructor();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_BroadcastStaticDemo_CBroadcastReceiverTwo_H__
