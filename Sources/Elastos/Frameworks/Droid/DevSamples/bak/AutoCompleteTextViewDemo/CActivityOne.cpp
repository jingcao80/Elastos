#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <elastos/core/StringBuilder.h>
#include <stdio.h>
#include "R.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::CObjectContainer;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAutoCompleteTextView;
using Elastos::Droid::Widget::EIID_IAutoCompleteTextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AutoCompleteTextViewDemo {

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PRINT_ENTER_LEAVE("CActivityOne::OnCreate");
    SetContentView(R::layout::main);
    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);
    for (Int32 i = 0; i < 2; ++i) {
        StringBuilder sb("测试 ");
        sb += i;
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq.Get());
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::list_item, dataList, (IArrayAdapter**)&adapter);
    assert(adapter != NULL);
    AutoPtr<IView> temp = FindViewById(R::id::autoCompleteTextView1);
    mView = IAutoCompleteTextView::Probe(temp);
    AutoPtr<IListAdapter> tempAdapter = IListAdapter::Probe(adapter.Get());
    mView->SetAdapter(tempAdapter);
    AutoPtr<IViewOnFocusChangeListener> li = new FocusChange(this);
    //mView->SetOnFocusChangeListener(li);
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

CActivityOne::FocusChange::FocusChange(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{

}

ECode CActivityOne::FocusChange::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (hasFocus) {
        mHost->mView->ShowDropDown();
    }
    return NOERROR;
}

PInterface CActivityOne::FocusChange::Probe(
    /* [in]  */ REIID riid)
{

}

UInt32 CActivityOne::FocusChange::AddRef()
{

}

UInt32 CActivityOne::FocusChange::Release()
{

}

ECode CActivityOne::FocusChange::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}


} // namespace AutoCompleteTextViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
