
#include "CActivityOne.h"
#include "CServiceConnection.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

static const String TAG("ServiceDemo::CActivityOne");

//=======================================================================
// ButtonListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::ButtonListener, Object, IViewOnClickListener)

CActivityOne::ButtonListener::ButtonListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::ButtonListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);

    Logger::I(TAG, "ButtonListener::OnClick: %08x", id);
    switch(id) {
        case R::id::Bind:
        mHost->ClickBindService();
        break;
        case R::id::Unbind:
        mHost->ClickUnbindService();
        break;
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

    AutoPtr<IViewOnClickListener> l = (IViewOnClickListener*)new ButtonListener(this);

    AutoPtr<IView> view = FindViewById(R::id::Bind);
    view->SetOnClickListener(l);
    view = FindViewById(R::id::Unbind);
    view->SetOnClickListener(l);


    view = FindViewById(R::id::DisplayTime);
    mTextView = ITextView::Probe(view);

    AutoPtr<IWeakReference> weakRef;
    GetWeakReference((IWeakReference**)&weakRef);
    CServiceConnection::New(weakRef, (IServiceConnection**)&mServiceConnection);
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

ECode CActivityOne::ClickBindService()
{
    Logger::I(TAG, " >> ClickBindService() begin. pid = %d, tid = %d\n", getpid(),gettid());

    const String serviceName("Elastos.DevSamples.ServiceDemo.CServiceOne");

    AutoPtr<IComponentName> component;
    CComponentName::New(this, serviceName, (IComponentName**)&component);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(component);

    Boolean result;
    ECode ec = BindService(intent, mServiceConnection, IContext::BIND_AUTO_CREATE, &result);
    if (FAILED(ec)) {
        Logger::I(TAG, "failed to BindService: %s, ec=%08x", TO_CSTR(intent), ec);
    }
    Logger::I(TAG, " << ClickBindService() end");
    return NOERROR;
}

ECode CActivityOne::ClickUnbindService()
{
    Logger::I(TAG, " >> ClickUnbindService() begin");
    ECode ec = UnbindService(mServiceConnection);
    if (FAILED(ec)) {
        Logger::I(TAG, "failed to UnbindService, ec=%08x", ec);
    }
    Logger::I(TAG, " << ClickUnbindService() end");
    return NOERROR;
}

ECode CActivityOne::UpdateTextView(
    /* [in] */ const String& timeStr)
{
    Logger::I(TAG, "UpdateTextView: %s", timeStr.string());
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(timeStr);
    mTextView->SetText(csq);
    return NOERROR;
}


} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos
