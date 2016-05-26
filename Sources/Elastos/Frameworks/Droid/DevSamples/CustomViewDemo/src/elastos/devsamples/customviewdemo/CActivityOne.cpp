
#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace CustomViewDemo {

static const String TAG("CustomViewDemo::CActivityOne");

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
    ICustomImageView* civ = ICustomImageView::Probe(v);
    if (civ) {
        String name;
        civ->GetImageName(&name);
        Logger::I(TAG, "MyListener::OnClick %s", name.string());
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

    ICustomImageView* civ = ICustomImageView::Probe(view);
    if (civ) {
        civ->SetImageName(String("CustomImageView"));
    }
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

} // namespace CustomViewDemo
} // namespace DevSamples
} // namespace Elastos
