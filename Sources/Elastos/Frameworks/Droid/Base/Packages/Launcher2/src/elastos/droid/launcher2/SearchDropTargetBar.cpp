
#include "elastos/droid/launcher2/SearchDropTargetBar.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

MyAdapter::MyAdapter(
    /* [in] */ IView* v)
    : mV(v)
{
}

CARAPI MyAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mV->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
}

const Int32 SearchDropTargetBar::sTransitionInDuration = 200;
const Int32 SearchDropTargetBar::sTransitionOutDuration = 175;

static AutoPtr<IAccelerateInterpolator> InitAccelerateInterpolator()
{
    AutoPtr<IAccelerateInterpolator> tmp;
    CAccelerateInterpolator::New((IAccelerateInterpolator**)&tmp);
    return tmp;
}

AutoPtr<IAccelerateInterpolator> sAccelerateInterpolator = InitAccelerateInterpolator();

CAR_INTERFACE_IMPL_2(SearchDropTargetBar, FrameLayout, ISearchDropTargetBar, IDragControllerDragListener);

SearchDropTargetBar::SearchDropTargetBar(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mIsSearchBarHidden(FALSE)
    , mBarHeight(0)
    , mDeferOnDragEnd(FALSE)
    , mEnableDropDownDropTargets(FALSE)
{
    SearchDropTargetBar(context, attrs, 0);
}

SearchDropTargetBar::SearchDropTargetBar(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mIsSearchBarHidden(FALSE)
    , mBarHeight(0)
    , mDeferOnDragEnd(FALSE)
    , mEnableDropDownDropTargets(FALSE)
{
    FrameLayout(context, attrs, defStyle);
}

ECode SearchDropTargetBar::Setup(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IDragController* dragController)
{
    dragController->AddDragListener(this);
    dragController->AddDragListener(mInfoDropTarget);
    dragController->AddDragListener(mDeleteDropTarget);
    dragController->AddDropTarget(mInfoDropTarget);
    dragController->AddDropTarget(mDeleteDropTarget);
    dragController->SetFlingToDeleteDropTarget(mDeleteDropTarget);
    mInfoDropTarget->SetLauncher(launcher);
    return mDeleteDropTarget->SetLauncher(launcher);
}

void SearchDropTargetBar::PrepareStartAnimation(
    /* [in] */ IView* v)
{
    // Enable the hw layers before the animation starts (will be disabled in the onAnimationEnd
    // callback below)
    v->SetLayerType(View.LAYER_TYPE_HARDWARE, NULL);
}

void SearchDropTargetBar::SetupAnimation(
    /* [in] */ IObjectAnimator* anim,
    /* [in] */ IView* v)
{
    anim->SetInterpolator(sAccelerateInterpolator);
    anim->SetDuration(sTransitionInDuration);

    IAnimator::Probe(anim)->AddListener(IAnimatorListener::Probe((MyAdapter));
}

ECode SearchDropTargetBar::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();

    // Get the individual components
    FindViewById(R.id.qsb_search_bar, (IView**)&mQSBSearchBar);
    FindViewById(R.id.drag_target_bar, (IView**)&mDropTargetBar);

    AutoPtr<IView> tmp;
    mDropTargetBar->FindViewById(R.id.info_target_text, (IView**)&tmp);
    mInfoDropTarget = IButtonDropTarget::Probe(tmp);
    AutoPtr<IView> tmp2;
    mDropTargetBar->FindViewById(R.id.delete_target_text (IView**)&tmp2);
    mDeleteDropTarget = IButtonDropTarget::Probe(tmp2);
    AutoPtr<IResources> resources;
    getResources((IResources**)&resources);
    resources->GetDimensionPixelSize(R.dimen.qsb_bar_height, &mBarHeight);

    mInfoDropTarget->SetSearchDropTargetBar(this);
    mDeleteDropTarget->SetSearchDropTargetBar(this);

    resources->GetBoolean(R.bool.config_useDropTargetDownTransition, &mEnableDropDownDropTargets);

    // Create the various fade animations
    if (mEnableDropDownDropTargets) {
        mDropTargetBar->SetTranslationY(-mBarHeight);
        ObjectAnimator::OfFloat(mDropTargetBar, String("translationY"),
                -mBarHeight, 0f, (IObjectAnimator**)&mDropTargetBarAnim);
        ObjectAnimator::OfFloat(mQSBSearchBar, String("translationY"), 0f,
                -mBarHeight, (IObjectAnimator**)&mQSBSearchBarAnim);
    }
    else {
        mDropTargetBar->SetAlpha(0f);
        ObjectAnimator::OfFloat(mDropTargetBar, String("alpha"), 0f,
                1f, (IObjectAnimator**)&mDropTargetBarAnim);
        ObjectAnimator::OfFloat(mQSBSearchBar, String("alpha"), 1f,
                0f, (IObjectAnimator**)&mQSBSearchBarAnim);
    }
    SetupAnimation(mDropTargetBarAnim, mDropTargetBar);
    return SetupAnimation(mQSBSearchBarAnim, mQSBSearchBar);
}

ECode SearchDropTargetBar::FinishAnimations()
{
    PrepareStartAnimation(mDropTargetBar);
    mDropTargetBarAnim->Reverse();
    PrepareStartAnimation(mQSBSearchBar);
    return mQSBSearchBarAnim->Reverse();
}

ECode SearchDropTargetBar::ShowSearchBar(
    /* [in] */ Boolean animated)
{
    if (!mIsSearchBarHidden) {
        return NOERROR;
    }
    if (animated) {
        PrepareStartAnimation(mQSBSearchBar);
        mQSBSearchBarAnim->Reverse();
    }
    else {
        mQSBSearchBarAnim->Cancel();
        if (mEnableDropDownDropTargets) {
            mQSBSearchBar->SetTranslationY(0);
        } else {
            mQSBSearchBar->SetAlpha(1f);
        }
    }
    mIsSearchBarHidden = FALSE;
    return NOERROR;
}

ECode SearchDropTargetBar::HideSearchBar(
    /* [in] */ Boolean animated)
{
    if (mIsSearchBarHidden) {
        return NOERROR;
    }
    if (animated) {
        PrepareStartAnimation(mQSBSearchBar);
        mQSBSearchBarAnim->Start();
    }
    else {
        mQSBSearchBarAnim->Cancel();
        if (mEnableDropDownDropTargets) {
            mQSBSearchBar->SetTranslationY(-mBarHeight);
        }
        else {
            mQSBSearchBar->SetAlpha(0f);
        }
    }
    mIsSearchBarHidden = TRUE;
    return NOERROR;
}

ECode SearchDropTargetBar::GetTransitionInDuration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = sTransitionInDuration;
    return NOERROR;
}

ECode SearchDropTargetBar::GetTransitionOutDuration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = sTransitionOutDuration;
    return NOERROR;
}

