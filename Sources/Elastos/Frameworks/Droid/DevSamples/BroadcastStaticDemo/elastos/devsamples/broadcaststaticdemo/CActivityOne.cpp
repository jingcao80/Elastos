
#include "CActivityOne.h"
#include "CBroadcastReceiverOne.h"
#include "CBroadcastReceiverTwo.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastStaticDemo {

static const String TAG("BroadcastStaticDemo::CActivityOne");
const String RECEIVER_KEY("Elastos.DevSamples.BroadcastStaticDemo.TEST_RECEIVER");

//=======================================================================
// MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    switch(id) {
    case R::id::RegisterReceiver:
        Logger::I(TAG, "MyListener::OnClick Register");
        mHost->Send();
        break;

    // case R::id::UnregisterReceiver:
    //     Logger::I(TAG, "MyListener::OnClick Unregister");
    //     mHost->Unregister();
    //     break;

    // case R::id::SendBroadcast:
    //     Logger::I(TAG, "MyListener::OnClick Send");
    //     mHost->Send();
    //     break;
    }

    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    AutoPtr<IView> view = FindViewById(R::id::RegisterReceiver);
    view->SetOnClickListener(l.Get());

    // view = FindViewById(R::id::UnregisterReceiver);
    // view->SetOnClickListener(l.Get());

    // view = FindViewById(R::id::SendBroadcast);
    // view->SetOnClickListener(l.Get());

    Register();
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

ECode CActivityOne::Register()
{
    if (mReceiverOne != NULL) {
        return NOERROR;
    }

    AutoPtr<IIntentFilter> filter;
    AutoPtr<IIntent> intent;

    CBroadcastReceiverOne::New((IBroadcastReceiver**)&mReceiverOne);
    CIntentFilter::New(RECEIVER_KEY, (IIntentFilter**)&filter);
    RegisterReceiver(mReceiverOne,filter, (IIntent**)&intent);

    filter = NULL; intent = NULL;
    CBroadcastReceiverTwo::New((IBroadcastReceiver**)&mReceiverTwo);
    CIntentFilter::New(RECEIVER_KEY, (IIntentFilter**)&filter);
    return RegisterReceiver(mReceiverTwo,filter, (IIntent**)&intent);
}

ECode CActivityOne::Unregister()
{
    if (mReceiverOne != NULL) {
        UnregisterReceiver(mReceiverOne);
        mReceiverOne = NULL;
    }
    if (mReceiverOne != NULL) {
        UnregisterReceiver(mReceiverTwo);
        mReceiverTwo = NULL;
    }
    return NOERROR;
}

ECode CActivityOne::Send()
{
    AutoPtr<IIntent> intent;
    CIntent::New(RECEIVER_KEY, (IIntent**)&intent);
    return SendOrderedBroadcast(intent, String(NULL));
}

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Elastos
