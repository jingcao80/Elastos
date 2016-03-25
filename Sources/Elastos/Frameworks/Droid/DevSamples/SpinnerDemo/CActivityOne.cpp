#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <stdio.h>
#include "R.h"

using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SpinnerDemo {

MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

PInterface MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IAdapterViewOnItemSelectedListener) {
        return this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }
    return NULL;
}

UInt32 MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 MyListener::Release()
{
    return ElRefBase::Release();
}

ECode MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IAdapterViewOnItemSelectedListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode MyListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    PEL(" MyListener::OnItemSelected")
    Int32 parentId = -1, viewId = -1;
    if (parent) parent->GetId(&parentId);
    if (view) view->GetId(&viewId);

    PFL_EX("Parent: %08x OnItemSelected %08x at %d, id: %d.", parentId, viewId, position, id);
    return NOERROR;
}

ECode MyListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    Int32 parentId = -1;
    if (parent) parent->GetId(&parentId);

    PFL_EX("Parent: %08x OnNothingSelected.", parentId);
    return NOERROR;
}

ECode MyListener::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);

    if (id == R::id::toastButton) {
        mHost->ShowToast();
    }
    return NOERROR;
}


//=============================================================================================

ECode CActivityOne::ShowToast()
{
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);

    String msg("测试:This is an Android Toast Message!");
    AutoPtr<ICharSequence> seq;
    CStringWrapper::New(msg, (ICharSequence**)&seq);

    AutoPtr<IToast> toast;
    helper->MakeText(context, seq, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Slogger::D("CActivityOne", " >> OnCreate...");

    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    IAdapterViewOnItemSelectedListener* selectListener = (IAdapterViewOnItemSelectedListener*)(l->Probe(EIID_IAdapterViewOnItemSelectedListener));
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

    // Spinner
    //
    AutoPtr<IView> temp = FindViewById(R::id::spinner);
    AutoPtr<ISpinner> sp = ISpinner::Probe(temp.Get());
    sp->SetOnItemSelectedListener(selectListener);

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);
    for (Int32 i = 0; i < 5; ++i) {
        StringBuilder sb("Item ");
        sb += i;
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq->Probe(EIID_IInterface));
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::simple_spinner_item, dataList, (IArrayAdapter**)&adapter);
    Int32 aCount;
    adapter->GetCount(&aCount);
    sp->SetAdapter(adapter);

    // Button
    //
    temp = FindViewById(R::id::toastButton);
    assert(temp != NULL);
    temp->SetOnClickListener(clickListener);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Slogger::D("CActivityOne", " >> OnStart...");
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    Slogger::D("CActivityOne", " >> OnResume...");
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    Slogger::D("CActivityOne", " >> OnPause...");
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    Slogger::D("CActivityOne", " >> OnStop...");
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    Slogger::D("CActivityOne", " >> OnDestroy...");
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Slogger::D("CActivityOne", " >> OnActivityResult...");
    return NOERROR;
}

} // namespace SpinnerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