ECode SearchDropTargetBar::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    // Animate out the QSB search bar, and animate in the drop target bar
    PrepareStartAnimation(mDropTargetBar);
    mDropTargetBarAnim->Start();
    if (!mIsSearchBarHidden) {
        PrepareStartAnimation(mQSBSearchBar);
        mQSBSearchBarAnim->Start();
    }
    return NOERROR;
}

ECode SearchDropTargetBar::DeferOnDragEnd()
{
    mDeferOnDragEnd = TRUE;
}

ECode SearchDropTargetBar::OnDragEnd()
{
    if (!mDeferOnDragEnd) {
        // Restore the QSB search bar, and animate out the drop target bar
        PrepareStartAnimation(mDropTargetBar);
        mDropTargetBarAnim->Reverse();
        if (!mIsSearchBarHidden) {
            PrepareStartAnimation(mQSBSearchBar);
            mQSBSearchBarAnim->Reverse();
        }
    }
    else {
        mDeferOnDragEnd = FALSE;
    }
    return NOERROR;
}

ECode SearchDropTargetBar::OnSearchPackagesChanged(
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

ECode SearchDropTargetBar::GetSearchBarBounds(
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (mQSBSearchBar != NULL) {
        AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Allco(2);
        mQSBSearchBar->GetLocationOnScreen(pos);

        AutoPtr<IRect> rect;
        CRect::New((IRect**)&rect);
        rect->SetLeft = (*pos)[0];
        rect->SetTop = (*pos)[1];
        Int32 width;
        mQSBSearchBar->GetWidth(&width);
        rect->SetRight = (*pos)[0] + width;
        Int32 height;
        mQSBSearchBar->GetHeight(&height);
        rect->SetBottom = (*pos)[1] + height;
        *result = rect;
        return NOERROR;
    } else {
        *result = NULL;
        return NOERROR;
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos