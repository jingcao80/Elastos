
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/ViewAnimator.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(ViewAnimator, FrameLayout, IViewAnimator);
ViewAnimator::ViewAnimator()
    : mWhichChild(0)
    , mFirstTime(TRUE)
    , mAnimateFirstTime(TRUE)
{}

ECode ViewAnimator::constructor(
    /* [in] */ IContext* context)
{
    FrameLayout::constructor(context);
    InitViewAnimator(context, NULL);
    return NOERROR;
}

ECode ViewAnimator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ViewAnimator),
            ArraySize(R::styleable::ViewAnimator));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(
            attrs, attrIds, (ITypedArray**)&a));
    Int32 resource;
    a->GetResourceId(R::styleable::ViewAnimator_inAnimation, 0, &resource);

    if (resource > 0) {
        SetInAnimation(context, resource);
    }

    a->GetResourceId(R::styleable::ViewAnimator_outAnimation, 0, &resource);
    if (resource > 0) {
        SetOutAnimation(context, resource);
    }

    Boolean flag;
    a->GetBoolean(R::styleable::ViewAnimator_animateFirstView, TRUE, &flag);
    SetAnimateFirstView(flag);

    a->Recycle();
    InitViewAnimator(context, NULL);
    return NOERROR;
}

void ViewAnimator::InitViewAnimator(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    if (attrs == NULL) {
        // For compatibility, always measure children when undefined.
        SetMeasureAllChildren(TRUE);
        return;
    }

    // For compatibility, default to measure children, but allow XML
    // attribute to override.
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::FrameLayout),
            ArraySize(R::styleable::FrameLayout));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
            attrs, attrIds, (ITypedArray**)&a));

    Boolean measureAllChildren;
    a->GetBoolean(R::styleable::FrameLayout_measureAllChildren, TRUE, &measureAllChildren);
    SetMeasureAllChildren(measureAllChildren);
    a->Recycle();
}

ECode ViewAnimator::SetDisplayedChild(
    /* [in] */ Int32 whichChild)
{
    mWhichChild = whichChild;
    Int32 count = 0;
    GetChildCount(&count);
    if (whichChild >= count) {
        mWhichChild = 0;
    } else if (whichChild < 0) {
        mWhichChild = count - 1;
    }
    AutoPtr<IView> view;
    GetFocusedChild((IView**)&view);
    Boolean hasFocus = view.Get() != NULL;
    // This will clear old focus if we had it
    ShowOnly(mWhichChild);
    if (hasFocus) {
        Boolean tmp = FALSE;
        // Try to retake focus if we had it
        RequestFocus(IView::FOCUS_FORWARD, &tmp);
    }
    return NOERROR;
}

ECode ViewAnimator::GetDisplayedChild(
    /* [in] */ Int32* child)
{
    *child = mWhichChild;
    return NOERROR;
}

ECode ViewAnimator::ShowNext()
{
    return SetDisplayedChild(mWhichChild + 1);
}

ECode ViewAnimator::ShowPrevious()
{
    return SetDisplayedChild(mWhichChild - 1);
}

ECode ViewAnimator::ShowOnly(
    /* [in] */ Int32 childIndex,
    /* [in] */ Boolean animate)
{
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (i == childIndex) {
            if (animate && mInAnimation != NULL) {
                child->StartAnimation(mInAnimation);
            }
            child->SetVisibility(IView::VISIBLE);
            mFirstTime = FALSE;
        } else {
            Int32 visible;
            child->GetVisibility(&visible);
            AutoPtr<IAnimation> animTemp;
            child->GetAnimation((IAnimation**)&animTemp);
            if (animate && mOutAnimation != NULL && visible == IView::VISIBLE) {
                child->StartAnimation(mOutAnimation);
            } else if (animTemp == mInAnimation) {
                child->ClearAnimation();
            }
            child->SetVisibility(IView::GONE);
        }
    }
    return NOERROR;
}

ECode ViewAnimator::ShowOnly(
    /* [in] */ Int32 childIndex) {
    Boolean animate = (!mFirstTime || mAnimateFirstTime);
    return ShowOnly(childIndex, animate);
}

ECode ViewAnimator::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FrameLayout::AddView(child, index, params);
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 1) {
        child->SetVisibility(IView::VISIBLE);
    } else {
        child->SetVisibility(IView::GONE);
    }
    if (index >= 0 && mWhichChild >= index) {
        // Added item above current one, increment the index of the displayed child
        SetDisplayedChild(mWhichChild + 1);
    }
    return NOERROR;
}

ECode ViewAnimator::RemoveAllViews()
{
    FrameLayout::RemoveAllViews();
    mWhichChild = 0;
    mFirstTime = TRUE;
    return NOERROR;
}

ECode ViewAnimator::RemoveView(
    /* [IN] */ IView* view)
{
    Int32 index = 0;
    IndexOfChild(view, &index);
    if (index >= 0) {
        RemoveViewAt(index);
    }
    return NOERROR;
}

ECode ViewAnimator::RemoveViewAt(
    /* [in] */ Int32 index)
{
    FrameLayout::RemoveViewAt(index);
    Int32 childCount = 0;
    GetChildCount(&childCount);
    if (childCount == 0) {
        mWhichChild = 0;
        mFirstTime = TRUE;
    } else if (mWhichChild >= childCount) {
        // Displayed is above child count, so float down to top of stack
        SetDisplayedChild(childCount - 1);
    } else if (mWhichChild == index) {
        // Displayed was removed, so show the new child living in its place
        SetDisplayedChild(mWhichChild);
    }
    return NOERROR;
}

ECode ViewAnimator::RemoveViewInLayout(
    /* [in] */ IView* view)
{
    return RemoveView(view);
}

ECode ViewAnimator::RemoveViews(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    FrameLayout::RemoveViews(start, count);
    Int32 n = 0;
    if ((GetChildCount(&n), n) == 0) {
        mWhichChild = 0;
        mFirstTime = TRUE;
    } else if (mWhichChild >= start && mWhichChild < start + count) {
        // Try showing new displayed child, wrapping if needed
        SetDisplayedChild(mWhichChild);
    }
    return NOERROR;
}

ECode ViewAnimator::RemoveViewsInLayout(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    return RemoveViews(start, count);
}

ECode ViewAnimator::GetCurrentView(
    /* [out] */ IView** curView)
{
    VALIDATE_NOT_NULL(curView);
    return GetChildAt(mWhichChild, curView);
}

ECode ViewAnimator::GetInAnimation(
    /* [out] */ IAnimation** anim)
{
    VALIDATE_NOT_NULL(anim);
    *anim = mInAnimation;
    REFCOUNT_ADD(*anim);
    return NOERROR;
}

ECode ViewAnimator::SetInAnimation(
    /* [in] */ IAnimation* inAnimation)
{
    mInAnimation = inAnimation;
    return NOERROR;
}

ECode ViewAnimator::GetOutAnimation(
    /* [out] */ IAnimation** anim)
{
    VALIDATE_NOT_NULL(anim);
    *anim = mOutAnimation;
    REFCOUNT_ADD(*anim);
    return NOERROR;
}

ECode ViewAnimator::SetOutAnimation(
    /* [in] */ IAnimation* outAnimation)
{
    mOutAnimation = outAnimation;
    return NOERROR;
}

ECode ViewAnimator::SetInAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
    AutoPtr<IAnimation> anim;
    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    utils->LoadAnimation(context, resourceID, (IAnimation**)&anim);
    return SetInAnimation(anim);
}

ECode ViewAnimator::SetOutAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
    AutoPtr<IAnimation> anim;
    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    utils->LoadAnimation(context, resourceID, (IAnimation**)&anim);
    return SetOutAnimation(anim);
}

ECode ViewAnimator::GetAnimateFirstView(
    /* [out] */ Boolean* animate)
{
    VALIDATE_NOT_NULL(animate);
    *animate = mAnimateFirstTime;
    return NOERROR;
}

ECode ViewAnimator::SetAnimateFirstView(
    /* [in] */ Boolean animate)
{
    mAnimateFirstTime = animate;
    return NOERROR;
}

ECode ViewAnimator::GetBaseline(
    /* [out] */ Int32* baseline)
{
    VALIDATE_NOT_NULL(baseline);
    AutoPtr<IView> view;
    GetCurrentView((IView**)&view);
    Int32 viewBaseline;
    view->GetBaseline(&viewBaseline);
    Int32 superBaseline;
    FrameLayout::GetBaseline(&superBaseline);
    *baseline = (view != NULL) ? viewBaseline : superBaseline;
    return NOERROR;
}

ECode ViewAnimator::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    String className("CViewAnimator");
    AutoPtr<ICharSequence> cs;
    CString::New(className, (ICharSequence**)&cs);
    IAccessibilityRecord::Probe(event)->SetClassName(cs);
    return NOERROR;
}

ECode ViewAnimator::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    String className("CViewAnimator");
    AutoPtr<ICharSequence> cs;
    CString::New(className, (ICharSequence**)&cs);
    info->SetClassName(cs);
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
