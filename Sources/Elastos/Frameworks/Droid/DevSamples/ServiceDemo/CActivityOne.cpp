
#include "CActivityOne.h"
#include "R.h"
#include <stdio.h>
#include <utils/SystemClock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::Core::CObjectContainer;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
//using Elastos::Droid::View::Animation::CRotateAnimation;
//using Elastos::Droid::View::Animation::CAlphaAnimation;
//using Elastos::Droid::View::Animation::CRotateAnimation;
//using Elastos::Droid::View::Animation::CTranslateAnimation;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ServiceDemo {

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

    PRINT_FILE_LINE_EX("=== CActivityOne::MyListener::OnClick : Ready to UnbindService ===")
    mHost->UnbindService(mHost->mConn);

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
    SetContentView(R::layout::main);

    AutoPtr<IView> b = FindViewById(R::id::AnamtionButton);
    assert(b != NULL);
    b->SetOnClickListener(clickListener);

    CServiceConnection::New((IServiceConnection**)&mConn);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    AutoPtr<IIntent> intent;
    AutoPtr<IComponentName> com;
    AutoPtr<IComponentName> component;

    CComponentName::New(this, String("ServiceDemo.CServiceOne"), (IComponentName**)&com);
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(com);

// PRINT_FILE_LINE_EX("=== CActivityOne::OnStart : Ready to StartService ===")
//     StartService(intent, (IComponentName**)&component);

    Boolean result;
PRINT_FILE_LINE_EX("=== CActivityOne::OnStart : Ready to BindService === pid = %d, tid = %d\n", getpid(),gettid());
    BindService(intent, mConn, IContext::BIND_AUTO_CREATE, &result);

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

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
