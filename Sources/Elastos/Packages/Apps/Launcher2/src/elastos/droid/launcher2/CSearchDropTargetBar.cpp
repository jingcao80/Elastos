
#include "elastos/droid/launcher2/CSearchDropTargetBar.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CSearchDropTargetBar::MyAdapter::MyAdapter(
    /* [in] */ IView* v)
    : mV(v)
{
}

ECode CSearchDropTargetBar::MyAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mV->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
}

const Int32 CSearchDropTargetBar::sTransitionInDuration = 200;
const Int32 CSearchDropTargetBar::sTransitionOutDuration = 175;

static AutoPtr<IAccelerateInterpolator> InitAccelerateInterpolator()
{
    AutoPtr<IAccelerateInterpolator> tmp;
    CAccelerateInterpolator::New((IAccelerateInterpolator**)&tmp);
    return tmp;
}

AutoPtr<IAccelerateInterpolator> CSearchDropTargetBar::sAccelerateInterpolator =
        InitAccelerateInterpolator();

CAR_INTERFACE_IMPL_2(CSearchDropTargetBar, FrameLayout, ISearchDropTargetBar, IDragControllerDragListener);

CAR_OBJECT_IMPL(CSearchDropTargetBar);

CSearchDropTargetBar::CSearchDropTargetBar()
    : mIsSearchBarHidden(FALSE)
    , mBarHeight(0)
    , mDeferOnDragEnd(FALSE)
    , mEnableDropDownDropTargets(FALSE)
{
}

ECode CSearchDropTargetBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CSearchDropTargetBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return FrameLayout::constructor(context, attrs, defStyle);
}

ECode CSearchDropTargetBar::Setup(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IDragController* dragController)
{
    dragController->AddDragListener(this);
    dragController->AddDragListener(IDragControllerDragListener::Probe(mInfoDropTarget));
    dragController->AddDragListener(IDragControllerDragListener::Probe(mDeleteDropTarget));
    dragController->AddDropTarget(IDropTarget::Probe(mInfoDropTarget));
    dragController->AddDropTarget(IDropTarget::Probe(mDeleteDropTarget));
    dragController->SetFlingToDeleteDropTarget(IDropTarget::Probe(mDeleteDropTarget));
    mInfoDropTarget->SetLauncher(launcher);
    return mDeleteDropTarget->SetLauncher(launcher);
}

void CSearchDropTargetBar::PrepareStartAnimation(
    /* [in] */ IView* v)
{
    // Enable the hw layers before the animation starts (will be disabled in the onAnimationEnd
    // callback below)
    v->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
}

void CSearchDropTargetBar::SetupAnimation(
    /* [in] */ IObjectAnimator* anim,
    /* [in] */ IView* v)
{
    IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(sAccelerateInterpolator));
    IAnimator::Probe(anim)->SetDuration(sTransitionInDuration);
    AutoPtr<IAnimatorListener> adapter = new MyAdapter(v);
Slogger::D("CAppsCustomizeTabHost", "=======CAppsCustomizeTabHost::SetupAnimation adapter=%p",adapter.Get());
    IAnimator::Probe(anim)->AddListener(adapter);
}

ECode CSearchDropTargetBar::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();

    // Get the individual components
    FindViewById(Elastos::Droid::Launcher2::R::id::qsb_search_bar, (IView**)&mQSBSearchBar);
    FindViewById(Elastos::Droid::Launcher2::R::id::drag_target_bar, (IView**)&mDropTargetBar);

    AutoPtr<IView> tmp;
    mDropTargetBar->FindViewById(Elastos::Droid::Launcher2::R::id::info_target_text, (IView**)&tmp);
    mInfoDropTarget = IButtonDropTarget::Probe(tmp);
    AutoPtr<IView> tmp2;
    mDropTargetBar->FindViewById(Elastos::Droid::Launcher2::R::id::delete_target_text, (IView**)&tmp2);
    mDeleteDropTarget = IButtonDropTarget::Probe(tmp2);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::qsb_bar_height, &mBarHeight);

    mInfoDropTarget->SetSearchDropTargetBar(this);
    mDeleteDropTarget->SetSearchDropTargetBar(this);

    resources->GetBoolean(Elastos::Droid::Launcher2::R::bool_::config_useDropTargetDownTransition,
            &mEnableDropDownDropTargets);

    // Create the various fade animations
    if (mEnableDropDownDropTargets) {
        mDropTargetBar->SetTranslationY(-mBarHeight);
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = (Float)(-mBarHeight);
        (*array)[1] = 0.0f;
        helper->OfFloat(TO_IINTERFACE(mDropTargetBar), String("translationY"),
                array, (IObjectAnimator**)&mDropTargetBarAnim);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(2);
        (*array2)[0] = 0.0f;
        (*array2)[1] = (Float)(-mBarHeight);
        helper->OfFloat(TO_IINTERFACE(mQSBSearchBar), String("translationY"),
                array2, (IObjectAnimator**)&mQSBSearchBarAnim);
    }
    else {
        mDropTargetBar->SetAlpha(0.0f);
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = 0.0;
        (*array)[1] = 1.0f;
        helper->OfFloat(TO_IINTERFACE(mDropTargetBar), String("alpha"),
                array, (IObjectAnimator**)&mDropTargetBarAnim);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(2);
        (*array2)[0] = 1.0;
        (*array2)[1] = 0.0f;
        helper->OfFloat(TO_IINTERFACE(mQSBSearchBar), String("alpha"),
                array2, (IObjectAnimator**)&mQSBSearchBarAnim);
    }
    SetupAnimation(mDropTargetBarAnim, mDropTargetBar);
    SetupAnimation(mQSBSearchBarAnim, mQSBSearchBar);
    return NOERROR;
}

