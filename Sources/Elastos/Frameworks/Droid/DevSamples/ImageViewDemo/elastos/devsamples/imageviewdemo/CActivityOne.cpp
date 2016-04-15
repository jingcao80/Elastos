
#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ImageViewDemo {

static const String TAG("ImageViewDemo::CActivityOne");

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
    Logger::I(TAG, "MyListener::OnClick");

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> service;
    sm->GetService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
    IIActivityManager* am = IIActivityManager::Probe(service);

    String package("Elastos.DevSamples.ImageDemo");
    String action("android.intent.action.MAIN");
    String nullStr;

    Logger::I(TAG, " trying to StartActivity %s", package.string());
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(package);
    intent->SetAction(action);
    Int32 status;
    ECode ec = am->StartActivity(NULL, package, intent, nullStr,
        NULL, nullStr, 0, 0, NULL, NULL, &status);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to StartActivity package:%s, action:%s, ec=%08x, status=%d, intent=%s",
            package.string(), action.string(), ec, status, TO_CSTR(intent));
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

    AutoPtr<IView> view = FindViewById(R::id::ImageView);
    IImageView* imageView = IImageView::Probe(view);
    imageView->SetImageResource(R::drawable::earth);
    imageView->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_FIT_CENTER);

    AutoPtr<MyListener> l = new MyListener(this);
    view->SetOnClickListener(l.Get());

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

} // namespace ImageViewDemo
} // namespace DevSamples
} // namespace Elastos
