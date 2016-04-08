
#include "CActivityOne.h"
#include "R.h"
#include <elautoptr.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ActivityTest {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(String("ActivityTest"), String("ActivityTest.CActivityTwo"));
    ECode ec = mHost->StartActivity(intent);
    // if (FAILED(mHost->StartActivity(intent))) {
    //     Slogger::E("CActivityOne", "StartActivity---CActivityTwo---failed");
    // }
Slogger::D("CActivityOne", "===============================OnClick===========1, ec=[0x%08x]", ec);
    return NOERROR;
}

CActivityOne::CActivityOne()
    : mState(0)
{}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
Slogger::D("CActivityOne", "=================OnCreate, savedInstanceState=[%p]", savedInstanceState);
    Activity::OnCreate(savedInstanceState);

    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

    mStartNewActivity = FindViewById(R::id::newActivity);
    assert(mStartNewActivity != NULL);
    mStartNewActivity->SetOnClickListener(clickListener);

    mTextValue = FindViewById(R::id::RestoreInstanceState);
    assert(mTextValue != NULL);
    if (savedInstanceState != NULL) {
        ITextView::Probe(mTextValue)->SetText(NULL);
    }

    return NOERROR;
}

ECode CActivityOne::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnRestoreInstanceState(savedInstanceState);
    assert(savedInstanceState != NULL);
    savedInstanceState->GetInt32(String("ActivityTest.CActivityOne"), &mState);
Slogger::D("CActivityOne", "OnRestoreInstanceState, mState=====[%d]", mState);
    assert(ITextView::Probe(mTextValue) != NULL);
    AutoPtr<ICharSequence> txt;
    StringBuilder sb;
    sb.Append(String("OnRestoreInstanceState value:["));
    sb.Append(mState);
    sb.AppendChar(']');

    CStringWrapper::New(sb.ToString(), (ICharSequence**)&txt);
    ITextView::Probe(mTextValue)->SetText(txt);
    return NOERROR;
}

ECode CActivityOne::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    mState = 123456;
    outState->PutInt32(String("ActivityTest.CActivityOne"), mState);
Slogger::D("CActivityOne", "OnSaveInstanceState, mState====[%d]", mState);
    return Activity::OnSaveInstanceState(outState);
}

ECode CActivityOne::OnStart()
{
Slogger::D("CActivityOne", "=============================================OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
Slogger::D("CActivityOne", "=============================================OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
Slogger::D("CActivityOne", "=============================================OnPause");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
Slogger::D("CActivityOne", "=============================================OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
Slogger::D("CActivityOne", "=============================================OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace ActivityTest
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
