
#include "CActivityTwo.h"
#include "R.h"
#include <stdio.h>
#include <utils/SystemClock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUIDemo {

Int32 CActivityTwo::mActivityId = 0;

CAR_INTERFACE_IMPL(CActivityTwo::MyListener, IViewOnClickListener)

CActivityTwo::MyListener::MyListener(
    /* [in] */ CActivityTwo* host) :
    mHost(host)
{
}

ECode CActivityTwo::MyListener::OnClick(
    /* [in] */ IView* view)
{
    PRINT_ENTER_LEAVE("CActivityTwo::MyListener::OnClick");

    Int32 id;
    view->GetId(&id);

    // if (id == R::id::BackButton) {
    //     mHost->LaunchActivityOne();
    // }
    return NOERROR;
}

ECode CActivityTwo::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PRINT_ENTER_LEAVE("CActivityTwo::OnCreate");

    // Create Listener
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

    // Setup ContentView
    //
    SetContentView(R::layout::main);

    // // Setup Button
    // //
    // AutoPtr<IView> backButton = FindViewById(R::id::BackButton);
    // assert(backButton != NULL);
    // backButton->SetOnClickListener(clickListener);

    // AutoPtr<IView> noneButton = FindViewById(R::id::NoneButton);
    // assert(noneButton != NULL);
    // noneButton->SetOnClickListener(clickListener);

    // // Setup TextView
    // //
    // AutoPtr<IView> tmpView = FindViewById(R::id::TextViewTwo);
    // AutoPtr<ITextView> textView = ITextView::Probe(tmpView.Get());
    // assert(textView != NULL);

    // StringBuilder sb("ActivityTwo 实例：");
    // sb += (++mActivityId);
    // AutoPtr<ICharSequence> csq;
    // CStringWrapper::New(sb.ToString(), (ICharSequence**)&csq);
    // textView->SetText(csq);

    return NOERROR;
}

ECode CActivityTwo::LaunchActivityOne()
{
    PFL_EX("CActivityTwo::LaunchActivityOne()");

    String packageName("SystemUIDemo");
    String actionName("android.intent.action.MAIN");

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(packageName);
    intent->SetAction(actionName);

    ECode ec = StartActivity(intent);
    if (FAILED(ec)) {
        PFL_EX("Failed to start first activity. ec: %08x", ec);
    }
    return NOERROR;
}

ECode CActivityTwo::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityTwo::OnStart");
    return NOERROR;
}

ECode CActivityTwo::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityTwo::OnResume");
    return NOERROR;
}

ECode CActivityTwo::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityTwo::OnPause");
    return NOERROR;
}

ECode CActivityTwo::OnStop()
{
    PRINT_ENTER_LEAVE("CActivityTwo::OnStop");
    return NOERROR;
}

ECode CActivityTwo::OnDestroy()
{
    PRINT_ENTER_LEAVE("CActivityTwo::OnDestroy");
    return NOERROR;
}

ECode CActivityTwo::OnActivityResult(
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
