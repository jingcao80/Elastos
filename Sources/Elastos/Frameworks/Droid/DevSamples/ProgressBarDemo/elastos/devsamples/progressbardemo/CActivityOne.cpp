#include "CActivityOne.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::EIID_IViewFactory;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Droid::Widget::IViewAnimator;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace DevSamples {
namespace ProgressBarDemo {

//============================================================
//     CActivityOne::ActivityOneCheckedChangeListener
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::ActivityOneCheckedChangeListener, Object, ICompoundButtonOnCheckedChangeListener)

CActivityOne::ActivityOneCheckedChangeListener::ActivityOneCheckedChangeListener()
{
}

ECode CActivityOne::ActivityOneCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    PFL_EX("CActivityOne::ActivityOneCheckedChangeListener::OnCheckedChanged is running");
    return NOERROR;
}

//============================================================
//              CActivityOne::Switcher
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::Switcher, Object, IViewFactory)

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
    IView::Probe(v)->SetBackgroundColor(IColor::WHITE);
    *view = IView::Probe(v);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

//============================================================
//              CActivityOne::ImageSwitcherListener
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::ImageSwitcherListener, Object, IViewOnClickListener)

CActivityOne::ImageSwitcherListener::ImageSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::ImageSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    IView::Probe(mHost->mImageSwitch)->SetVisibility(IView::VISIBLE);
    IView::Probe(mHost->mSwitcher)->SetVisibility(IView::GONE);
    Int32 size;
    mHost->mList->GetSize(&size);
    AutoPtr<IInterface> o;
    mHost->mList->Get(index, (IInterface**)&o);
    if (v == IView::Probe(mHost->mImagePrev)) {
        if (index == 0) {
            index = size - 1;
        }
        mHost->mImageSwitch->SetImageDrawable(IDrawable::Probe(o));
        --index;
    }
    else if (v == IView::Probe(mHost->mImageNext)) {
        if (index == size - 1) {
            index = 0;
        }
        mHost->mImageSwitch->SetImageDrawable(IDrawable::Probe(o));
        ++index;
    }
    return NOERROR;
}

//============================================================
//              CActivityOne::TextSwitcherListener
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::TextSwitcherListener, Object, IViewOnClickListener)

CActivityOne::TextSwitcherListener::TextSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::TextSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    IView::Probe(mHost->mImageSwitch)->SetVisibility(IView::GONE);
    IView::Probe(mHost->mSwitcher)->SetVisibility(IView::GONE);
    AutoPtr<ICharSequence> csq;
    if (v == IView::Probe(mHost->mImagePrev)) {
        CString::New(String("jiazhenjiang========================================================"), (ICharSequence**)&csq);
        mHost->mTextSwitch->SetText(csq);
    }
    else if (v == IView::Probe(mHost->mImageNext)) {
        CString::New(String("wangguangming!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!"), (ICharSequence**)&csq);
        mHost->mTextSwitch->SetText(csq);
    }
    return NOERROR;
}

//============================================================
//              CActivityOne::ActivityListener
//============================================================

CAR_INTERFACE_IMPL_2(CActivityOne::ActivityListener, Object, ISeekBarOnSeekBarChangeListener, IViewOnClickListener)

CActivityOne::ActivityListener::ActivityListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
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
    IView::Probe(mHost->mSwitcher)->SetVisibility(IView::VISIBLE);
    IView::Probe(mHost->mImageSwitch)->SetVisibility(IView::GONE);
    if (v == IView::Probe(mHost->mPrev)) {
        IViewAnimator::Probe(mHost->mSwitcher)->ShowPrevious();
    }
    else if ( v == IView::Probe(mHost->mNext)) {
        IViewAnimator::Probe(mHost->mSwitcher)->ShowNext();
    }
    return NOERROR;
}

//============================================================
//     CActivityOne
//============================================================

Int32 CActivityOne::count = 0;
Int32 CActivityOne::index = 0;

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I("CActivityOne", " >> constructor()");
    return Activity::constructor();
}


ECode CActivityOne::OnCreate(
        /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PEL("CActivityOne::OnCreate");
    SetContentView(R::layout::main);

    AutoPtr<IView> ProgressBar = FindViewById(R::id::ProgressBar);
    mProgress = IProgressBar::Probe(ProgressBar);

    assert (mProgress);

    AutoPtr<IView> tabhost;
    FindViewById(R::id::tabhost, (IView**)&tabhost);
    mTabHost = ITabHost::Probe(tabhost);
    assert(mTabHost);
    mTabHost->Setup();

    AutoPtr<ITabSpec> specone;
    mTabHost->NewTabSpec(String("tab1"), (ITabSpec**)&specone);
    AutoPtr<ICharSequence> one;
    CString::New(String("Tab One"), (ICharSequence**)&one);
    specone->SetIndicator(one);
    specone->SetContent(R::id::view1);
    mTabHost->AddTab(specone);

    AutoPtr<ITabSpec> spectwo;
    mTabHost->NewTabSpec(String("tab2"), (ITabSpec**)&spectwo);
    AutoPtr<ICharSequence> two;
    CString::New(String("Tab Two"), (ICharSequence**)&two);
    spectwo->SetIndicator(two);
    spectwo->SetContent(R::id::view2);
    mTabHost->AddTab(spectwo);

    AutoPtr<ITabSpec> specthree;
    mTabHost->NewTabSpec(String("tab3"), (ITabSpec**)&specthree);
    AutoPtr<ICharSequence> three;
    CString::New(String("Tab Three"), (ICharSequence**)&three);
    specthree->SetIndicator(three);
    specthree->SetContent(R::id::view3);
    mTabHost->AddTab(specthree);

    AutoPtr<IView> DatePicker;
    FindViewById(R::id::DatePicker, (IView**)&DatePicker);
    mDate = IDatePicker::Probe(DatePicker);
    AutoPtr<IView> bar;
    FindViewById(R::id::SeekBar, (IView**)&bar);
    mBar = ISeekBar::Probe(bar);


    AutoPtr<IView> num;
    FindViewById(R::id::NumberPicker, (IView**)&num);
    mNumber = INumberPicker::Probe(num);

    AutoPtr<IView> timepicker;
    FindViewById(R::id::TimePicker, (IView**)&timepicker);
    mTime = ITimePicker::Probe(timepicker);
    AutoPtr<IView> switcher;
    FindViewById(R::id::ViewSwitcher, (IView**)&switcher);
    mSwitcher = IViewSwitcher::Probe(switcher);

    AutoPtr<IView> prev;
    FindViewById(R::id::prev, (IView**)&prev);
    AutoPtr<IView> next;
    FindViewById(R::id::next, (IView**)&next);
    mPrev = IImageButton::Probe(prev);
    mNext = IImageButton::Probe(next);
    AutoPtr<IView> imageswitcher;
    FindViewById(R::id::ImageSwitcher, (IView**)&imageswitcher);
    mImageSwitch = IImageSwitcher::Probe(imageswitcher);

    AutoPtr<IView> imageprev;
    FindViewById(R::id::last, (IView**)&imageprev);
    AutoPtr<IView> imagenext;
    FindViewById(R::id::next, (IView**)&imagenext);
    mImagePrev = IImageButton::Probe(imageprev);
    mImageNext = IImageButton::Probe(imagenext);

    assert(mBar);
    assert (mNumber);
    assert(mSwitcher);
    assert(mPrev && mNext);
    assert(mImageSwitch && mImagePrev && mImageNext);

    AutoPtr<ActivityListener> al = new ActivityListener(this);
    AutoPtr<IViewOnClickListener> vl = (IViewOnClickListener*)al.Get();

    AutoPtr<Switcher> sr = new Switcher(this);
    AutoPtr<IViewFactory> factory = (IViewFactory*)sr.Get();

    IViewSwitcher::Probe(mImageSwitch)->SetFactory(factory);
    AutoPtr<ImageSwitcherListener> isl = new ImageSwitcherListener(this);

    AutoPtr<IViewOnClickListener> il = (IViewOnClickListener*)isl.Get();
    IView::Probe(mImagePrev)->SetOnClickListener(il);
    IView::Probe(mImageNext)->SetOnClickListener(il);
    AutoPtr<IDrawable> d;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDrawable(R::drawable::test0, (IDrawable**)&d);
    mList->Add(d);
    res->GetDrawable(R::drawable::test1, (IDrawable**)&d);
    mList->Add(d);
    res->GetDrawable(R::drawable::test2, (IDrawable**)&d);
    mList->Add(d);
    res->GetDrawable(R::drawable::test3, (IDrawable**)&d);
    mList->Add(d);
    res->GetDrawable(R::drawable::test4, (IDrawable**)&d);
    mList->Add(d);
    IView::Probe(mPrev)->SetOnClickListener(vl);
    IView::Probe(mNext)->SetOnClickListener(vl);

    IViewAnimator::Probe(mSwitcher)->SetDisplayedChild(0);

    mNumber->SetMinValue(0);
    mNumber->SetMaxValue(9);
    AutoPtr<ActivityListener> listener = new ActivityListener(this);
    mBar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)listener.Get());
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

} // namespace ProgressBarDemo
} // namespace DevSamples
} // namespace Elastos
