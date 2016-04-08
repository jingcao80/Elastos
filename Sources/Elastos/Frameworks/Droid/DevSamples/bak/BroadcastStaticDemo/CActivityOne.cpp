
#include "CActivityOne.h"
#include "R.h"
#include <stdio.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BroadcastStaticDemo {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CActivityOne::MyListener, IViewOnClickListener)

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    printf("\n==Ready to SendOrderedBroadcast message!==\n");
    String text = String("elastos.BroadcastDemo.TEST_RECEIVER");
    AutoPtr<IIntent> pIntent;
    CIntent::New(text,(IIntent**)&pIntent);
    mHost->SendOrderedBroadcast(pIntent, String(NULL));

    return NOERROR;
}


ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::BroadcastButton);
    assert(view != NULL);
    AutoPtr<MyListener> l = new MyListener(this);
    view->SetOnClickListener(l.Get());

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    // printf("\n==Ready to send Broadcast message!==\n");
    // String text = String("elastos.BroadcastDemo.TEST_RECEIVER");
    // AutoPtr<IIntent> pIntent;
    // CIntent::New(text,(IIntent**)&pIntent);

    //IComponentName* pComponentName;
    //CComponentName::New("BroadcastDemo",,&pComponentName);
    //pIntent->SetComponent(pComponentName);

    //pIntent->SetAction(text);

    //IIntentFilter* pIntentFilter;
    //CIntentFilter::New(text,&pIntentFilter);
    //IBroadcastReceiver* pBroadcastReceiver;
    //CBroadcastReceiverOne::New(&pBroadcastReceiver);
    //RegisterReceiver(pBroadcastReceiver,pIntentFilter);

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

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos