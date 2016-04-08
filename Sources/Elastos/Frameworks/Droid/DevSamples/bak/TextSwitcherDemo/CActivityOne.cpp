#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <stdio.h>
#include "R.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Widget::ITextSwitcher;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace TextSwitcherDemo {

CAR_INTERFACE_IMPL_2(MyListener, IViewOnClickListener, IViewFactory)

MyListener::MyListener(
    /* [in] */ CActivityOne* host)
{
    mHost = host;
}

ECode MyListener::OnClick(
   /* [in] */ IView* v)
{
    printf("The Button in TextSwitcher was Clicked");
    static StringBuffer sb("文字转换器:");
    static Char32 ch = 'A';
    static Int32 i = 0;
    if(ch < 'z' && i == 1)
    {
        ch += 1;
    } else {
        i = 1;
        ch = 'A';
    }
    sb += ch;
    AutoPtr<ICharSequence> cs = sb.ToCharSequence();
    mHost->mSwitcher->SetText(cs);
    return NOERROR;
}


ECode MyListener::MakeView(
   /* [out] */ IView** view)
{
    printf("MyListener::MakeView");
    AutoPtr<ITextView> v;
    CTextView::New(mHost,(ITextView**)&v);
    v->SetTextSize(36.f);
    *view = IView::Probe(v);
    if(*view)
        (*view)->AddRef();
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PRINT_ENTER_LEAVE("CActivityOne::OnCreate");
    SetContentView(R::layout::main);
    mButton = IButton::Probe(FindViewById(R::id::button));
    mSwitcher = ITextSwitcher::Probe(FindViewById(R::id::switcher));
    AutoPtr<MyListener> l = new MyListener(this);
    mSwitcher->SetFactory(l);
    mButton->SetOnClickListener(l);
    AutoPtr<IAnimation> animIn;
    AutoPtr<IAnimation> animOut;
    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    utils->LoadAnimation(this, 0x010a0000, (IAnimation**)&animIn);
    utils->LoadAnimation(this, 0x010a0001, (IAnimation**)&animOut);
    mSwitcher->SetInAnimation(animIn);
    mSwitcher->SetOutAnimation(animOut);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    PRINT_ENTER_LEAVE("OnActivityResult");
    return NOERROR;
}

} // namespace TextSwitcherDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
