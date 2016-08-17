
#include "elastos/droid/systemui/keyguard/CMultiPaneChallengeLayout.h"
#include <elastos/core/Math.h>
#include "R.h"

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams,
        ViewGroup::MarginLayoutParams, IMultiPaneChallengeLayoutLayoutParams)

CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams::MultiPaneChallengeLayoutLayoutParams()
    : mCenterWithinArea(0.0f)
    , mChildType(0)
    , mGravity(IGravity::NO_GRAVITY)
    , mMaxWidth(-1)
    , mMaxHeight(-1)
{
}

ECode CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams::constructor()
{
    return constructor(WRAP_CONTENT, WRAP_CONTENT);
}

ECode CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IMultiPaneChallengeLayout* parent)
{
    ViewGroup::MarginLayoutParams::constructor(c, attrs);

    AutoPtr<ArrayOf<Int32> > attrIds =
            TO_ATTRS_ARRAYOF(R::styleable::MultiPaneChallengeLayout_Layout);
    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    a->GetFloat(
            R::styleable::MultiPaneChallengeLayout_Layout_layout_centerWithinArea, 0, &mCenterWithinArea);
    a->GetInt32(R::styleable::MultiPaneChallengeLayout_Layout_layout_childType,
            CHILD_TYPE_NONE, &mChildType);
    a->GetInt32(R::styleable::MultiPaneChallengeLayout_Layout_layout_gravity,
            IGravity::NO_GRAVITY, &mGravity);
    a->GetDimensionPixelSize(
            R::styleable::MultiPaneChallengeLayout_Layout_layout_maxWidth, -1, &mMaxWidth);
    a->GetDimensionPixelSize(
            R::styleable::MultiPaneChallengeLayout_Layout_layout_maxHeight, -1, &mMaxHeight);

    // Default gravity settings based on type and parent orientation
    if (mGravity == IGravity::NO_GRAVITY) {
        if (((CMultiPaneChallengeLayout*)parent)->mOrientation == HORIZONTAL) {
            switch (mChildType) {
                case CHILD_TYPE_WIDGET:
                    mGravity = IGravity::LEFT | IGravity::CENTER_VERTICAL;
                    break;
                case CHILD_TYPE_CHALLENGE:
                    mGravity = IGravity::RIGHT | IGravity::CENTER_VERTICAL;
                    break;
                case CHILD_TYPE_USER_SWITCHER:
                    mGravity = IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL;
                    break;
            }
        } else {
            switch (mChildType) {
                case CHILD_TYPE_WIDGET:
                    mGravity = IGravity::TOP | IGravity::CENTER_HORIZONTAL;
                    break;
                case CHILD_TYPE_CHALLENGE:
                    mGravity = IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL;
                    break;
                case CHILD_TYPE_USER_SWITCHER:
                    mGravity = IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL;
                    break;
            }
        }
    }

    return a->Recycle();
}

ECode CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return ViewGroup::MarginLayoutParams::constructor(width, height);
}

ECode CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams::constructor(
    /* [in] */ IMultiPaneChallengeLayoutLayoutParams* source)
{
    constructor(IViewGroupMarginLayoutParams::Probe(source));

    MultiPaneChallengeLayoutLayoutParams* _source =
            (MultiPaneChallengeLayoutLayoutParams*)source;
    mCenterWithinArea = _source->mCenterWithinArea;
    mChildType = _source->mChildType;
    mGravity = _source->mGravity;
    mMaxWidth = _source->mMaxWidth;
    mMaxHeight = _source->mMaxHeight;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CMultiPaneChallengeLayout::MyOnClickListener, Object, IViewOnClickListener)

ECode CMultiPaneChallengeLayout::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->HideBouncer();
}

ECode CMultiPaneChallengeLayout::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return mHost->mScrimView->SetVisibility(VISIBLE);
}

ECode CMultiPaneChallengeLayout::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mHost->mScrimView->SetVisibility(INVISIBLE);
}

const String CMultiPaneChallengeLayout::TAG("MultiPaneChallengeLayout");

CAR_OBJECT_IMPL(CMultiPaneChallengeLayout)

CAR_INTERFACE_IMPL_2(CMultiPaneChallengeLayout, ViewGroup, IChallengeLayout,
        IMultiPaneChallengeLayout)

CMultiPaneChallengeLayout::CMultiPaneChallengeLayout()
    : mOrientation(0)
    , mIsBouncing(FALSE)
{
    CRect::New((IRect**)&mTempRect);
    CRect::New((IRect**)&mZeroPadding);
    CRect::New((IRect**)&mInsets);

    mScrimClickListener = new MyOnClickListener(this);
}

ECode CMultiPaneChallengeLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CMultiPaneChallengeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CMultiPaneChallengeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    ViewGroup::constructor(context, attrs, defStyleAttr);

    AutoPtr<ArrayOf<Int32> > attrIds =
            TO_ATTRS_ARRAYOF(R::styleable::MultiPaneChallengeLayout);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, 0,
            (ITypedArray**)&a);
    a->GetInt32(R::styleable::MultiPaneChallengeLayout_android_orientation,
            HORIZONTAL, &mOrientation);
    a->Recycle();

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDisplayMetrics((IDisplayMetrics**)&mDisplayMetrics);

    SetSystemUiVisibility(SYSTEM_UI_FLAG_LAYOUT_STABLE | SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::SetInsets(
    /* [in] */ IRect* insets)
{
    return mInsets->Set(insets);
}

ECode CMultiPaneChallengeLayout::IsChallengeShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::IsChallengeOverlapping(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::ShowChallenge(
    /* [in] */ Boolean)
{
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::GetBouncerAnimationDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration)

    *duration = ANIMATE_BOUNCE_DURATION;
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::ShowBouncer()
{
    if (mIsBouncing) return NOERROR;
    mIsBouncing = TRUE;
    if (mScrimView != NULL) {
        if (mChallengeView != NULL) {
            mChallengeView->ShowBouncer(ANIMATE_BOUNCE_DURATION);
        }

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 1.0f;
        AutoPtr<IObjectAnimator> oa;
        helper->OfFloat(mScrimView, String("alpha"), array, (IObjectAnimator**)&oa);
        AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
        anim->SetDuration(ANIMATE_BOUNCE_DURATION);

        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this);
        anim->AddListener(lis);
        anim->Start();
    }
    if (mBouncerListener != NULL) {
        mBouncerListener->OnBouncerStateChanged(TRUE);
    }
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::HideBouncer()
{
    if (!mIsBouncing) return NOERROR;
    mIsBouncing = FALSE;
    if (mScrimView != NULL) {
        if (mChallengeView != NULL) {
            mChallengeView->HideBouncer(ANIMATE_BOUNCE_DURATION);
        }

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.0f;
        AutoPtr<IObjectAnimator> oa;
        helper->OfFloat(mScrimView, String("alpha"), array, (IObjectAnimator**)&oa);
        AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
        anim->SetDuration(ANIMATE_BOUNCE_DURATION);

        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter2(this);
        anim->AddListener(lis);
        anim->Start();
    }
    if (mBouncerListener != NULL) {
        mBouncerListener->OnBouncerStateChanged(FALSE);
    }
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::IsBouncing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsBouncing;
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::SetOnBouncerStateChangedListener(
    /* [in] */ IChallengeLayoutOnBouncerStateChangedListener* listener)
{
    mBouncerListener = listener;
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    if (mIsBouncing && TO_IINTERFACE(child) != TO_IINTERFACE(mChallengeView)) {
        // Clear out of the bouncer if the user tries to move focus outside of
        // the security challenge view.
        HideBouncer();
    }
    return ViewGroup::RequestChildFocus(child, focused);
}

ECode CMultiPaneChallengeLayout::SetScrimView(
    /* [in] */ IView* scrim)
{
    if (mScrimView != NULL) {
        mScrimView->SetOnClickListener(NULL);
    }
    mScrimView = scrim;
    if (mScrimView != NULL) {
        mScrimView->SetAlpha(mIsBouncing ? 1.0f : 0.0f);
        mScrimView->SetVisibility(mIsBouncing ? VISIBLE : INVISIBLE);
        mScrimView->SetFocusable(TRUE);
        mScrimView->SetOnClickListener(mScrimClickListener);
    }
    return NOERROR;
}

Int32 CMultiPaneChallengeLayout::GetVirtualHeight(
    /* [in] */ IMultiPaneChallengeLayoutLayoutParams* lp,
    /* [in] */ Int32 height,
    /* [in] */ Int32 heightUsed)
{
    Int32 virtualHeight = height;
    AutoPtr<IView> root;
    GetRootView((IView**)&root);
    if (root != NULL) {
        // This calculation is super dodgy and relies on several assumptions.
        // Specifically that the root of the window will be padded in for insets
        // and that the window is LAYOUT_IN_SCREEN.
        Int32 h;
        mDisplayMetrics->GetHeightPixels(&h);
        Int32 ptop;
        root->GetPaddingTop(&ptop);
        Int32 t;
        mInsets->GetTop(&t);
        virtualHeight = h - ptop - t;
    }
    MultiPaneChallengeLayoutLayoutParams* _lp = (MultiPaneChallengeLayoutLayoutParams*)lp;
    if (_lp->mChildType == IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_USER_SWITCHER) {
        // Always measure the user switcher as if there were no IME insets
        // on the window.
        return virtualHeight - heightUsed;
    }
    else if (_lp->mChildType == IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_PAGE_DELETE_DROP_TARGET) {
        return height;
    }
    return Elastos::Core::Math::Min(virtualHeight - heightUsed, height);
}

ECode CMultiPaneChallengeLayout::OnMeasure(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    if (MeasureSpec::GetMode(widthSpec) != MeasureSpec::EXACTLY ||
            MeasureSpec::GetMode(heightSpec) != MeasureSpec::EXACTLY) {
        // throw new IllegalArgumentException(
        //         "MultiPaneChallengeLayout must be measured with an exact size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 width = MeasureSpec::GetSize(widthSpec);
    Int32 height = MeasureSpec::GetSize(heightSpec);
    SetMeasuredDimension(width, height);

    Int32 top;
    mInsets->GetTop(&top);
    Int32 bottom;
    mInsets->GetBottom(&bottom);
    Int32 insetHeight = height - top - bottom;

    Int32 insetHeightSpec = MeasureSpec::MakeMeasureSpec(insetHeight,
            MeasureSpec::EXACTLY);

    Int32 widthUsed = 0;
    Int32 heightUsed = 0;

    // First pass. Find the challenge view and measure the user switcher,
    // which consumes space in the layout.
    mChallengeView = NULL;
    mUserSwitcherView = NULL;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<MultiPaneChallengeLayoutLayoutParams> lp =
                (MultiPaneChallengeLayoutLayoutParams*)IMultiPaneChallengeLayoutLayoutParams::Probe(params);
        if (lp->mChildType == IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_CHALLENGE) {
            if (mChallengeView != NULL) {
                // throw new IllegalStateException(
                //         "There may only be one child of type challenge");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (IKeyguardSecurityContainer::Probe(child) == NULL) {
                // throw new IllegalArgumentException(
                //         "Challenge must be a KeyguardSecurityContainer");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mChallengeView = IKeyguardSecurityContainer::Probe(child);
        }
        else if (lp->mChildType == IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_USER_SWITCHER) {
            if (mUserSwitcherView != NULL) {
                // throw new IllegalStateException(
                //         "There may only be one child of type userSwitcher");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mUserSwitcherView = child;

            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility == GONE) continue;

            Int32 adjustedWidthSpec = widthSpec;
            Int32 adjustedHeightSpec = insetHeightSpec;
            if (lp->mMaxWidth >= 0) {
                adjustedWidthSpec = MeasureSpec::MakeMeasureSpec(
                        Elastos::Core::Math::Min(lp->mMaxWidth, width),
                        MeasureSpec::EXACTLY);
            }
            if (lp->mMaxHeight >= 0) {
                adjustedHeightSpec = MeasureSpec::MakeMeasureSpec(
                        Elastos::Core::Math::Min(lp->mMaxHeight, insetHeight),
                        MeasureSpec::EXACTLY);
            }
            // measureChildWithMargins will resolve layout direction for the LayoutParams
            MeasureChildWithMargins(child, adjustedWidthSpec, 0, adjustedHeightSpec, 0);

            // Only subtract out space from one dimension. Favor vertical.
            // Offset by 1.5x to add some balance along the other edge.
            AutoPtr<IGravity> helper;
            CGravity::AcquireSingleton((IGravity**)&helper);
            Boolean res;
            if (helper->IsVertical(lp->mGravity, &res), res) {
                Int32 height;
                child->GetMeasuredHeight(&height);
                heightUsed += height * 1.5f;
            }
            else if (helper->IsHorizontal(lp->mGravity, &res), res) {
                Int32 width;
                child->GetMeasuredWidth(&width);
                widthUsed += width * 1.5f;
            }
        }
        else if (lp->mChildType == IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_SCRIM) {
            SetScrimView(child);
            child->Measure(widthSpec, heightSpec);
        }
    }

    // Second pass. Measure everything that's left.
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<MultiPaneChallengeLayoutLayoutParams> lp =
                (MultiPaneChallengeLayoutLayoutParams*)IMultiPaneChallengeLayoutLayoutParams::Probe(params);
        Int32 visibility;
        if (lp->mChildType == IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_USER_SWITCHER ||
                lp->mChildType == IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_SCRIM ||
                (child->GetVisibility(&visibility), visibility) == GONE) {
            // Don't need to measure GONE children, and the user switcher was already measured.
            continue;
        }

        Int32 virtualHeight = GetVirtualHeight(lp, insetHeight, heightUsed);

        Int32 adjustedWidthSpec;
        Int32 adjustedHeightSpec;
        if (lp->mCenterWithinArea > 0) {
            if (mOrientation == HORIZONTAL) {
                adjustedWidthSpec = MeasureSpec::MakeMeasureSpec(
                        (Int32) ((width - widthUsed) * lp->mCenterWithinArea + 0.5f),
                        MeasureSpec::EXACTLY);
                adjustedHeightSpec = MeasureSpec::MakeMeasureSpec(
                        virtualHeight,
                        MeasureSpec::EXACTLY);
            }
            else {
                adjustedWidthSpec = MeasureSpec::MakeMeasureSpec(
                        width - widthUsed, MeasureSpec::EXACTLY);
                adjustedHeightSpec = MeasureSpec::MakeMeasureSpec(
                        (Int32) (virtualHeight * lp->mCenterWithinArea + 0.5f),
                        MeasureSpec::EXACTLY);
            }
        }
        else {
            adjustedWidthSpec = MeasureSpec::MakeMeasureSpec(
                    width - widthUsed, MeasureSpec::EXACTLY);
            adjustedHeightSpec = MeasureSpec::MakeMeasureSpec(
                    virtualHeight, MeasureSpec::EXACTLY);
        }
        if (lp->mMaxWidth >= 0) {
            adjustedWidthSpec = MeasureSpec::MakeMeasureSpec(
                    Elastos::Core::Math::Min(lp->mMaxWidth, MeasureSpec::GetSize(adjustedWidthSpec)),
                    MeasureSpec::EXACTLY);
        }
        if (lp->mMaxHeight >= 0) {
            adjustedHeightSpec = MeasureSpec::MakeMeasureSpec(
                    Elastos::Core::Math::Min(lp->mMaxHeight, MeasureSpec::GetSize(adjustedHeightSpec)),
                    MeasureSpec::EXACTLY);
        }

        MeasureChildWithMargins(child, adjustedWidthSpec, 0, adjustedHeightSpec, 0);
    }
    return NOERROR;
}

ECode CMultiPaneChallengeLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    AutoPtr<IRect> padding = mTempRect;

    Int32 left;
    GetPaddingLeft(&left);
    padding->SetLeft(left);
    Int32 top;
    GetPaddingTop(&top);
    padding->SetTop(top);
    Int32 right;
    GetPaddingRight(&right);
    padding->SetRight(right);
    Int32 bottom;
    GetPaddingBottom(&bottom);
    padding->SetBottom(bottom);

    Int32 width = r - l;
    Int32 height = b - t;
    Int32 itop;
    mInsets->GetTop(&itop);
    Int32 ibottom;
    mInsets->GetBottom(&ibottom);
    Int32 insetHeight = height - itop - ibottom;

    // Reserve extra space in layout for the user switcher by modifying
    // local padding during this layout pass
    Int32 visibility;
    if (mUserSwitcherView != NULL &&
            (mUserSwitcherView->GetVisibility(&visibility), visibility) != GONE) {
        LayoutWithGravity(width, insetHeight, mUserSwitcherView, padding, TRUE);
    }

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<MultiPaneChallengeLayoutLayoutParams> lp =
                (MultiPaneChallengeLayoutLayoutParams*)IMultiPaneChallengeLayoutLayoutParams::Probe(params);

        // We did the user switcher above if we have one.
        Int32 visibility;
        if (child == mUserSwitcherView ||
                (child->GetVisibility(&visibility), visibility) == GONE) continue;

        if (TO_IINTERFACE(child) == TO_IINTERFACE(mScrimView)) {
            child->Layout(0, 0, width, height);
            continue;
        }
        else if (lp->mChildType ==
                IMultiPaneChallengeLayoutLayoutParams::CHILD_TYPE_PAGE_DELETE_DROP_TARGET) {
            LayoutWithGravity(width, insetHeight, child, mZeroPadding, FALSE);
            continue;
        }

        LayoutWithGravity(width, insetHeight, child, padding, FALSE);
    }
    return NOERROR;
}

void CMultiPaneChallengeLayout::LayoutWithGravity(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IView* child,
    /* [in] */ IRect* padding,
    /* [in] */ Boolean adjustPadding)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<MultiPaneChallengeLayoutLayoutParams> lp =
            (MultiPaneChallengeLayoutLayoutParams*)IMultiPaneChallengeLayoutLayoutParams::Probe(params);

    Int32 t;
    padding->GetTop(&t);
    Int32 b;
    padding->GetBottom(&b);
    Int32 pt;
    GetPaddingTop(&pt);
    Int32 pb;
    GetPaddingBottom(&pb);
    Int32 heightUsed = t + b - pt - pb;
    height = GetVirtualHeight(lp, height, heightUsed);

    Int32 direction;
    GetLayoutDirection(&direction);
    AutoPtr<IGravity> helper;
    CGravity::AcquireSingleton((IGravity**)&helper);
    Int32 gravity;
    helper->GetAbsoluteGravity(lp->mGravity, direction, &gravity);

    Boolean fixedLayoutSize = lp->mCenterWithinArea > 0;
    Boolean fixedLayoutHorizontal = fixedLayoutSize && mOrientation == HORIZONTAL;
    Boolean fixedLayoutVertical = fixedLayoutSize && mOrientation == VERTICAL;

    Int32 adjustedWidth;
    Int32 adjustedHeight;
    if (fixedLayoutHorizontal) {
        Int32 left;
        padding->GetLeft(&left);
        Int32 right;
        padding->GetRight(&right);
        Int32 paddedWidth = width - left - right;
        adjustedWidth = (Int32) (paddedWidth * lp->mCenterWithinArea + 0.5f);
        adjustedHeight = height;
    }
    else if (fixedLayoutVertical) {
        Int32 pt;
        GetPaddingTop(&pt);
        Int32 pb;
        GetPaddingBottom(&pb);
        Int32 paddedHeight = height - pt - pb;
        adjustedWidth = width;
        adjustedHeight = (Int32) (paddedHeight * lp->mCenterWithinArea + 0.5f);
    }
    else {
        adjustedWidth = width;
        adjustedHeight = height;
    }

    Boolean isVertical;
    helper->IsVertical(gravity, &isVertical);
    Boolean isHorizontal;
    helper->IsHorizontal(gravity, &isHorizontal);
    Int32 childWidth;
    child->GetMeasuredWidth(&childWidth);
    Int32 childHeight;
    child->GetMeasuredHeight(&childHeight);

    Int32 left;
    padding->GetLeft(&left);
    Int32 top;
    padding->GetTop(&top);
    Int32 right = left + childWidth;
    Int32 bottom = top + childHeight;
    switch (gravity & IGravity::VERTICAL_GRAVITY_MASK) {
        case IGravity::TOP:
        {
            Int32 t;
            padding->GetTop(&t);
            top = fixedLayoutVertical ?
                    t + (adjustedHeight - childHeight) / 2 : t;
            bottom = top + childHeight;
            if (adjustPadding && isVertical) {
                padding->SetTop(bottom);
                Int32 b;
                padding->GetBottom(&b);
                padding->SetBottom(b + childHeight / 2);
            }
            break;
        }
        case IGravity::BOTTOM:
        {
            Int32 t;
            padding->GetTop(&t);
            bottom = fixedLayoutVertical
                    ? t + height - (adjustedHeight - childHeight) / 2
                    : t + height;
            top = bottom - childHeight;
            if (adjustPadding && isVertical) {
                padding->SetBottom(height - top);
                padding->SetTop(t + childHeight / 2);
            }
            break;
        }
        case IGravity::CENTER_VERTICAL:
        {
            Int32 t;
            padding->GetTop(&t);
            top = t + (height - childHeight) / 2;
            bottom = top + childHeight;
            break;
        }
    }
    switch (gravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
        case IGravity::LEFT:
        {
            Int32 l;
            padding->GetLeft(&l);
            left = fixedLayoutHorizontal ?
                    l + (adjustedWidth - childWidth) / 2 : l;
            right = left + childWidth;
            if (adjustPadding && isHorizontal && !isVertical) {
                padding->SetLeft(right);
                Int32 r;
                padding->GetRight(&r);
                padding->SetRight(r + childWidth / 2);
            }
            break;
        }
        case IGravity::RIGHT:
        {
            Int32 r;
            padding->GetRight(&r);
            right = fixedLayoutHorizontal
                    ? width - r - (adjustedWidth - childWidth) / 2
                    : width - r;
            left = right - childWidth;
            if (adjustPadding && isHorizontal && !isVertical) {
                padding->SetRight(width - left);
                Int32 l;
                padding->GetLeft(&l);
                padding->SetLeft(l + childWidth / 2);
            }
            break;
        }
        case IGravity::CENTER_HORIZONTAL:
        {
            Int32 l;
            padding->GetLeft(&l);
            Int32 r;
            padding->GetRight(&r);
            Int32 paddedWidth = width - l - r;
            left = (paddedWidth - childWidth) / 2;
            right = left + childWidth;
            break;
        }
    }

    Int32 _t;
    mInsets->GetTop(&_t);
    top += _t;
    bottom += _t;
    child->Layout(left, top, right, bottom);
}

