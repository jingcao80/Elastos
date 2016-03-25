
#include "CActivityOne.h"
#include "R.h"
//#include "Uri.h"
#include <elautoptr.h>
#include <stdio.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::EIID_IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::App::IIActivityManager;

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace TextViewTest {

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
        return (PInterface)this;
    }
    else if (riid == EIID_IRadioGroupOnCheckedChangeListener) {
        return this;
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

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IRadioGroupOnCheckedChangeListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnCheckedChanged(
        /* [in] */ IRadioGroup* group,
        /* [in] */ Int32 checkedId)
{
    PRINT_FILE_LINE

    // RadioGroup language
    if(checkedId == R::id::radio_button_language_english) {
        mHost->mTextView->SetText(R::string::text_english);
    }
    if(checkedId == R::id::radio_button_language_chinese) {
        mHost->mTextView->SetText(R::string::text_chinese);
    }

    // RadionGroup gravity
    if(checkedId == R::id::radio_button_gravity_top) {
        mHost->mTextView->SetGravity(IGravity::TOP);
    }
    if(checkedId == R::id::radio_button_gravity_bottom) {
        mHost->mTextView->SetGravity(IGravity::BOTTOM);
    }
    if(checkedId == R::id::radio_button_gravity_left) {
        mHost->mTextView->SetGravity(IGravity::LEFT);
    }
    if(checkedId == R::id::radio_button_gravity_right) {
        mHost->mTextView->SetGravity(IGravity::RIGHT);
    }
    if(checkedId == R::id::radio_button_gravity_center) {
        mHost->mTextView->SetGravity(IGravity::CENTER);
    }
    if(checkedId == R::id::radio_button_gravity_center_vertical) {
        mHost->mTextView->SetGravity(IGravity::CENTER_VERTICAL);
    }
    if(checkedId == R::id::radio_button_gravity_center_horizontal) {
        mHost->mTextView->SetGravity(IGravity::CENTER_HORIZONTAL);
    }

    PRINT_FILE_LINE

    return NOERROR;
}

// #include <unistd.h>

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    GetResources((IResources**)&resources);

    AutoPtr<IView> view = FindViewById(R::id::text_view);
    assert(view != NULL);
    mTextView = ITextView::Probe(view);
    mTextView->SetText(R::string::text_english);

    view = FindViewById(R::id::radio_group_language);
    assert(view != NULL);
    mRadioGroupLanguage = IRadioGroup::Probe(view);
    mRadioGroupLanguage->Check(R::id::radio_button_language_english);

    view = FindViewById(R::id::radio_group_gravity);
    assert(view != NULL);
    mRadioGroupGravity = IRadioGroup::Probe(view);
    mRadioGroupGravity->Check(R::id::radio_button_gravity_top);

    AutoPtr<MyListener> l = new MyListener(this);
    mRadioGroupLanguage->SetOnCheckedChangeListener(l.Get());
    mRadioGroupGravity->SetOnCheckedChangeListener(l.Get());

    return NOERROR;
}

ECode CActivityOne::OnStart()
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

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace ImageViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
