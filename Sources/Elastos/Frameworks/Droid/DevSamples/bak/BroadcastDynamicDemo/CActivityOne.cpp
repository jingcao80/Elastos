
#include <stdio.h>
#include <unistd.h>
#include <elautoptr.h>
#include "CActivityOne.h"
#include "CBroadcastReceiverOne.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BroadcastDynamicDemo {

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    printf("\n== CActivityOne::OnCreate ==\n");
    const String text=String("elastos.BroadcastDynamicDemo.TEST_RECEIVER");
    mReceiver = new CBroadcastReceiverOne();
    CIntentFilter::New(text, (IIntentFilter**)&mFilter);
    printf("\n==Ready to Register Broadcast Receiver!==\n");
    AutoPtr<IIntent> pIntent;
    RegisterReceiver(mReceiver,mFilter,(IIntent**)&pIntent);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    printf("\n==Ready to send Broadcast message!==\n");
    String text=String("elastos.BroadcastDynamicDemo.TEST_RECEIVER");
    AutoPtr<IIntent> pIntent;
    CIntent::New(text,(IIntent**)&pIntent);

    SendBroadcast(pIntent);
    // sleep(5);
    // printf("\n==Ready to Unregister Receiver!==\n");
    // UnregisterReceiver(mReceiver);
    // sleep(1);
    // printf("\n==Ready to send Broadcast message 2 !==\n");
    // SendBroadcast(pIntent);
    return NOERROR;
}

ECode CActivityOne::OnRestart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}


} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos