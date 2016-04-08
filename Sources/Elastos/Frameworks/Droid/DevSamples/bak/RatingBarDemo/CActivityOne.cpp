#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <elastos/core/StringBuilder.h>
#include <stdio.h>
#include "R.h"

using Elastos::Droid::Widget::EIID_IOnRatingBarChangeListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace RatingBarDemo {

CActivityOne::RatingBarListener::RatingBarListener(
    /* [in] */ CActivityOne* host)
{
    mHost = host;
}

PInterface CActivityOne::RatingBarListener::Probe(
    /* [in] */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return (IInterface*)(IOnRatingBarChangeListener *)this;
    }
    else if ( riid == EIID_IOnRatingBarChangeListener ) {
        return (IOnRatingBarChangeListener *)this;
    }
    return NULL;
}

UInt32 CActivityOne::RatingBarListener::AddRef()
{
    return ElRefBase::AddRef();
}
UInt32 CActivityOne::RatingBarListener::Release()
{
    return ElRefBase::Release();
}
ECode CActivityOne::RatingBarListener::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(IOnRatingBarChangeListener *)this) {
        *iid = EIID_IOnRatingBarChangeListener ;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CActivityOne::RatingBarListener::OnRatingChanged(
    /* [in] */ IRatingBar* ratingBar,
    /* [in] */ Float rating,
    /* [in] */ Boolean fromUser)
{
    PFL_EX("CActivityOne::RatingBarListener::OnRatingChanged, rating: %f", rating);
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PRINT_ENTER_LEAVE("CActivityOne::OnCreate");
    SetContentView(R::layout::main);
    AutoPtr<IView> temp = FindViewById(R::id::ratingBar);
    AutoPtr<IRatingBar> bar = IRatingBar::Probe(temp);
    AutoPtr<IOnRatingBarChangeListener> listener = new RatingBarListener(this);
    bar->SetOnRatingBarChangeListener(listener);
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

} // namespace AutoCompleteTextViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
