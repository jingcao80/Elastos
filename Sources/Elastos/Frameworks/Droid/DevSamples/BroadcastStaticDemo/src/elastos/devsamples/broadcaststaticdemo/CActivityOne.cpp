
#include "CActivityOne.h"
#include "R.h"
#include "elastos/droid/R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::CDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::Widget::IRelativeLayout;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::CButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastStaticDemo {

static const String TAG("BroadcastStaticDemo::CActivityOne");
const String RECEIVER_KEY("Elastos.DevSamples.BroadcastDemo.TEST_RECEIVER");
const Boolean CActivityOne::TEST_ORDERED_BROADCAST = TRUE;

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
    case R::id::SendBroadcast:
        Logger::I(TAG, "MyListener::OnClick Send");
        mHost->Send();
        break;
    case R::id::TestMount:
        Logger::I(TAG, "MyListener::OnClick Test Mount");
        mHost->OtherTest();
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

    AutoPtr<MyListener> l = new MyListener(this);
    AutoPtr<IView> view = FindViewById(R::id::SendBroadcast);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::TestMount);
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

ECode CActivityOne::Send()
{
    AutoPtr<IIntent> intent;
    CIntent::New(RECEIVER_KEY, (IIntent**)&intent);
    intent->PutExtra(String("msg"), String("Hello Broadcast!"));

    if (TEST_ORDERED_BROADCAST) {
        return SendOrderedBroadcast(intent, String(NULL));
    }
    else {
        return SendBroadcast(intent);
    }

    return NOERROR;
}

ECode CActivityOne::AddButton(
    /* [in] */ ILinearLayout* zenButtons,
    /* [in] */ const char* text)
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::segmented_button, IViewGroup::Probe(zenButtons), FALSE, (IView**)&view);
    AutoPtr<ICharSequence> cs = CoreUtils::Convert(text);
    ITextView::Probe(view)->SetText(cs);

    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 count;
    IViewGroup::Probe(zenButtons)->GetChildCount(&count);
    if (count == 0) {
        IViewGroupMarginLayoutParams::Probe(lp)->SetRightMargin(0);
        IViewGroupMarginLayoutParams::Probe(lp)->SetLeftMargin(0); // first button has no margin
    }
    view->SetLayoutParams(lp);
    IViewGroup::Probe(zenButtons)->AddView(view);
    return NOERROR;
}

ECode CActivityOne::CreateVolumeDialog()
{
    Boolean bval;
    AutoPtr<IDialog> mDialog;
    CDialog::New(this, (IDialog**)&mDialog);

    AutoPtr<IWindow> window;
    mDialog->GetWindow((IWindow**)&window);
    window->RequestFeature(IWindow::FEATURE_NO_TITLE, &bval);

    mDialog->SetCanceledOnTouchOutside(TRUE);
    mDialog->SetContentView(R::layout::volume_dialog);
    mDialog->Create();

    AutoPtr<IWindowManagerLayoutParams> lp;
    window->GetAttributes((IWindowManagerLayoutParams**)&lp);
    lp->SetType(IWindowManagerLayoutParams::TYPE_TOAST);
    window->SetAttributes(lp);

    AutoPtr<IDrawable> cd;
    CColorDrawable::New(0xff882020, (IDrawable**)&cd);
    window->SetBackgroundDrawable(cd);

    AutoPtr<IView> mView;
    window->FindViewById(Elastos::Droid::R::id::content, (IView**)&mView);

    AutoPtr<IViewParent> vp;
    mView->GetParent((IViewParent**)&vp);

    AutoPtr<IView> v1, v2, v3;
    mView->FindViewById(R::id::visible_panel, (IView**)&v1);
    AutoPtr<IViewGroup> mPanel = IViewGroup::Probe(v1);

    mView->FindViewById(R::id::slider_panel, (IView**)&v2);
    AutoPtr<IViewGroup> mSliderPanel = IViewGroup::Probe(v2);

    {
        AutoPtr<IButton> button;
        CButton::New(this, (IButton**)&button);

        AutoPtr<ILinearLayoutLayoutParams> lp;
        CLinearLayoutLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            120,
            (ILinearLayoutLayoutParams**)&lp);
        lp->SetGravity(IGravity::CENTER);
        IView::Probe(button)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
        ITextView::Probe(button)->SetGravity(IGravity::CENTER);
        ITextView::Probe(button)->SetText(CoreUtils::Convert("Button"));
        mSliderPanel->AddView(IView::Probe(button));
    }

    mDialog->Show();
    return NOERROR;
}

ECode CActivityOne::OtherTest()
{
    // CreateVolumeDialog();

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);

    AutoPtr<IInterface> obj;
    serviceManager->GetService(String("mount"), (IInterface**)&obj);
    AutoPtr<IIMountService> mount = IIMountService::Probe(obj);
    if (mount != NULL) {
        AutoPtr<ArrayOf<IStorageVolume*> > volumes;
        ECode ec = mount->GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);
        if (FAILED(ec)) {
            Logger::E(TAG, "Exception during GetVolumeList");
        }
        else {
            if (volumes != NULL) {
                for (Int32 i = 0; i < volumes->GetLength(); ++i) {
                    Logger::I(TAG, " volume %d : %s", i, TO_CSTR((*volumes)[i]));
                }
            }
        }
    }
    else {
        Logger::W(TAG, "MountService unavailable for shutdown");
    }
    return NOERROR;
}

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Elastos