ECode CMultiPaneChallengeLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** p)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<MultiPaneChallengeLayoutLayoutParams> _p =
            new MultiPaneChallengeLayoutLayoutParams();
    _p->constructor(context, attrs, this);
    *p = IViewGroupLayoutParams::Probe(_p);
    REFCOUNT_ADD(*p)
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> CMultiPaneChallengeLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    if(IMultiPaneChallengeLayoutLayoutParams::Probe(p) != NULL) {
        AutoPtr<MultiPaneChallengeLayoutLayoutParams> _p =
            new MultiPaneChallengeLayoutLayoutParams();
        _p->constructor(IMultiPaneChallengeLayoutLayoutParams::Probe(p));
        return IViewGroupLayoutParams::Probe(_p);
    }
    else {
        if(IViewGroupMarginLayoutParams::Probe(p) != NULL) {
            AutoPtr<MultiPaneChallengeLayoutLayoutParams> _p =
                new MultiPaneChallengeLayoutLayoutParams();
            _p->constructor(IViewGroupMarginLayoutParams::Probe(p));
            return IViewGroupLayoutParams::Probe(_p);
        }
        else {
            AutoPtr<MultiPaneChallengeLayoutLayoutParams> _p =
                new MultiPaneChallengeLayoutLayoutParams();
            _p->constructor(p);
            return IViewGroupLayoutParams::Probe(_p);
        }
    }
    return NULL;
}

ECode CMultiPaneChallengeLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** outp)
{
    VALIDATE_NOT_NULL(outp)

    AutoPtr<MultiPaneChallengeLayoutLayoutParams> _p =
            new MultiPaneChallengeLayoutLayoutParams();
    _p->constructor();
    *outp = IViewGroupLayoutParams::Probe(_p);
    REFCOUNT_ADD(*outp)
    return NOERROR;
}

Boolean CMultiPaneChallengeLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return IMultiPaneChallengeLayoutLayoutParams::Probe(p) != NULL;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
