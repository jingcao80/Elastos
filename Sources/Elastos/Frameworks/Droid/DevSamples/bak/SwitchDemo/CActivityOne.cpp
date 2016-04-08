#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <elastos/core/StringBuilder.h>
#include <stdio.h>
#include "R.h"
#include <elastos/utility/logging/Slogger.h>
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::EIID_ISwitch;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SwitchDemo {

CAR_INTERFACE_IMPL(CActivityOne::MyListener, ICompoundButtonOnCheckedChangeListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::MyListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    SLOGGERD(String("CActivityOne::MyListener"), "CActivityOne::MyListener::OnCheckedChanged, isChecked = %d", isChecked);
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
AutoPtr<IView> v = FindViewById(R::id::textview);
Slogger::D("jiazhenjiang", "address of TextView: %p", IView::Probe(v));
/*    AutoPtr<ISwitch> switchTest = ISwitch::Probe(FindViewById(R::id::switch_test));
    AutoPtr<MyListener> l = new MyListener(this);
    switchTest->SetOnCheckedChangeListener(l);*/
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

} // namespace SwitchDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