ECode CSearchDropTargetBar::FinishAnimations()
{
    PrepareStartAnimation(mDropTargetBar);
    IAnimator::Probe(mDropTargetBarAnim)->Reverse();
    PrepareStartAnimation(mQSBSearchBar);
    return IAnimator::Probe(mQSBSearchBarAnim)->Reverse();
}

ECode CSearchDropTargetBar::ShowSearchBar(
    /* [in] */ Boolean animated)
{
    if (!mIsSearchBarHidden) {
        return NOERROR;
    }
    if (animated) {
        PrepareStartAnimation(mQSBSearchBar);
        IAnimator::Probe(mQSBSearchBarAnim)->Reverse();
    }
    else {
        IAnimator::Probe(mQSBSearchBarAnim)->Cancel();
        if (mEnableDropDownDropTargets) {
            mQSBSearchBar->SetTranslationY(0);
        } else {
            mQSBSearchBar->SetAlpha(1.0f);
        }
    }
    mIsSearchBarHidden = FALSE;
    return NOERROR;
}

ECode CSearchDropTargetBar::HideSearchBar(
    /* [in] */ Boolean animated)
{
    if (mIsSearchBarHidden) {
        return NOERROR;
    }
    if (animated) {
        PrepareStartAnimation(mQSBSearchBar);
        IAnimator::Probe(mQSBSearchBarAnim)->Start();
    }
    else {
        IAnimator::Probe(mQSBSearchBarAnim)->Cancel();
        if (mEnableDropDownDropTargets) {
            mQSBSearchBar->SetTranslationY(-mBarHeight);
        }
        else {
            mQSBSearchBar->SetAlpha(0.0f);
        }
    }
    mIsSearchBarHidden = TRUE;
    return NOERROR;
}

ECode CSearchDropTargetBar::GetTransitionInDuration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = sTransitionInDuration;
    return NOERROR;
}

ECode CSearchDropTargetBar::GetTransitionOutDuration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = sTransitionOutDuration;
    return NOERROR;
}

ECode CSearchDropTargetBar::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    // Animate out the QSB search bar, and animate in the drop target bar
    PrepareStartAnimation(mDropTargetBar);
    IAnimator::Probe(mDropTargetBarAnim)->Start();
    if (!mIsSearchBarHidden) {
        PrepareStartAnimation(mQSBSearchBar);
        IAnimator::Probe(mQSBSearchBarAnim)->Start();
    }
    return NOERROR;
}

ECode CSearchDropTargetBar::DeferOnDragEnd()
{
    mDeferOnDragEnd = TRUE;
    return NOERROR;
}

ECode CSearchDropTargetBar::OnDragEnd()
{
    if (!mDeferOnDragEnd) {
        // Restore the QSB search bar, and animate out the drop target bar
        PrepareStartAnimation(mDropTargetBar);
        IAnimator::Probe(mDropTargetBarAnim)->Reverse();
        if (!mIsSearchBarHidden) {
            PrepareStartAnimation(mQSBSearchBar);
            IAnimator::Probe(mQSBSearchBarAnim)->Reverse();
        }
    }
    else {
        mDeferOnDragEnd = FALSE;
    }
    return NOERROR;
}

ECode CSearchDropTargetBar::OnSearchPackagesChanged(
    /* [in] */ Boolean searchVisible,
    /* [in] */ Boolean voiceVisible)
{
    if (mQSBSearchBar != NULL) {
        AutoPtr<IDrawable> bg;
        mQSBSearchBar->GetBackground((IDrawable**)&bg);
        if (bg != NULL && (!searchVisible && !voiceVisible)) {
            // Save the background and disable it
            mPreviousBackground = bg;
            return mQSBSearchBar->SetBackgroundResource(0);
        }
        else if (mPreviousBackground != NULL && (searchVisible || voiceVisible)) {
            // Restore the background
            return mQSBSearchBar->SetBackground(mPreviousBackground);
        }
    }
    return NOERROR;
}

ECode CSearchDropTargetBar::GetSearchBarBounds(
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (mQSBSearchBar != NULL) {
        AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Alloc(2);
        mQSBSearchBar->GetLocationOnScreen(pos);

        AutoPtr<IRect> rect;
        CRect::New((IRect**)&rect);
        rect->SetLeft((*pos)[0]);
        rect->SetTop((*pos)[1]);
        Int32 width;
        mQSBSearchBar->GetWidth(&width);
        rect->SetRight((*pos)[0] + width);
        Int32 height;
        mQSBSearchBar->GetHeight(&height);
        rect->SetBottom((*pos)[1] + height);
        *result = rect;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        *result = NULL;
        return NOERROR;
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos