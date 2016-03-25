#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.Core.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Widget::EIID_IViewFactory;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ImageButtonDemo {

Int32 CActivityOne::count = 0;
Int32 CActivityOne::index = 0;

ECode CActivityOne::OnCreate(
        /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PEL("CActivityOne::OnCreate");
    SetContentView(R::layout::main);

    AutoPtr<IView> ProgressBar = FindViewById(R::id::ProgressBar);
    mProgress = IProgressBar::Probe(ProgressBar);

    assert (mProgress);

    /*AutoPtr<IView> tabhost = FindViewById(R::id::tabhost);
    mTabHost = ITabHost::Probe(tabhost);
    assert(mTabHost);
    mTabHost->Setup();

    AutoPtr<ITabSpec> specone;
    mTabHost->NewTabSpec(String("tab1"), (ITabSpec**)&specone);
    AutoPtr<ICharSequence> one;
    CStringWrapper::New(String("Tab One"), (ICharSequence**)&one);
    specone->SetIndicator(one);
    specone->SetContent(R::id::view1);
    mTabHost->AddTab(specone);

    AutoPtr<ITabSpec> spectwo;
    mTabHost->NewTabSpec(String("tab2"), (ITabSpec**)&spectwo);
    AutoPtr<ICharSequence> two;
    CStringWrapper::New(String("Tab Two"), (ICharSequence**)&two);
    spectwo->SetIndicator(two);
    spectwo->SetContent(R::id::view2);
    mTabHost->AddTab(spectwo);

    AutoPtr<ITabSpec> specthree;
    mTabHost->NewTabSpec(String("tab3"), (ITabSpec**)&specthree);
    AutoPtr<ICharSequence> three;
    CStringWrapper::New(String("Tab Three"), (ICharSequence**)&three);
    specthree->SetIndicator(three);
    specthree->SetContent(R::id::view3);
    mTabHost->AddTab(specthree);*/

    /*AutoPtr<IView> DatePicker = FindViewById(R::id::DatePicker);
    mDate = IDatePicker::Probe(DatePicker);*/
    /*AutoPtr<IView> bar = FindViewById(R::id::SeekBar);
    mBar = ISeekBar::Probe(bar);



    AutoPtr<IView> num = FindViewById(R::id::NumberPicker);
    mNumber = INumberPicker::Probe(num);

    AutoPtr<IView> timepicker = FindViewById(R::id::TimePicker);
    mTime = ITimePicker::Probe(timepicker);
    AutoPtr<IView> switcher = FindViewById(R::id::ViewSwitcher);
    mSwitcher = IViewSwitcher::Probe(switcher);

    AutoPtr<IView> prev = FindViewById(R::id::prev);
    AutoPtr<IView> next = FindViewById(R::id::next);
    mPrev = IImageButton::Probe(prev);
    mNext = IImageButton::Probe(next);
    AutoPtr<IView> imageswitcher = FindViewById(R::id::ImageSwitcher);
    mImageSwitch = IImageSwitcher::Probe(imageswitcher);

    AutoPtr<IView> imageprev = FindViewById(R::id::last);
    AutoPtr<IView> imagenext = FindViewById(R::id::next);
    mImagePrev = IImageButton::Probe(imageprev);
    mImageNext = IImageButton::Probe(imagenext);*/

    /*assert(mBar);

    assert (mNumber);
    assert(mSwitcher);
    assert(mPrev && mNext);
    assert(mImageSwitch && mImagePrev && mImageNext);

    AutoPtr<IViewOnClickListener> vl = new ActivityListener(this);

    AutoPtr<IViewFactory> factory = new Switcher(this);
    mImageSwitch->SetFactory(factory);
    AutoPtr<IViewOnClickListener> il = new ImageSwitcherListener(this);
    mImagePrev->SetOnClickListener(il);
    mImageNext->SetOnClickListener(il);
    AutoPtr<IDrawable> d;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDrawable(R::drawable::test0, (IDrawable**)&d);
    mList.PushBack(d);
    res->GetDrawable(R::drawable::test1, (IDrawable**)&d);
    mList.PushBack(d);
    res->GetDrawable(R::drawable::test2, (IDrawable**)&d);
    mList.PushBack(d);
    res->GetDrawable(R::drawable::test3, (IDrawable**)&d);
    mList.PushBack(d);
    res->GetDrawable(R::drawable::test4, (IDrawable**)&d);
    mList.PushBack(d);
    mPrev->SetOnClickListener(vl);
    mNext->SetOnClickListener(vl);

    mSwitcher->SetDisplayedChild(0);

    mNumber->SetMinValue(0);
    mNumber->SetMaxValue(9);
    AutoPtr<ActivityListener> listener = new ActivityListener(this);
    mBar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)listener->Probe(EIID_ISeekBarOnSeekBarChangeListener));*/
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    PEL("CActivityOne::OnStart");
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PEL("CActivityOne::OnResume");
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PEL("CActivityOne::OnPause");
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PEL("CActivityOne::OnStop");
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PEL("CActivityOne::OnDestroy");
    return NOERROR;
}

//====================================================
CActivityOne::ActivityListener::ActivityListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{

}

PInterface CActivityOne::ActivityListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    } else  if (riid == EIID_ISeekBarOnSeekBarChangeListener) {
        return this;
    } else if (riid == EIID_IViewOnClickListener) {
        return this;
    }
    return NULL;
}

UInt32 CActivityOne::ActivityListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::ActivityListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::ActivityListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pObject == this) {
        *pIID = EIID_ISeekBarOnSeekBarChangeListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    PRINT_FILE_LINE_EX("OnProgressChanged progress = %d", progress);
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    PRINT_FILE_LINE_EX("CActivityOne::ActivityListener::OnStartTrackingTouch");
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    PRINT_FILE_LINE_EX("CActivityOne::ActivityListener::OnStopTrackingTouch");
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnClick(
    /* [in] */ IView* v)
{
    /*mHost->mSwitcher->SetVisibility(IView::VISIBLE);
    mHost->mImageSwitch->SetVisibility(IView::GONE);
    if (v == mHost->mPrev) {
        mHost->mSwitcher->ShowPrevious();
    } else if ( v == mHost->mNext) {
        mHost->mSwitcher->ShowNext();
    }*/
}

//============================================================
//              CActivityOne::Switcher
//============================================================

CActivityOne::Switcher::Switcher(
    /* [in] */ IContext* context)
    : mContext(context)
{

}

ECode CActivityOne::Switcher::MakeView(
    /* [out] */ IView** view)
{
    AutoPtr<IImageView> v;
    CImageView::New(mContext, (IImageView**)&v);
    v->SetBackgroundColor(IColor::WHITE);
    *view = v;
    REFCOUNT_ADD(*view);
}

PInterface CActivityOne::Switcher::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    } else if (riid == EIID_IViewFactory) {
        return this;
    }
    return NULL;
}

UInt32 CActivityOne::Switcher::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::Switcher::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::Switcher::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

//============================================================
//              CActivityOne::ImageSwitcherListener
//============================================================

CActivityOne::ImageSwitcherListener::ImageSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{

}

PInterface CActivityOne::ImageSwitcherListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    } else if (riid == EIID_IViewOnClickListener) {
        return this;
    }
    return NULL;
}

UInt32 CActivityOne::ImageSwitcherListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::ImageSwitcherListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::ImageSwitcherListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

ECode CActivityOne::ImageSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    /*mHost->mImageSwitch->SetVisibility(IView::VISIBLE);
    mHost->mSwitcher->SetVisibility(IView::GONE);
    if (v == mHost->mImagePrev) {
        if (index == 0) {
            index = mHost->mList.GetSize()-1;
        }
        mHost->mImageSwitch->SetImageDrawable(mHost->mList[index]);
        --index;
    } else if (v == mHost->mImageNext) {
        if (index == mHost->mList.GetSize()-1) {
            index = 0;
        }
        mHost->mImageSwitch->SetImageDrawable(mHost->mList[index]);
        ++index;
    }*/
}

//============================================================
//              CActivityOne::TextSwitcherListener
//============================================================

CActivityOne::TextSwitcherListener::TextSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{

}

PInterface CActivityOne::TextSwitcherListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    } else if (riid == EIID_IViewOnClickListener) {
        return this;
    }
    return NULL;
}

UInt32 CActivityOne::TextSwitcherListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::TextSwitcherListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::TextSwitcherListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

ECode CActivityOne::TextSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    /*mHost->mImageSwitch->SetVisibility(IView::GONE);
    mHost->mSwitcher->SetVisibility(IView::GONE);
    AutoPtr<ICharSequence> csq;
    if (v == mHost->mImagePrev) {
        CStringWrapper::New(String("jiazhenjiang========================================================"), (ICharSequence**)&csq);
        mHost->mTextSwitch->SetText(csq);
    } else if (v == mHost->mImageNext) {
        CStringWrapper::New(String("wangguangming!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!"), (ICharSequence**)&csq);
        mHost->mTextSwitch->SetText(csq);
    }*/
}

//============================================================
//     CActivityOne::ActivityOneCheckedChangeListener
//============================================================

CActivityOne::ActivityOneCheckedChangeListener::ActivityOneCheckedChangeListener()
{

}

PInterface CActivityOne::ActivityOneCheckedChangeListener::Probe(
    /* [in]  */ REIID riid)
{

}

UInt32 CActivityOne::ActivityOneCheckedChangeListener::AddRef()
{

}

UInt32 CActivityOne::ActivityOneCheckedChangeListener::Release()
{

}

ECode CActivityOne::ActivityOneCheckedChangeListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

ECode CActivityOne::ActivityOneCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    PFL_EX("CActivityOne::ActivityOneCheckedChangeListener::OnCheckedChanged is running");
}

} // namespace SurfaceViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
