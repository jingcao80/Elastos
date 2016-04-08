#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <stdio.h>
#include "R.h"
#include <elastos/utility/logging/Slogger.h>
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ScrollViewDemo {

CAR_INTERFACE_IMPL(MyListener, IViewOnClickListener)

MyListener::MyListener(
    /* [in] */ CActivityOne* host)
{
    mHost = host;
}

ECode MyListener::OnClick(
   /* [in] */ IView* v)
{
    Slogger::D("jiazhenjiang", "............................The Button in ScrollView was Clicked");
    Int32 height = 0;
    mHost->mScroll->GetHeight(&height);
Slogger::D("jiazhenjiang", "mScroll height = %d", height);
    Int32 count = 0, len = 0;
    mHost->mScroll->GetChildCount(&count);
Slogger::D("jiazhenjiang", "mScroll childlen count = %d", count);
    for(Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> v;
        mHost->mScroll->GetChildAt(i, (IView**)&v);
        Int32 childlen = 0;
        v->GetHeight(&childlen);
        len += childlen;
Slogger::D("jiazhenjiang", "childlen = %d, len = %d", childlen, len);
    }
    mHost->mScroll->Fling(2000);
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Slogger::D("jiazhenjiang", "CActivityOne::OnCreate");
    SetContentView(R::layout::main);
    AutoPtr<IView> v = FindViewById(R::id::edit);
    AutoPtr<IEditText> mEdit = IEditText::Probe(v);

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

} // namespace ScrollViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
