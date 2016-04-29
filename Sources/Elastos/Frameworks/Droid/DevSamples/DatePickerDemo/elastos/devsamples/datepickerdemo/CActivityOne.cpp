
#include <Elastos.Droid.Graphics.h>
#include "CActivityOne.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::EIID_IViewFactory;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Droid::Widget::IViewAnimator;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DatePickerDemo {

static const String DBG_TAG("CActivityOne");

Int32 CActivityOne::sCount = 0;
Int32 CActivityOne::sIndex = 0;

//=======================================================================
// CActivityOne::ActivityListener
//=======================================================================

CAR_INTERFACE_IMPL_2(CActivityOne::ActivityListener, Object, ISeekBarOnSeekBarChangeListener, IViewOnClickListener)

CActivityOne::ActivityListener::ActivityListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

CActivityOne::ActivityListener::~ActivityListener()
{}

ECode CActivityOne::ActivityListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    Logger::D("CActivityOne::ActivityListener", "OnProgressChanged progress = %d", progress);
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    Logger::D("CActivityOne::ActivityListener", "OnStartTrackingTouch");
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    Logger::D("CActivityOne::ActivityListener", "OnStopTrackingTouch");
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnClick(
    /* [in] */ IView* v)
{
    IView::Probe(mHost->mSwitcher)->SetVisibility(IView::VISIBLE);
    IView::Probe(mHost->mImageSwitch)->SetVisibility(IView::GONE);

    IViewAnimator* va = IViewAnimator::Probe(mHost->mSwitcher);
    if (v == IView::Probe(mHost->mPrev)) {
        va->ShowPrevious();
    }
    else if (v == IView::Probe(mHost->mNext)) {
        va->ShowNext();
    }
    return NOERROR;
}

//============================================================
//CActivityOne::ActivityOneCheckedChangeListener
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::ActivityOneCheckedChangeListener, Object, ICompoundButtonOnCheckedChangeListener);

CActivityOne::ActivityOneCheckedChangeListener::ActivityOneCheckedChangeListener()
{}

CActivityOne::ActivityOneCheckedChangeListener::~ActivityOneCheckedChangeListener()
{}

ECode CActivityOne::ActivityOneCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    Logger::D("CActivityOne::ActivityOneCheckedChangeListener", "OnCheckedChanged is running");
    return NOERROR;
}

//============================================================
//CActivityOne::Switcher
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::Switcher, Object, IViewFactory)

CActivityOne::Switcher::Switcher(
    /* [in] */ IContext* context)
    : mContext(context)
{}

CActivityOne::Switcher::~Switcher()
{}

ECode CActivityOne::Switcher::MakeView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    AutoPtr<IImageView> v;
    CImageView::New(mContext, (IImageView**)&v);
    IView* _view = IView::Probe(v);
    _view->SetBackgroundColor(IColor::WHITE);
    *view = _view;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

//============================================================
//CActivityOne::ImageSwitcherListener
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::ImageSwitcherListener, Object, IViewOnClickListener)

CActivityOne::ImageSwitcherListener::ImageSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

CActivityOne::ImageSwitcherListener::~ImageSwitcherListener()
{}

ECode CActivityOne::ImageSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    IView::Probe(mHost->mImageSwitch)->SetVisibility(IView::VISIBLE);
    IView::Probe(mHost->mSwitcher)->SetVisibility(IView::GONE);
    if (v == IView::Probe(mHost->mImagePrev)) {
        if (sIndex == 0) {
            Int32 size;
            mHost->mList->GetSize(&size);
            sIndex = size - 1;
        }
        AutoPtr<IInterface> obj;
        mHost->mList->Get(sIndex, (IInterface**)&obj);
        mHost->mImageSwitch->SetImageDrawable(IDrawable::Probe(obj));
        --sIndex;
    }
    else if (v == IView::Probe(mHost->mImageNext)) {
        Int32 size;
        mHost->mList->GetSize(&size);
        if (sIndex == size - 1) {
            sIndex = 0;
        }
        AutoPtr<IInterface> obj;
        mHost->mList->Get(sIndex, (IInterface**)&obj);
        mHost->mImageSwitch->SetImageDrawable(IDrawable::Probe(obj));
        ++sIndex;
    }
    return NOERROR;
}

//============================================================
//CActivityOne::TextSwitcherListener
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::TextSwitcherListener, Object, IViewOnClickListener)

CActivityOne::TextSwitcherListener::TextSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

CActivityOne::TextSwitcherListener::~TextSwitcherListener()
{}

ECode CActivityOne::TextSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    IView::Probe(mHost->mImageSwitch)->SetVisibility(IView::GONE);
    IView::Probe(mHost->mSwitcher)->SetVisibility(IView::GONE);
    if (v == IView::Probe(mHost->mImagePrev)) {
        mHost->mTextSwitch->SetText(CoreUtils::Convert("===TextSwitcherListener 1 ==="));
    }
    else if (v == IView::Probe(mHost->mImageNext)) {
        mHost->mTextSwitch->SetText(CoreUtils::Convert("===TextSwitcherListener 2 ==="));
    }
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(DBG_TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Logger::I(DBG_TAG, " >> OnCreate");
    SetContentView(R::layout::main);

    AutoPtr<IView> DatePicker = FindViewById(R::id::DatePicker);
    mDate = IDatePicker::Probe(DatePicker);
    for (Int32 i = 0; i < 10000000; ++i) {
        mDate->UpdateDate(2016, 4, 26);
    }


    AutoPtr<IView> tabhost = FindViewById(R::id::tabhost);
    mTabHost = ITabHost::Probe(tabhost);
    assert(mTabHost);
    mTabHost->Setup();

    AutoPtr<ITabSpec> specone;
    mTabHost->NewTabSpec(String("tab1"), (ITabSpec**)&specone);
    specone->SetIndicator(CoreUtils::Convert("Tab One"));
    specone->SetContent(R::id::view1);
    mTabHost->AddTab(specone);

    AutoPtr<ITabSpec> spectwo;
    mTabHost->NewTabSpec(String("tab2"), (ITabSpec**)&spectwo);
    spectwo->SetIndicator(CoreUtils::Convert("Tab Two"));
    spectwo->SetContent(R::id::view2);
    mTabHost->AddTab(spectwo);

    AutoPtr<ITabSpec> specthree;
    mTabHost->NewTabSpec(String("tab3"), (ITabSpec**)&specthree);
    specthree->SetIndicator(CoreUtils::Convert("Tab Three"));
    specthree->SetContent(R::id::view3);
    mTabHost->AddTab(specthree);


    AutoPtr<IView> bar = FindViewById(R::id::SeekBar);
    mBar = ISeekBar::Probe(bar);

    AutoPtr<IView> ProgressBar = FindViewById(R::id::ProgressBar);
    mProgress = IProgressBar::Probe(ProgressBar);

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
    mImageNext = IImageButton::Probe(imagenext);


    assert(mBar);
    assert(mProgress);
    assert(mNumber);
    assert(mSwitcher);
    assert(mPrev && mNext);
    assert(mImageSwitch && mImagePrev && mImageNext);

    AutoPtr<IViewOnClickListener> vl = new ActivityListener(this);

    AutoPtr<IViewFactory> factory = new Switcher(this);
    IViewSwitcher::Probe(mImageSwitch)->SetFactory(factory);
    AutoPtr<IViewOnClickListener> il = new ImageSwitcherListener(this);
    IView::Probe(mImagePrev)->SetOnClickListener(il);
    IView::Probe(mImageNext)->SetOnClickListener(il);

    CArrayList::New((IList**)&mList);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDrawable> d;
    res->GetDrawable(R::drawable::test0, (IDrawable**)&d);
    mList->Add(d);
    d = NULL;
    res->GetDrawable(R::drawable::test1, (IDrawable**)&d);
    mList->Add(d);
    d = NULL;
    res->GetDrawable(R::drawable::test2, (IDrawable**)&d);
    mList->Add(d);
    d = NULL;
    res->GetDrawable(R::drawable::test3, (IDrawable**)&d);
    mList->Add(d);
    d = NULL;
    res->GetDrawable(R::drawable::test4, (IDrawable**)&d);
    mList->Add(d);
    IView::Probe(mPrev)->SetOnClickListener(vl);
    IView::Probe(mNext)->SetOnClickListener(vl);

    IViewAnimator::Probe(mSwitcher)->SetDisplayedChild(0);

    mNumber->SetMinValue(0);
    mNumber->SetMaxValue(9);
    AutoPtr<ActivityListener> listener = new ActivityListener(this);
    mBar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)listener);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

} // namespace DatePickerDemo
} // namespace DevSamples
} // namespace Elastos
