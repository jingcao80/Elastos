
#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

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
namespace ImageDemo {

static const String TAG("ImageDemo::CActivityOne");

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
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

CActivityOne::~CActivityOne()
{
    Logger::I(TAG, " >> Destory ImageDemo::CActivityOne");
}

UInt32 CActivityOne::AddRef()
{
    UInt32 count = Activity::AddRef();
    // android::CallStack stack;
    // stack.update();
    // String backtrace(stack.toString("").string());
    // Logger::I(TAG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    // Logger::I(TAG, "ImageDemoActivity::AddRef, refcount: %d, callstack:\n%s", count, backtrace.string());
    // Logger::I(TAG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    return count;
}

UInt32 CActivityOne::Release()
{
    UInt32 count = Activity::Release();
    Logger::I(TAG, "ImageDemoActivity::Release, refcount: %d", count);
    // android::CallStack stack;
    // stack.update();
    // String backtrace(stack.toString("").string());
    // Logger::I(TAG, "-------------------------------------------------------");
    // Logger::I(TAG, "ImageDemoActivity::Release, refcount: %d, callstack:\n%s", GetStrongCount(), backtrace.string());
    // Logger::I(TAG, "-------------------------------------------------------");

    return count;
}

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
    imageView->SetImageResource(R::drawable::cupple);
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

} // namespace ImageDemo
} // namespace DevSamples
} // namespace Elastos
