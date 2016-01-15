
#include "CMainActivity.h"
#include "CBindHelloService.h"
#include "R.h"
#include <elastos/Logger.h>
#include <elastos/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace HelloService {

CAR_INTERFACE_IMPL(CMainActivity::ClickListener, IViewOnClickListener)
CAR_INTERFACE_IMPL(CMainActivity::ServiceConnection, IServiceConnection)

CMainActivity::ClickListener::ClickListener(
    /* [in] */ CMainActivity* host)
    : mHost(host)
{}

CMainActivity::ServiceConnection::CServiceConnection(
    /* [in] */ CMainActivity* host)
    : mHost(host)
{}

ECode CMainActivity::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    ((CBindHelloService::HelloBinder*)service)->GetService(mHost->mBindService);
    assert(mHost->mBindService != NULL);
    Logger::D(TAG, "OnServiceConnected");
    return NOERROR;
}

ECode CMainActivity::CServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    Logger::D(TAG, "OnServiceDisconnected");
    return NOERROR;
}

ECode CMainActivity::ClickListener::OnClick(
    /*[in]*/ IView* view)
{
    Int32 viewId;
    view->GetId(&viewId);
    switch(viewId) {
        case R::id::btn_hello: {
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            StringBuilder sb("Hello this is a service!");
            AutoPtr<ICharSequence> cs = sb.ToCharSequence();
            AutoPtr<IToast> toast;
            helper->MakeText(mHost,cs,IToast::LENGTH_LONG,(IToast**)&toast);
            toast->Show();
            mHost->mBindService->SayHello();
            break;
        }
        default:
        	break;
    }

    return NOERROR;
}

const String CMainActivity::TAG("CMainActivity");

CMainActivity::CMainActivity()
{
    mClickListener = new ClickListener(this);
    mConn = new CServiceConnection(this);
}

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);
    Logger::D(TAG, "CMainActivity OnCreate");
    AutoPtr<IView> button = FindViewById(R::id::btn_hello);
    button->SetOnClickListener(mClickListener);
    return NOERROR;
}

ECode CMainActivity::OnStart()
{
    Activity::OnStart();
    AutoPtr<IComponentName> com;
    CComponentName::New(this, String("HelloService.CBindHelloService"), (IComponentName**)&com);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(com);

    Logger::D(TAG,"=== CMainActivity::OnStart : Ready to BindService === ");
    Boolean result;
    BindService(intent, mConn, IContext::BIND_AUTO_CREATE, &result);

    Logger::D(TAG, "CMainActivity OnStart");
    return NOERROR;
}

ECode CMainActivity::OnRestart()
{
    Activity::OnRestart();
    Logger::D(TAG, "CMainActivity OnRestart");
    return NOERROR;
}

ECode CMainActivity::OnResume()
{
    Activity::OnResume();
    Logger::D(TAG, "CMainActivity OnResume");
    return NOERROR;
}

ECode CMainActivity::OnPause()
{
    Activity::OnPause();
    Logger::D(TAG, "CMainActivity OnPause");
    return NOERROR;
}

ECode CMainActivity::OnStop()
{
    Activity::OnStop();
    UnbindService(mConn);
    Logger::D(TAG, "CMainActivity OnStop");
    return NOERROR;
}

ECode CMainActivity::OnDestroy()
{
    Activity::OnDestroy();
    Logger::D(TAG, "CMainActivity OnDestroy");
    return NOERROR;
}

} // namespace HelloService
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
