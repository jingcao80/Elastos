
#include "CActivityOne.h"
#include "R.h"
#include <os/SystemClock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::CInputManager;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUIDemo {

Boolean CActivityOne::mIsNavigationBarAdded = FALSE;
Int32 CActivityOne::mActivityId = 0;

CAR_INTERFACE_IMPL(CActivityOne::MyListener, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* view)
{
    PRINT_ENTER_LEAVE("CActivityOne::MyListener::OnClick");

    Int32 id;
    view->GetId(&id);

    // if (id == R::id::SecondActivityButton) {
    //     mHost->LaunchActivityTwo();
    // }
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PRINT_ENTER_LEAVE("CActivityOne::OnCreate");

    // Create Listener
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

    // Setup ContentView
    //
    // SetContentView(R::layout::main);
    SetContentView(R::layout::system_bar);

    // // Setup Button
    // //
    // AutoPtr<IView> secondActivityButton = FindViewById(R::id::SecondActivityButton);
    // if (secondActivityButton != NULL)
    //     secondActivityButton->SetOnClickListener(clickListener);

    // // Setup TextView
    // //
    // AutoPtr<IView> tmpView = FindViewById(R::id::TextViewOne);
    // AutoPtr<ITextView> textView = ITextView::Probe(tmpView.Get());
    // assert(textView != NULL);

    // StringBuilder sb("ActivityOne 实例：");
    // sb += (++mActivityId);
    // AutoPtr<ICharSequence> csq;
    // CStringWrapper::New(sb.ToString(), (ICharSequence**)&csq);
    // textView->SetText(csq);

    // Setup NavigationBar
    //
//    CreateNavigationBar();

    return NOERROR;
}

ECode CActivityOne::LaunchActivityTwo()
{
    PFL_EX("CActivityOne::LaunchActivityTwo()");

    String packageName("SystemUIDemo");
    String actionName("android.intent.action.SECOND");

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(packageName);
    intent->SetAction(actionName);

    ECode ec = StartActivity(intent);
    if (FAILED(ec)) {
        PFL_EX("Failed to start second activity. ec: %08x", ec);
    }
    return NOERROR;
}

ECode CActivityOne::CreateNavigationBar()
{
    if (!mIsNavigationBarAdded) {
        mIsNavigationBarAdded = TRUE;

        AutoPtr<ILayoutInflater> inflater;
        GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

        AutoPtr<IView> navigationBar;
        inflater->Inflate(R::layout::navigation_bar, NULL, (IView**)&navigationBar);

        AutoPtr<IWindowManagerLayoutParams> lp;
        CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR,
            0 | IWindowManagerLayoutParams::FLAG_TOUCHABLE_WHEN_WAKING
            | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            IPixelFormat::OPAQUE, (IWindowManagerLayoutParams**)&lp);

        lp->SetGravity(IGravity::BOTTOM | IGravity::FILL_HORIZONTAL);
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(String("NavigationBar"), (ICharSequence**)&cs);
        lp->SetTitle(cs);

        AutoPtr<IWindowManager> windowManager = GetWindowManager();
        windowManager->AddView(navigationBar, IViewGroupLayoutParams::Probe(lp.Get()));
    }

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStop");
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnDestroy");
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}


} // namespace SystemUIDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
