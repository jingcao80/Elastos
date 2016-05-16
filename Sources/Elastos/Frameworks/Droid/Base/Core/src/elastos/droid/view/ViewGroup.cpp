
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/CLayoutTransition.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/graphics/CPointF.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/view/FocusFinder.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/CViewGroupOverlay.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/animation/CTransformation.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/LayoutAnimationController.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::EIID_ITransitionListener;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Graphics::Insets;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::ViewRootImpl;
using Elastos::Droid::View::FocusFinder;
using Elastos::Droid::View::CMotionEvent;
using Elastos::Droid::View::CViewGroupOverlay;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::Animation::IAnimationParameters;
using Elastos::Droid::View::Animation::CTransformation;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::LayoutAnimationController;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {

static const String TAG("ViewGroup");

const Boolean ViewGroup::DBG = FALSE;
const Int32 ViewGroup::FOCUS_BEFORE_DESCENDANTS;
const Int32 ViewGroup::FOCUS_AFTER_DESCENDANTS;
const Int32 ViewGroup::FOCUS_BLOCK_DESCENDANTS;
const Int32 ViewGroup::PERSISTENT_NO_CACHE;
const Int32 ViewGroup::PERSISTENT_ANIMATION_CACHE;
const Int32 ViewGroup::PERSISTENT_SCROLLING_CACHE;
const Int32 ViewGroup::PERSISTENT_ALL_CACHES;
const Int32 ViewGroup::FLAG_CLIP_CHILDREN;
const Int32 ViewGroup::FLAG_CLIP_TO_PADDING;
const Int32 ViewGroup::FLAG_INVALIDATE_REQUIRED;
const Int32 ViewGroup::FLAG_RUN_ANIMATION;
const Int32 ViewGroup::FLAG_ANIMATION_DONE;
const Int32 ViewGroup::FLAG_PADDING_NOT_NULL;
const Int32 ViewGroup::FLAG_ANIMATION_CACHE;
const Int32 ViewGroup::FLAG_OPTIMIZE_INVALIDATE;
const Int32 ViewGroup::FLAG_CLEAR_TRANSFORMATION;
const Int32 ViewGroup::FLAG_NOTIFY_ANIMATION_LISTENER;
const Int32 ViewGroup::FLAG_ADD_STATES_FROM_CHILDREN;
const Int32 ViewGroup::FLAG_ALWAYS_DRAWN_WITH_CACHE;
const Int32 ViewGroup::FLAG_CHILDREN_DRAWN_WITH_CACHE;
const Int32 ViewGroup::FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE;
const Int32 ViewGroup::FLAG_MASK_FOCUSABILITY;
const Int32 ViewGroup::DESCENDANT_FOCUSABILITY_FLAGS[] = {
    FOCUS_BEFORE_DESCENDANTS, FOCUS_AFTER_DESCENDANTS,
    FOCUS_BLOCK_DESCENDANTS
};
const Int32 ViewGroup::ARRAY_INITIAL_CAPACITY;
const Int32 ViewGroup::ARRAY_CAPACITY_INCREMENT;
const Int32 ViewGroup::FLAG_USE_CHILD_DRAWING_ORDER;
const Int32 ViewGroup::FLAG_SUPPORT_STATIC_TRANSFORMATIONS;
const Int32 ViewGroup::FLAG_DISALLOW_INTERCEPT;
const Int32 ViewGroup::CLIP_TO_PADDING_MASK;
const Int32 ViewGroup::CHILD_LEFT_INDEX;
const Int32 ViewGroup::CHILD_TOP_INDEX;

const Int32 ViewGroup::FLAG_LAYOUT_MODE_WAS_EXPLICITLY_SET;
const Int32 ViewGroup::FLAG_IS_TRANSITION_GROUP;
const Int32 ViewGroup::FLAG_IS_TRANSITION_GROUP_SET;
const Int32 ViewGroup::FLAG_TOUCHSCREEN_BLOCKS_FOCUS;
const Int32 ViewGroup::LAYOUT_MODE_UNDEFINED;

Int32 ViewGroup::LAYOUT_MODE_DEFAULT = LAYOUT_MODE_CLIP_BOUNDS;

AutoPtr<IPaint> ViewGroup::sDebugPaint;
AutoPtr<ArrayOf<Float> > ViewGroup::sDebugLines;

const Int32 ViewGroup::TouchTarget::ALL_POINTER_IDS = -1; // all ones
const Int32 ViewGroup::TouchTarget::MAX_RECYCLED = 32;
Object ViewGroup::TouchTarget::sRecycleLock;
AutoPtr<ViewGroup::TouchTarget> ViewGroup::TouchTarget::sRecycleBin;
Int32 ViewGroup::TouchTarget::sRecycledCount = 0;

const Int32 ViewGroup::HoverTarget::MAX_RECYCLED = 32;
Object ViewGroup::HoverTarget::sRecycleLock;
AutoPtr<ViewGroup::HoverTarget> ViewGroup::HoverTarget::sRecycleBin;
Int32 ViewGroup::HoverTarget::sRecycledCount = 0;

const Int32 ViewGroup::ChildListForAccessibility::MAX_POOL_SIZE = 32;
AutoPtr<Pools::SynchronizedPool<ViewGroup::ChildListForAccessibility> > ViewGroup::ChildListForAccessibility::sPool;

const Int32 ViewGroup::ViewLocationHolder::COMPARISON_STRATEGY_STRIPE;
const Int32 ViewGroup::ViewLocationHolder::COMPARISON_STRATEGY_LOCATION;
Int32 ViewGroup::ViewLocationHolder::sComparisonStrategy = COMPARISON_STRATEGY_STRIPE;
const Int32 ViewGroup::ViewLocationHolder::MAX_POOL_SIZE = 32;
AutoPtr<Pools::SynchronizedPool<ViewGroup::ViewLocationHolder> > ViewGroup::ViewLocationHolder::sPool;

Boolean ViewGroup::DEBUG_DRAW = FALSE;


CAR_INTERFACE_IMPL(ViewGroup::LayoutParams, Object, IViewGroupLayoutParams);
ViewGroup::LayoutParams::~LayoutParams()
{
}

ViewGroup::LayoutParams::LayoutParams()
    : mWidth(0)
    , mHeight(0)
{}

ECode ViewGroup::LayoutParams::SetBaseAttributes(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 widthAttr,
    /* [in] */ Int32 heightAttr)
{
    FAIL_RETURN(a->GetLayoutDimension(widthAttr, String("layout_width"), &mWidth));
    return a->GetLayoutDimension(heightAttr, String("layout_height"), &mHeight);
}

ECode ViewGroup::LayoutParams::SizeToString(
    /* [in] */ Int32 size,
    /* [out] */ String* des)
{
    VALIDATE_NOT_NULL(des);

    if (size == IViewGroupLayoutParams::WRAP_CONTENT) {
        *des = "wrap-content";
        return NOERROR;
    }
    if (size == IViewGroupLayoutParams::MATCH_PARENT) {
        *des = "match-parent";
        return NOERROR;
    }
    *des = StringUtils::ToString(size);
    return E_NOT_IMPLEMENTED;
}

ECode ViewGroup::LayoutParams::constructor()
{
    return NOERROR;
}

ECode ViewGroup::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::ViewGroup_Layout),
        ArraySize(R::styleable::ViewGroup_Layout));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a));

    ECode ec = SetBaseAttributes(a,
            R::styleable::ViewGroup_Layout_layout_width,
            R::styleable::ViewGroup_Layout_layout_height);
    a->Recycle();
    return ec;
}

ECode ViewGroup::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mWidth = width;
    mHeight = height;
    return NOERROR;
}

ECode ViewGroup::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    source->GetWidth(&mWidth);
    source->GetHeight(&mHeight);
    return NOERROR;
}

ECode ViewGroup::LayoutParams::SetWidth(
    /* [in] */ Int32 width)
{
    mWidth = width;
    return NOERROR;
}

ECode ViewGroup::LayoutParams::SetHeight(
    /* [in] */ Int32 height)
{
    mHeight = height;
    return NOERROR;
}

ECode ViewGroup::LayoutParams::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode ViewGroup::LayoutParams::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode ViewGroup::LayoutParams::SetLayoutAnimationParameters(
    /* [in] */ IAnimationParameters* ap)
{
    mLayoutAnimationParameters = ap;
    return NOERROR;
}

ECode ViewGroup::LayoutParams::GetLayoutAnimationParameters(
    /* [out] */ IAnimationParameters** ap)
{
    VALIDATE_NOT_NULL(ap);
    *ap = mLayoutAnimationParameters;
    REFCOUNT_ADD(*ap);
    return NOERROR;
}

ECode ViewGroup::LayoutParams::ResolveLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    return NOERROR;
}

ECode ViewGroup::LayoutParams::Debug(
    /* [in] */ const String& output,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String widthStr, heightStr;
    SizeToString(mWidth, &widthStr);
    SizeToString(mHeight, &heightStr);
    *result = output + "ViewGroup.LayoutParams={ width=" + widthStr + ", height=" +
            heightStr + " }";
    return NOERROR;
}

ECode ViewGroup::LayoutParams::OnDebugDraw(
    /* [in] */ IView* view,
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    return NOERROR;
}


CAR_INTERFACE_IMPL(ViewGroup::MarginLayoutParams, LayoutParams, IViewGroupMarginLayoutParams)

const Int32 ViewGroup::MarginLayoutParams::DEFAULT_MARGIN_RELATIVE = Elastos::Core::Math::INT32_MIN_VALUE;
const Int32 ViewGroup::MarginLayoutParams::LAYOUT_DIRECTION_MASK = 0x00000003;
const Int32 ViewGroup::MarginLayoutParams::LEFT_MARGIN_UNDEFINED_MASK = 0x00000004;
const Int32 ViewGroup::MarginLayoutParams::RIGHT_MARGIN_UNDEFINED_MASK = 0x00000008;
const Int32 ViewGroup::MarginLayoutParams::RTL_COMPATIBILITY_MODE_MASK = 0x00000010;
const Int32 ViewGroup::MarginLayoutParams::NEED_RESOLUTION_MASK = 0x00000020;
const Int32 ViewGroup::MarginLayoutParams::DEFAULT_MARGIN_RESOLVED = 0;
const Int32 ViewGroup::MarginLayoutParams::UNDEFINED_MARGIN = IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE;

ViewGroup::MarginLayoutParams::MarginLayoutParams()
    : mLeftMargin(0)
    , mTopMargin(0)
    , mRightMargin(0)
    , mBottomMargin(0)
    , mStartMargin(IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE)
    , mEndMargin(IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE)
    , mMarginFlags(0)
{}

ECode ViewGroup::MarginLayoutParams::GetLeftMargin(
    /* [out] */ Int32* leftMargin)
{
    VALIDATE_NOT_NULL(leftMargin);
    *leftMargin = mLeftMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetLeftMargin(
    /* [in] */ Int32 leftMargin)
{
    mLeftMargin = leftMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetTopMargin(
    /* [out] */ Int32* topMargin)
{
    VALIDATE_NOT_NULL(topMargin);
    *topMargin = mTopMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetTopMargin(
    /* [in] */ Int32 topMargin)
{
    mTopMargin = topMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetRightMargin(
    /* [out] */ Int32* rightMargin)
{
    VALIDATE_NOT_NULL(rightMargin);
    *rightMargin = mRightMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetRightMargin(
    /* [in] */ Int32 rightMargin)
{
    mRightMargin = rightMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetBottomMargin(
    /* [out] */ Int32* bottomMargin)
{
    VALIDATE_NOT_NULL(bottomMargin);
    *bottomMargin = mBottomMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetBottomMargin(
    /* [in] */ Int32 bottomMargin)
{
    mBottomMargin = bottomMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetMargins(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    mLeftMargin = left;
    mTopMargin = top;
    mRightMargin = right;
    mBottomMargin = bottom;
    mMarginFlags &= ~LEFT_MARGIN_UNDEFINED_MASK;
    mMarginFlags &= ~RIGHT_MARGIN_UNDEFINED_MASK;
    Boolean isMarginRelative;
    IsMarginRelative(&isMarginRelative);
    if (isMarginRelative) {
        mMarginFlags |= NEED_RESOLUTION_MASK;
    } else {
        mMarginFlags &= ~NEED_RESOLUTION_MASK;
    }
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetMargins(
    /* [out] */ Int32* left,
    /* [out] */ Int32* top,
    /* [out] */ Int32* right,
    /* [out] */ Int32* bottom)
{
    assert(left && top && right && bottom);
    *left = mLeftMargin;
    *top = mTopMargin;
    *right = mRightMargin;
    *bottom = mBottomMargin;

    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetMarginsRelative(
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    mStartMargin = start;
    mTopMargin = top;
    mEndMargin = end;
    mBottomMargin = bottom;
    mMarginFlags |= NEED_RESOLUTION_MASK;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetMarginStart(
    /* [in] */ Int32 start)
{
    mStartMargin = start;
    mMarginFlags |= NEED_RESOLUTION_MASK;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetMarginStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start)
    if (mStartMargin != IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE) {
        *start = mStartMargin;
        return NOERROR;
    }
    if ((mMarginFlags & NEED_RESOLUTION_MASK) == NEED_RESOLUTION_MASK) {
        DoResolveMargins();
    }
    switch(mMarginFlags & LAYOUT_DIRECTION_MASK) {
        case IView::LAYOUT_DIRECTION_RTL:
            *start = mRightMargin;
            return NOERROR;
        case IView::LAYOUT_DIRECTION_LTR:
        default:
            *start = mLeftMargin;
            return NOERROR;
    }
}

ECode ViewGroup::MarginLayoutParams::SetMarginEnd(
    /* [in] */ Int32 end)
{
    mEndMargin = end;
    mMarginFlags |= NEED_RESOLUTION_MASK;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetMarginEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end)
    if (mEndMargin != IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE) {
        *end = mEndMargin;
        return NOERROR;
    }
    if ((mMarginFlags & NEED_RESOLUTION_MASK) == NEED_RESOLUTION_MASK) {
        DoResolveMargins();
    }
    switch(mMarginFlags & LAYOUT_DIRECTION_MASK) {
        case IView::LAYOUT_DIRECTION_RTL:
            *end = mLeftMargin;
            return NOERROR;
        case IView::LAYOUT_DIRECTION_LTR:
        default:
            *end = mRightMargin;
            return NOERROR;
    }
}

ECode ViewGroup::MarginLayoutParams::IsMarginRelative(
    /* [out] */ Boolean* set)
{
    VALIDATE_NOT_NULL(set)
    *set = (mStartMargin != IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE)
        || (mEndMargin != IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE);
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    if (layoutDirection != IView::LAYOUT_DIRECTION_LTR &&
            layoutDirection != IView::LAYOUT_DIRECTION_RTL) {
        return NOERROR;
    }
    if (layoutDirection != (mMarginFlags & LAYOUT_DIRECTION_MASK)) {
        mMarginFlags &= ~LAYOUT_DIRECTION_MASK;
        mMarginFlags |= (layoutDirection & LAYOUT_DIRECTION_MASK);
        Boolean isMarginRelative;
        IsMarginRelative(&isMarginRelative);
        if (isMarginRelative) {
            mMarginFlags |= NEED_RESOLUTION_MASK;
        } else {
            mMarginFlags &= ~NEED_RESOLUTION_MASK;
        }
    }
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetLayoutDirection(
    /* [out] */ Int32* layoutDirection)
{
    VALIDATE_NOT_NULL(layoutDirection)
    *layoutDirection = (mMarginFlags & LAYOUT_DIRECTION_MASK);
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::GetMarginFlags(
    /* [out] */ Byte* leftMargin)
{
    VALIDATE_NOT_NULL(leftMargin);
    *leftMargin = mLeftMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::SetMarginFlags(
    /* [in] */ Byte leftMargin)
{
    mLeftMargin = leftMargin;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::ResolveLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    SetLayoutDirection(layoutDirection);

    // No relative margin or pre JB-MR1 case or no need to resolve, just dont do anything
    // Will use the left and right margins if no relative margin is defined.
    Boolean isMarginRelative;
    IsMarginRelative(&isMarginRelative);
    if (!isMarginRelative ||
            (mMarginFlags & NEED_RESOLUTION_MASK) != NEED_RESOLUTION_MASK) return NOERROR;

    // Proceed with resolution
    DoResolveMargins();

    return NOERROR;
}

void ViewGroup::MarginLayoutParams::DoResolveMargins()
{
    if ((mMarginFlags & RTL_COMPATIBILITY_MODE_MASK) == RTL_COMPATIBILITY_MODE_MASK) {
        // if left or right margins are not defined and if we have some start or end margin
        // defined then use those start and end margins.
        if ((mMarginFlags & LEFT_MARGIN_UNDEFINED_MASK) == LEFT_MARGIN_UNDEFINED_MASK
                && mStartMargin > DEFAULT_MARGIN_RELATIVE) {
            mLeftMargin = mStartMargin;
        }
        if ((mMarginFlags & RIGHT_MARGIN_UNDEFINED_MASK) == RIGHT_MARGIN_UNDEFINED_MASK
                && mEndMargin > DEFAULT_MARGIN_RELATIVE) {
            mRightMargin = mEndMargin;
        }
    } else {
        // We have some relative margins (either the start one or the end one or both). So use
        // them and override what has been defined for left and right margins. If either start
        // or end margin is not defined, just set it to default "0".
        switch(mMarginFlags & LAYOUT_DIRECTION_MASK) {
            case IView::LAYOUT_DIRECTION_RTL:
                mLeftMargin = (mEndMargin > DEFAULT_MARGIN_RELATIVE) ?
                        mEndMargin : DEFAULT_MARGIN_RESOLVED;
                mRightMargin = (mStartMargin > DEFAULT_MARGIN_RELATIVE) ?
                        mStartMargin : DEFAULT_MARGIN_RESOLVED;
                break;
            case IView::LAYOUT_DIRECTION_LTR:
            default:
                mLeftMargin = (mStartMargin > DEFAULT_MARGIN_RELATIVE) ?
                        mStartMargin : DEFAULT_MARGIN_RESOLVED;
                mRightMargin = (mEndMargin > DEFAULT_MARGIN_RELATIVE) ?
                        mEndMargin : DEFAULT_MARGIN_RESOLVED;
                break;
        }
    }
    mMarginFlags &= ~NEED_RESOLUTION_MASK;
}

ECode ViewGroup::MarginLayoutParams::IsLayoutRtl(
    /* [out] */ Boolean* rtl)
{
    VALIDATE_NOT_NULL(rtl)
    *rtl = ((mMarginFlags & LAYOUT_DIRECTION_MASK) == IView::LAYOUT_DIRECTION_RTL);
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::OnDebugDraw(
    /* [in] */ IView* view,
    /* [in] */ ICanvas* canvas)
{
    assert(0 && "TODO");
    /*AutoPtr<IInsets> oi = IsLayoutModeOptical(view.mParent) ? view.getOpticalInsets() : Insets.NONE;

    fillDifference(canvas,
            view.getLeft()   + oi.left,
            view.getTop()    + oi.top,
            view.getRight()  - oi.right,
            view.getBottom() - oi.bottom,
            leftMargin,
            topMargin,
            rightMargin,
            bottomMargin,
            paint);*/

    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    LayoutParams::constructor();
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::ViewGroup_MarginLayout),
        ArraySize(R::styleable::ViewGroup_MarginLayout));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a));

    ECode ec = SetBaseAttributes(a,
            R::styleable::ViewGroup_MarginLayout_layout_width,
            R::styleable::ViewGroup_MarginLayout_layout_height);

    if (FAILED(ec)) {
        a->Recycle();
        return ec;
    }

    Int32 margin;
    a->GetDimensionPixelSize(
            R::styleable::ViewGroup_MarginLayout_layout_margin, -1, &margin);
    if (margin >= 0) {
        mLeftMargin = margin;
        mTopMargin = margin;
        mRightMargin= margin;
        mBottomMargin = margin;
    }
    else {
        a->GetDimensionPixelSize(
                R::styleable::ViewGroup_MarginLayout_layout_marginLeft,
                UNDEFINED_MARGIN, &mLeftMargin);
        if (mLeftMargin == UNDEFINED_MARGIN) {
            mMarginFlags |= LEFT_MARGIN_UNDEFINED_MASK;
            mLeftMargin = DEFAULT_MARGIN_RESOLVED;
        }
        a->GetDimensionPixelSize(
                R::styleable::ViewGroup_MarginLayout_layout_marginRight,
                UNDEFINED_MARGIN, &mRightMargin);
        if (mRightMargin == UNDEFINED_MARGIN) {
            mMarginFlags |= RIGHT_MARGIN_UNDEFINED_MASK;
            mRightMargin = DEFAULT_MARGIN_RESOLVED;
        }
        a->GetDimensionPixelSize(
                R::styleable::ViewGroup_MarginLayout_layout_marginTop,
                DEFAULT_MARGIN_RESOLVED, &mTopMargin);
        a->GetDimensionPixelSize(
                R::styleable::ViewGroup_MarginLayout_layout_marginBottom,
                DEFAULT_MARGIN_RESOLVED, &mBottomMargin);

        a->GetDimensionPixelSize(
                R::styleable::ViewGroup_MarginLayout_layout_marginStart,
                IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE, &mStartMargin);
        a->GetDimensionPixelSize(
                R::styleable::ViewGroup_MarginLayout_layout_marginEnd,
                IViewGroupMarginLayoutParams::DEFAULT_MARGIN_RELATIVE, &mEndMargin);

        Boolean isMarginRelative;
        IsMarginRelative(&isMarginRelative);
        if (isMarginRelative) {
           mMarginFlags |= NEED_RESOLUTION_MASK;
        }
    }

    AutoPtr<IApplicationInfo> info;
    c->GetApplicationInfo((IApplicationInfo**)&info);
    Boolean hasRtlSupport;
    info->HasRtlSupport(&hasRtlSupport);
    Int32 targetSdkVersion;
    info->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion < Build::VERSION_CODES::JELLY_BEAN_MR1 || !hasRtlSupport) {
        mMarginFlags |= RTL_COMPATIBILITY_MODE_MASK;
    }

    // Layout direction is LTR by default
    mMarginFlags |= IView::LAYOUT_DIRECTION_LTR;

    a->Recycle();
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    LayoutParams::constructor(width, height);
    mMarginFlags |= LEFT_MARGIN_UNDEFINED_MASK;
    mMarginFlags |= RIGHT_MARGIN_UNDEFINED_MASK;

    mMarginFlags &= ~NEED_RESOLUTION_MASK;
    mMarginFlags &= ~RTL_COMPATIBILITY_MODE_MASK;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    LayoutParams::constructor(source);
    mMarginFlags |= LEFT_MARGIN_UNDEFINED_MASK;
    mMarginFlags |= RIGHT_MARGIN_UNDEFINED_MASK;

    mMarginFlags &= ~NEED_RESOLUTION_MASK;
    mMarginFlags &= ~RTL_COMPATIBILITY_MODE_MASK;
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    IViewGroupLayoutParams::Probe(source)->GetWidth(&mWidth);
    IViewGroupLayoutParams::Probe(source)->GetHeight(&mHeight);
    source->GetMargins(&mLeftMargin, &mTopMargin, &mRightMargin, &mBottomMargin);
    source->GetMarginStart(&mStartMargin);
    source->GetMarginEnd(&mEndMargin);

    source->GetMarginFlags(&mMarginFlags);
    return NOERROR;
}

ECode ViewGroup::MarginLayoutParams::CopyMarginsFrom(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    source->GetLeftMargin(&mLeftMargin);
    source->GetTopMargin(&mTopMargin);
    source->GetRightMargin(&mRightMargin);
    source->GetBottomMargin(&mBottomMargin);
    source->GetMarginStart(&mStartMargin);
    source->GetMarginEnd(&mEndMargin);
    source->GetMarginFlags(&mMarginFlags);
    return NOERROR;
}

ViewGroup::NotifyAnimationListenerRunnable::NotifyAnimationListenerRunnable(
    /* [in] */ ViewGroup* host)
    : mHost(host)
{
}

ECode ViewGroup::NotifyAnimationListenerRunnable::Run()
{
    AutoPtr<IAnimation> animation;
    mHost->mLayoutAnimationController->GetAnimation((IAnimation**)&animation);
    mHost->mAnimationListener->OnAnimationEnd(animation);
    return NOERROR;
}

ViewGroup::DispatchDrawRunnable::DispatchDrawRunnable(
    /* [in] */ ViewGroup* host)
    : mHost(host)
{
}

ECode ViewGroup::DispatchDrawRunnable::Run()
{
    mHost->NotifyAnimationListener();
    return NOERROR;
}

CAR_INTERFACE_IMPL(ViewGroup::LayoutTransitionListener, Object, ITransitionListener);

ViewGroup::LayoutTransitionListener::LayoutTransitionListener(
    /* [in] */ ViewGroup* host)
    : mHost(host)
{
}

ECode ViewGroup::LayoutTransitionListener::StartTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    // We only care about disappearing items, since we need special logic to keep
    // those items visible after they've been 'removed'
    if (transitionType == ILayoutTransition::DISAPPEARING) {
        mHost->StartViewTransition(view);
    }

    return NOERROR;
}

ECode ViewGroup::LayoutTransitionListener::EndTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    Boolean changing = FALSE;
    if (mHost->mLayoutCalledWhileSuppressed && !(transition->IsChangingLayout(&changing), changing)) {
        mHost->RequestLayout();
        mHost->mLayoutCalledWhileSuppressed = FALSE;
    }
    if (transitionType == ILayoutTransition::DISAPPEARING && !mHost->mTransitioningViews.IsEmpty()) {
        mHost->EndViewTransition(view);
    }

    return NOERROR;
}

ViewGroup::TouchTarget::TouchTarget()
    : mPointerIdBits(0)
{
}

AutoPtr<ViewGroup::TouchTarget> ViewGroup::TouchTarget::Obtain(
    /* [in] */ IView* child,
    /* [in] */ Int32 pointerIdBits)
{
    AutoPtr<TouchTarget> target;
    AutoLock lock(sRecycleLock);
    {
        if (sRecycleBin == NULL) {
            target = new TouchTarget();
        }
        else {
            target = sRecycleBin;
            sRecycleBin = target->mNext;
            sRecycledCount--;
            target->mNext = NULL;
        }
    }

    target->mChild = child;
    target->mPointerIdBits = pointerIdBits;
    return target;
}

void ViewGroup::TouchTarget::Recycle()
{
    AutoLock lock(sRecycleLock);
    {
        if (sRecycledCount < MAX_RECYCLED) {
            mNext = sRecycleBin;
            sRecycleBin = this;
            sRecycledCount += 1;
        }
        else {
            mNext = NULL;
        }
        mChild = NULL;
    }
}


ViewGroup::HoverTarget::HoverTarget()
{
}

AutoPtr<ViewGroup::HoverTarget> ViewGroup::HoverTarget::Obtain(
    /* [in] */ IView* child)
{
    AutoPtr<HoverTarget> target;
    AutoLock lock(sRecycleLock);
    {
        if (sRecycleBin == NULL) {
            target = new HoverTarget();
        }
        else {
            target = sRecycleBin;
            sRecycleBin = target->mNext;
             sRecycledCount--;
            target->mNext = NULL;
        }
    }
    target->mChild = child;
    return target;
}

void ViewGroup::HoverTarget::Recycle()
{
    AutoLock lock(sRecycleLock);
    {
        if (sRecycledCount < MAX_RECYCLED) {
            mNext = sRecycleBin;
            sRecycleBin = this;
            sRecycledCount += 1;
        }
        else {
            mNext = NULL;
        }
        mChild = NULL;
    }
}

ViewGroup::ChildListForAccessibility::ChildListForAccessibility()
{
}

AutoPtr<ViewGroup::ChildListForAccessibility> ViewGroup::ChildListForAccessibility::Obtain(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Boolean sort)
{
    AutoPtr<ChildListForAccessibility> list = sPool->AcquireItem();;
    if (sPool == NULL) {
        list = new ChildListForAccessibility();
    }

    list->Init(parent, sort);
    return list;
}

void ViewGroup::ChildListForAccessibility::Recycle()
{
    Clear();

    sPool->ReleaseItem(this);
}

Int32 ViewGroup::ChildListForAccessibility::GetChildCount()
{
    Int32 result;
    mChildren->GetSize(&result);
    return result;
}

AutoPtr<IView> ViewGroup::ChildListForAccessibility::GetChildAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> temp;
    mChildren->Get(index, (IInterface**)&temp);
    AutoPtr<IView> result = IView::Probe(temp);
    return result;
}

Int32 ViewGroup::ChildListForAccessibility::GetChildIndex(
    /* [in] */ IView* child)
{
    Int32 result;
    mChildren->IndexOf(child, &result);
    return result;
}

void ViewGroup::ChildListForAccessibility::Init(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Boolean sort)
{
    Int32 childCount = 0;
    parent->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        parent->GetChildAt(i, (IView**)&child);
        mChildren->Add((IInterface*)child->Probe(EIID_IInterface));
    }
    if (sort) {
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IInterface> temp;
            mChildren->Get(i, (IInterface**)&temp);
            AutoPtr<IView> child = IView::Probe(temp);
            AutoPtr<ViewLocationHolder> holder = ViewLocationHolder::Obtain(parent, child);
            mHolders->Add((IInterface*)(IObject*)holder.Get());
        }
        Sort(mHolders);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IInterface> temp, view;
            mHolders->Get(i, (IInterface**)&temp);
            AutoPtr<ViewLocationHolder> holder = (ViewLocationHolder*)(IObject*)temp.Get();
            view = (IInterface*)holder->mView->Probe(EIID_IInterface);
            mChildren->Set(i, view);
            holder->Recycle();
        }
        mHolders->Clear();
    }
}

ECode ViewGroup::ChildListForAccessibility::Sort(
    /* [in] */ IList* holders)
{
    // This is gross but the least risky solution. The current comparison
    // strategy breaks transitivity but produces very good results. Coming
    // up with a new strategy requires time which we do not have, so ...
    //try {
        ViewLocationHolder::SetComparisonStrategy(
                ViewLocationHolder::COMPARISON_STRATEGY_STRIPE);
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        ECode ec = collections->Sort(holders);
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            ViewLocationHolder::SetComparisonStrategy(
                ViewLocationHolder::COMPARISON_STRATEGY_LOCATION);
            collections->Sort(holders);
        }
    //} catch (IllegalArgumentException iae) {
        // Note that in practice this occurs extremely rarely in a couple
        // of pathological cases.
    //}
    return NOERROR;
}

void ViewGroup::ChildListForAccessibility::Clear()
{
    mChildren->Clear();
}

ViewGroup::ViewLocationHolder::ViewLocationHolder()
    : mLayoutDirection(0)
{
    CRect::New((IRect**)&mLocation);
}

AutoPtr<ViewGroup::ViewLocationHolder> ViewGroup::ViewLocationHolder::Obtain(
    /* [in] */ IViewGroup* root,
    /* [in] */ IView* view)
{
    AutoPtr<ViewLocationHolder> holder = sPool->AcquireItem();
    if (sPool == NULL) {
        holder = new ViewLocationHolder();
    }
    holder->Init(root, view);
    return holder;
}

void ViewGroup::ViewLocationHolder::SetComparisonStrategy(
    /* [in] */ Int32 strategy)
{
    sComparisonStrategy = strategy;
}

void ViewGroup::ViewLocationHolder::Recycle()
{
    Clear();
    sPool->ReleaseItem(this);
}

Int32 ViewGroup::ViewLocationHolder::CompareTo(
    /* [in] */ ViewLocationHolder* another)
{
    // This instance is greater than an invalid argument.
    if (another == NULL) {
        return 1;
    }

    CRect* rect = (CRect*)mLocation.Get();
    CRect* rectAnother = (CRect*)another->mLocation.Get();

    if (sComparisonStrategy == COMPARISON_STRATEGY_STRIPE) {
        // First is above second.
        if (rect->mBottom - rectAnother->mTop <= 0) {
            return -1;
        }
        // First is below second.
        if (rect->mTop - rectAnother->mBottom >= 0) {
            return 1;
        }
    }

    // We are ordering left-to-right, top-to-bottom.
    if (mLayoutDirection == IView::LAYOUT_DIRECTION_LTR) {
        Int32 leftDifference = rect->mLeft - rectAnother->mLeft;
        // First more to the left than second.
        if (leftDifference != 0) {
            return leftDifference;
        }
    }
    else { // RTL
        Int32 rightDifference = rect->mRight - rectAnother->mRight;
        // First more to the right than second.
        if (rightDifference != 0) {
            return -rightDifference;
        }
    }
    // Break tie by top.
    Int32 topDiference = rect->mTop - rectAnother->mTop;
    if (topDiference != 0) {
        return topDiference;
    }
    // Break tie by height.
    Int32 h1, h2;
    mLocation->GetHeight(&h1);
    another->mLocation->GetHeight(&h2);
    Int32 heightDiference = h1 - h2;
    if (heightDiference != 0) {
        return -heightDiference;
    }
    // Break tie by width.
    Int32 w1, w2;
    mLocation->GetHeight(&w1);
    another->mLocation->GetHeight(&w2);
    Int32 widthDiference = w1 - w2;
    if (widthDiference != 0) {
        return -widthDiference;
    }

    // Just break the tie somehow. The accessibility ids are unique
    // and stable, hence this is deterministic tie breaking.
    Int32 id, anotherId;
    mView->GetAccessibilityViewId(&id);
    another->mView->GetAccessibilityViewId(&anotherId);
    return id - anotherId;
}

void ViewGroup::ViewLocationHolder::Init(
    /* [in] */ IViewGroup* root,
    /* [in] */ IView* view)
{
    view->GetDrawingRect(mLocation);
    root->OffsetDescendantRectToMyCoords(view, mLocation);
    assert(view != NULL);
    mView = VIEW_PROBE(view);
    (IView::Probe(root))->GetLayoutDirection(&mLayoutDirection);
}

void ViewGroup::ViewLocationHolder::Clear()
{
    mView = NULL;
    mLocation->Set(0, 0, 0, 0);
}

CAR_INTERFACE_IMPL_3(ViewGroup, View, IViewGroup, IViewParent, IViewManager)

ViewGroup::ViewGroup()
    : mGroupFlags(0)
    , mPersistentDrawingCache(0)
    , mChildAcceptsDrag(FALSE)
    , mLastTouchDownTime(0)
    , mLastTouchDownIndex(-1)
    , mLastTouchDownX(0.0f)
    , mLastTouchDownY(0.0f)
    , mHoveredSelf(FALSE)
    , mLayoutMode(LAYOUT_MODE_UNDEFINED)
    , mChildrenCount(0)
    , mSuppressLayout(FALSE)
    , mLayoutCalledWhileSuppressed(FALSE)
    , mChildCountWithTransientState(0)
{
    CTransformation::New((ITransformation**)&mChildTransformation);
    CPointF::New((IPointF**)&mLocalPoint);

    mLayoutTransitionListener = new LayoutTransitionListener(this);
}

ECode ViewGroup::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ViewGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ViewGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ViewGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(View::constructor(context, attrs, defStyleAttr, defStyleRes))
    InitViewGroup();
    InitFromAttributes(context, attrs, defStyleAttr, defStyleRes);
    return NOERROR;
}

Boolean ViewGroup::DebugDraw()
{
    return DEBUG_DRAW || (mAttachInfo != NULL && mAttachInfo->mDebugLayout);
}

void ViewGroup::InitViewGroup()
{
    // ViewGroup doesn't draw by default
    if (!DebugDraw()) {
        SetFlags(WILL_NOT_DRAW, DRAW_MASK);
    }
    mGroupFlags |= FLAG_CLIP_CHILDREN;
    mGroupFlags |= FLAG_CLIP_TO_PADDING;
    mGroupFlags |= FLAG_ANIMATION_DONE;
    mGroupFlags |= FLAG_ANIMATION_CACHE;
    mGroupFlags |= FLAG_ALWAYS_DRAWN_WITH_CACHE;

    AutoPtr<IApplicationInfo> appInfo;
    mContext->GetApplicationInfo((IApplicationInfo**)&appInfo);
    assert(appInfo != NULL);
    Int32 targetSdkVersion = 0;
    appInfo->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion >= Build::VERSION_CODES::HONEYCOMB) {
        mGroupFlags |= FLAG_SPLIT_MOTION_EVENTS;
    }

    SetDescendantFocusability(FOCUS_BEFORE_DESCENDANTS);
    mChildren = ArrayOf<IView*>::Alloc(ARRAY_INITIAL_CAPACITY);
    mChildrenCount = 0;

    mPersistentDrawingCache = PERSISTENT_SCROLLING_CACHE;
}

void ViewGroup::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::ViewGroup),
        ArraySize(R::styleable::ViewGroup));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
        attrs, attrIds, (ITypedArray**)&a));

    Int32 N;
    a->GetIndexCount(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 attr;
        a->GetIndex(i, &attr);
        switch (attr) {
            case R::styleable::ViewGroup_clipChildren:
            {
                Boolean value;
                a->GetBoolean(attr, TRUE, &value);
                SetClipChildren(value);
                break;
            }
            case R::styleable::ViewGroup_clipToPadding:
            {
                Boolean value;
                a->GetBoolean(attr, TRUE, &value);
                SetClipToPadding(value);
                break;
            }
            case R::styleable::ViewGroup_animationCache:
            {
                Boolean value;
                a->GetBoolean(attr, TRUE, &value);
                SetAnimationCacheEnabled(value);
                break;
            }
            case R::styleable::ViewGroup_persistentDrawingCache:
            {
                Int32 value;
                a->GetInt32(attr, PERSISTENT_SCROLLING_CACHE, &value);
                SetPersistentDrawingCache(value);
                break;
            }
            case R::styleable::ViewGroup_addStatesFromChildren:
            {
                Boolean value;
                a->GetBoolean(attr, FALSE, &value);
                SetAddStatesFromChildren(value);
                break;
            }
            case R::styleable::ViewGroup_alwaysDrawnWithCache:
            {
                Boolean value;
                a->GetBoolean(attr, TRUE, &value);
                SetAlwaysDrawnWithCacheEnabled(value);
                break;
            }
            case R::styleable::ViewGroup_layoutAnimation:
            {
                Int32 id;
                a->GetResourceId(attr, -1, &id);
                if (id > 0) {
                    AutoPtr<ILayoutAnimationController> controller;
                    AnimationUtils::LoadLayoutAnimation(
                        mContext, id, (ILayoutAnimationController**)&controller);
                    SetLayoutAnimation(controller);
                }
                break;
            }
            case R::styleable::ViewGroup_descendantFocusability:
            {
                Int32 value;
                a->GetInt32(attr, 0, &value);
                SetDescendantFocusability(DESCENDANT_FOCUSABILITY_FLAGS[value]);
                break;
            }
            case R::styleable::ViewGroup_splitMotionEvents: {
                Boolean value = FALSE;
                a->GetBoolean(attr, FALSE, &value);
                SetMotionEventSplittingEnabled(value);
                break;
            }
            case R::styleable::ViewGroup_animateLayoutChanges: {
                Boolean animateLayoutChanges = FALSE;
                a->GetBoolean(attr, FALSE, &animateLayoutChanges);
                if (animateLayoutChanges) {
                    AutoPtr<ILayoutTransition> transition;
                    CLayoutTransition::New((ILayoutTransition**)&transition);
                    SetLayoutTransition(transition);
                }
                break;
            }
            case R::styleable::ViewGroup_layoutMode: {
                Int32 temp;
                a->GetInt32(attr, LAYOUT_MODE_UNDEFINED, &temp);
                SetLayoutMode(temp);
                break;
            }
            case R::styleable::ViewGroup_transitionGroup: {
                Boolean value = FALSE;
                a->GetBoolean(attr, FALSE, &value);
                SetTransitionGroup(value);
                break;
            }
            case R::styleable::ViewGroup_touchscreenBlocksFocus:
            {
                Boolean value = FALSE;
                a->GetBoolean(attr, FALSE, &value);
                SetTouchscreenBlocksFocus(value);
                break;
            }
            default:
                break;
        }
    }

    a->Recycle();
}

ECode ViewGroup::GetDescendantFocusability(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mGroupFlags & FLAG_MASK_FOCUSABILITY;
    return NOERROR;
}

ECode ViewGroup::SetDescendantFocusability(
    /* [in] */ Int32 focusability)
{
    switch (focusability) {
        case FOCUS_BEFORE_DESCENDANTS:
        case FOCUS_AFTER_DESCENDANTS:
        case FOCUS_BLOCK_DESCENDANTS:
            break;
        default:
        Slogger::E("ViewGroup", "must be one of FOCUS_BEFORE_DESCENDANTS, FOCUS_AFTER_DESCENDANTS, FOCUS_BLOCK_DESCENDANTS");
//            throw new IllegalArgumentException("must be one of FOCUS_BEFORE_DESCENDANTS, "
//                    + "FOCUS_AFTER_DESCENDANTS, FOCUS_BLOCK_DESCENDANTS");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mGroupFlags &= ~FLAG_MASK_FOCUSABILITY;
    mGroupFlags |= (focusability & FLAG_MASK_FOCUSABILITY);
    return NOERROR;
}

ECode ViewGroup::HandleFocusGainInternal(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    if (mFocused != NULL) {
        (VIEW_PROBE(mFocused))->UnFocus(this);
        mFocused = NULL;
    }

    return View::HandleFocusGainInternal(direction, previouslyFocusedRect);
}

ECode ViewGroup::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    if (DBG) {
        Slogger::D(TAG, /*this + */" requestChildFocus()");
    }

    Int32 descendantFocusability;
    GetDescendantFocusability(&descendantFocusability);
    if (descendantFocusability == FOCUS_BLOCK_DESCENDANTS) {
        return NOERROR;
    }

    // Unfocus us, if necessary
    View::UnFocus(focused);

    // We had a previous notion of who had focus. Clear it.
    if (mFocused.Get() != child) {
        if (mFocused != NULL) {
            (VIEW_PROBE(mFocused))->UnFocus(focused);
        }

        mFocused = child;
    }

    if (mParent != NULL) {
        mParent->RequestChildFocus(this, focused);
    }
    return NOERROR;
}

ECode ViewGroup::FocusableViewAvailable(
    /* [in] */ IView* v)
{
    Int32 descendantFocusability;
    GetDescendantFocusability(&descendantFocusability);
    Boolean isFocused, isFocusableInTouchMode;
    IsFocused(&isFocused);
    IsFocusableInTouchMode(&isFocusableInTouchMode);

    Boolean should = FALSE;
    if (mParent != NULL
        // shortcut: don't report a new focusable view if we block our descendants from
        // getting focus
        && (descendantFocusability != FOCUS_BLOCK_DESCENDANTS)
        && (isFocusableInTouchMode || (ShouldBlockFocusForTouchscreen(&should), !should))
        // shortcut: don't report a new focusable view if we already are focused
        // (and we don't prefer our descendants)
        //
        // note: knowing that mFocused is non-NULL is not a good enough reason
        // to break the traversal since in that case we'd actually have to find
        // the focused view and make sure it wasn't FOCUS_AFTER_DESCENDANTS and
        // an ancestor of v; this will get checked for at ViewAncestor
        && !(isFocused && descendantFocusability != FOCUS_AFTER_DESCENDANTS)) {
        mParent->FocusableViewAvailable(v);
    }
    return NOERROR;
}

ECode ViewGroup::ShowContextMenuForChild(
    /* [in] */ IView* originalView,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mParent != NULL) {
        mParent->ShowContextMenuForChild(originalView, res);
    }

    return NOERROR;
}

ECode ViewGroup::StartActionModeForChild(
    /* [in] */ IView* originalView,
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** res)
{
    VALIDATE_NOT_NULL(res)
    if (mParent != NULL) {
        mParent->StartActionModeForChild(originalView, callback, res);
        return NOERROR;
    }

    *res = NULL;
    return NOERROR;
}

ECode ViewGroup::FocusSearch(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res)
    Boolean isRootNamespace;
    if (IsRootNamespace(&isRootNamespace), isRootNamespace) {
        // root namespace means we should consider ourselves the top of the
        // tree for focus searching; otherwise we could be focus searching
        // into other tabs.  see LocalActivityManager and TabHost for more info
        //
        FocusFinder::GetInstance()->FindNextFocus(
            this, focused, direction, res);
    }
    else if (mParent != NULL) {
        mParent->FocusSearch(focused, direction, res);
    }

    return NOERROR;
}

ECode ViewGroup::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

ECode ViewGroup::RequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IViewParent> parent = mParent;
    if (parent == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    Boolean propagate;
    OnRequestSendAccessibilityEvent(child, event, &propagate);
    if (!propagate) {
        *res = FALSE;
        return NOERROR;
    }
    parent->RequestSendAccessibilityEvent(this, event, res);
    return NOERROR;
}

ECode ViewGroup::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mAccessibilityDelegate != NULL) {
        mAccessibilityDelegate->OnRequestSendAccessibilityEvent(
                this, child, event, res);
        return NOERROR;
    }
    else {
        *res = OnRequestSendAccessibilityEventInternal(child, event);
        return NOERROR;
    }
}

Boolean ViewGroup::OnRequestSendAccessibilityEventInternal(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event)
{
    return TRUE;
}

/**
 * Translates the given bounds and intersections from child coordinates to
 * local coordinates. In case any interactive sibling of the calling child
 * covers the latter, a new intersections is added to the intersection list.
 * This method is for the exclusive use by the accessibility layer to compute
 * a point where a sequence of down and up events would click on a view.
 *
 * @param child The child making the call.
 * @param bounds The bounds to translate in child coordinates.
 * @param intersections The intersections of interactive views covering the child.
 * @return True if the bounds and intersections were computed, false otherwise.
 */
ECode ViewGroup::TranslateBoundsAndIntersectionsInWindowCoordinates(
    /* [in] */ IView* child,
    /* [in] */ IRectF* bounds,
    /* [in] */ IList* intersections,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    // Not attached, done.
    if (mAttachInfo == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Float alpha, transitionAlpha;
    Int32 visibility;
    if ((GetAlpha(&alpha), alpha) <= 0 || (GetTransitionAlpha(&transitionAlpha), transitionAlpha) <= 0
        || (GetVisibility(&visibility), visibility) != IView::VISIBLE) {
        // Cannot click on a view with an invisible predecessor.
        *res = FALSE;
        return NOERROR;
    }

    // Compensate for the child transformation.
    Boolean hasIdentityMatrix = ((View*)child)->HasIdentityMatrix();
    if (!hasIdentityMatrix) {
        AutoPtr<IMatrix> matrix;
        child->GetMatrix((IMatrix**)&matrix);
        matrix->MapRect(bounds, &hasIdentityMatrix);

        Int32 intersectionCount;
        intersections->GetSize(&intersectionCount);

        for (Int32 i = 0; i < intersectionCount; i++)
        {
            AutoPtr<IInterface> temp;
            intersections->Get(i, (IInterface**)&temp);
            AutoPtr<IRectF> intersection = IRectF::Probe(temp);
            matrix->MapRect(intersection, &hasIdentityMatrix);
        }
    }

    // Translate the bounds from child to parent coordinates.
    View* temp = ((View*)child);
    Int32 dx = temp->mLeft - mScrollX;
    Int32 dy = temp->mTop - mScrollY;
    bounds->Offset(dx, dy);
    OffsetRects(IArrayList::Probe(intersections), dx, dy);

    // If the bounds do not intersect our bounds, done.
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    Boolean result;
    bounds->Intersects(0, 0, width, height, &result);
    if (!result) {
        *res = FALSE;
        return NOERROR;
    }

    // Check whether any clickable siblings cover the child
    // view and if so keep track of the intersections. Also
    // respect Z ordering when iterating over children.
    AutoPtr<IList> orderedList = BuildOrderedChildList();
    Boolean useCustomOrder = orderedList == NULL
            && IsChildrenDrawingOrderEnabled();

    Int32 childCount = mChildrenCount;
    for (Int32 i = childCount - 1; i >= 0; i--) {
        Int32 childIndex = useCustomOrder
                ? GetChildDrawingOrder(childCount, i) : i;
        AutoPtr<IView> sibling;
        if (orderedList == NULL) {
            sibling = (*mChildren)[childIndex];
        } else {
            AutoPtr<IInterface> temp;
            orderedList->Get(childIndex, (IInterface**)&temp);
            sibling = IView::Probe(temp);
        }

        // We care only about siblings over the child.
        if (child == sibling) {
            break;
        }

        // Ignore invisible views as they are not interactive.
        Int32 siblingVisibility;
        sibling->GetVisibility(&siblingVisibility);
        if (siblingVisibility != IView::VISIBLE) {
            continue;
        }

        // If sibling is not interactive we do not care.
        Boolean isClickable, isLongClickable;
        sibling->IsClickable(&isClickable);
        sibling->IsLongClickable(&isLongClickable);
        if (!isClickable && !isLongClickable) {
            continue;
        }

        // Compute the sibling bounds in its coordinates.
        AutoPtr<IRectF> siblingBounds = mAttachInfo->mTmpTransformRect1;
        Int32 siblingW, siblingH;
        sibling->GetWidth(&siblingW);
        sibling->GetHeight(&siblingH);
        siblingBounds->Set(0, 0, siblingW, siblingH);

        // Take into account the sibling transformation matrix.
        Boolean siblingHasIdentityMatrix = (VIEW_PROBE(sibling))->HasIdentityMatrix();
        if (!siblingHasIdentityMatrix) {
            AutoPtr<IMatrix> matrix;
            sibling->GetMatrix((IMatrix**)&matrix);
            Boolean result;
            matrix->MapRect(siblingBounds, &result);
        }

        // Offset the sibling to our coordinates.
        View* siblingTemp = VIEW_PROBE(sibling);
        Int32 siblingDx = siblingTemp->mLeft - mScrollX;
        Int32 siblingDy = siblingTemp->mTop - mScrollY;
        siblingBounds->Offset(siblingDx, siblingDy);

        // Compute the intersection between the child and the sibling.
        Boolean siblingBoundsIntersect;
        siblingBounds->Intersect(bounds, &siblingBoundsIntersect);
        if (siblingBoundsIntersect) {
            // If an interactive sibling completely covers the child, done.
            siblingBounds->Equals(bounds, &siblingBoundsIntersect);
            if (siblingBoundsIntersect) {
                *res = FALSE;
                return NOERROR;
            }
            // Keep track of the intersection rectangle.
            AutoPtr<IRectF> intersection;
            CRectF::New(siblingBounds, (IRectF**)&intersection);
            intersections->Add((IInterface*)intersection->Probe(EIID_IInterface));
        }
    }

    if (IViewGroup::Probe(mParent)) {
        AutoPtr<ViewGroup> parentGroup = (ViewGroup*)IViewGroup::Probe(mParent);
        parentGroup->TranslateBoundsAndIntersectionsInWindowCoordinates(
                this, bounds, intersections, res);
        return NOERROR;
    }
    *res = TRUE;
    return NOERROR;
}

ECode ViewGroup::ChildHasTransientStateChanged(
    /* [in] */ IView* child,
    /* [in] */ Boolean childHasTransientState)
{
    Boolean oldHasTransientState;
    HasTransientState(&oldHasTransientState);
    if (childHasTransientState) {
        mChildCountWithTransientState++;
    }
    else {
        mChildCountWithTransientState--;
    }

    Boolean newHasTransientState;
    HasTransientState(&newHasTransientState);
    if (mParent != NULL && oldHasTransientState != newHasTransientState) {
        return mParent->ChildHasTransientStateChanged(this, newHasTransientState);
    }

    return NOERROR;
}

ECode ViewGroup::HasTransientState(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean hasTransientState;
    View::HasTransientState(&hasTransientState);
    *res = mChildCountWithTransientState > 0 || hasTransientState;
    return NOERROR;
}

ECode ViewGroup::DispatchUnhandledMove(
     /* [in] */ IView* focused,
     /* [in] */ Int32 direction,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mFocused != NULL) {
        mFocused->DispatchUnhandledMove(focused, direction, res);
    }

    return NOERROR;
}

ECode ViewGroup::ClearChildFocus(
    /* [in] */ IView* child)
{
    if (DBG) {
        Slogger::D(TAG, "ViewGroup %p: clearChildFocus()", this);
    }

    mFocused = NULL;
    if (mParent != NULL) {
        mParent->ClearChildFocus(this);
    }

    return NOERROR;
}

ECode ViewGroup::ClearFocus()
{
    if (mFocused == NULL) {
        View::ClearFocus();
    }
    else {
        AutoPtr<IView> focused = mFocused;
        mFocused = NULL;
        focused->ClearFocus();
    }

    return NOERROR;
}

ECode ViewGroup::UnFocus(
    /* [in] */ IView* focused)
{
    if (mFocused == NULL) {
        View::UnFocus(focused);
    }
    else {
        (VIEW_PROBE(mFocused))->UnFocus(focused);
        mFocused = NULL;
    }

    return NOERROR;
}

ECode ViewGroup::GetFocusedChild(
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mFocused;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode ViewGroup::GetDeepestFocusedChild(
    /* [out] */ IView** focused)
{
    VALIDATE_NOT_NULL(focused)
    *focused = NULL;

    AutoPtr<IView> v = this;
    while (v != NULL) {
        Boolean res;
        v->IsFocused(&res);
        if (res) {
            *focused = v;
            REFCOUNT_ADD(*focused);
            return NOERROR;
        }
        AutoPtr<IViewGroup> vg = IViewGroup::Probe(v);
        v = NULL;
        if (vg) {
            vg->GetFocusedChild((IView**)&v);
        }
    }
    return NOERROR;
}

ECode ViewGroup::HasFocus(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = (mPrivateFlags & PFLAG_FOCUSED) != 0 || mFocused != NULL;
    return NOERROR;
}

ECode ViewGroup::FindFocus(
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res)
    Boolean isFocused;
    if (IsFocused(&isFocused), isFocused) {
        *res = this;
        REFCOUNT_ADD(*res)
        return NOERROR;
    }

    if (mFocused != NULL) {
        return (VIEW_PROBE(mFocused))->FindFocus(res);
    }
    *res = NULL;
    return NOERROR;
}

ECode ViewGroup::HasFocusable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if ((mViewFlags & VISIBILITY_MASK) != IView::VISIBLE) {
        *res = FALSE;
        return NOERROR;
    }
    Boolean isFocused;
    if (IsFocusable(&isFocused), isFocused) {
        *res = TRUE;
        return NOERROR;
    }
    Int32 descendantFocusability;
    GetDescendantFocusability(&descendantFocusability);
    if (descendantFocusability != FOCUS_BLOCK_DESCENDANTS) {
        for (Int32 i = 0; i < mChildrenCount; i++) {
            Boolean hasFocusable = FALSE;
            (*mChildren)[i]->HasFocusable(&hasFocusable);
            if (hasFocusable) {
                *res = TRUE;
                return NOERROR;
            }
        }
    }

    *res = FALSE;
    return NOERROR;
}

ECode ViewGroup::AddFocusables(
    /* [in] */ IList* views,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 focusableMode)
{
    assert(views);

    Int32 focusableCount;
    views->GetSize(&focusableCount);

    Int32 descendantFocusability = 0;
    GetDescendantFocusability(&descendantFocusability);
    if (descendantFocusability != FOCUS_BLOCK_DESCENDANTS) {
        Boolean should = FALSE;
        if (ShouldBlockFocusForTouchscreen(&should), should) {
            focusableMode |= FOCUSABLES_TOUCH_MODE;
        }
        for (Int32 i = 0; i < mChildrenCount; i++) {
            View* temp = (View*)(*mChildren)[i];
            if ((temp->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
                temp->AddFocusables(IArrayList::Probe(views), direction, focusableMode);
            }
        }
    }

    // we add ourselves (if focusable) in all cases except for when we are
    // FOCUS_AFTER_DESCENDANTS and there are some descendants focusable.  this is
    // to avoid the focus search finding layouts when a more precise search
    // among the focusable children would be more interesting.
    //
    Int32 size;
    views->GetSize(&size);
    Boolean isFocusableInTouchMode = FALSE, should = FALSE;
    IsFocusableInTouchMode(&isFocusableInTouchMode);
    if ((descendantFocusability != FOCUS_AFTER_DESCENDANTS
            // No focusable descendants
            || (focusableCount == size)) &&
            (isFocusableInTouchMode || (ShouldBlockFocusForTouchscreen(&should), !should))) {
            View::AddFocusables(IArrayList::Probe(views), direction, focusableMode);
    }

    return NOERROR;
}

/**
 * Set whether this ViewGroup should ignore focus requests for itself and its children.
 * If this option is enabled and the ViewGroup or a descendant currently has focus, focus
 * will proceed forward.
 *
 * @param touchscreenBlocksFocus true to enable blocking focus in the presence of a touchscreen
 */
ECode ViewGroup::SetTouchscreenBlocksFocus(
    /* [in] */ Boolean touchscreenBlocksFocus)
{
    if (touchscreenBlocksFocus) {
        mGroupFlags |= FLAG_TOUCHSCREEN_BLOCKS_FOCUS;
        Boolean focus;
        if (HasFocus(&focus), focus) {
            AutoPtr<IView> focusedChild;
            GetDeepestFocusedChild((IView**)&focusedChild);
            Boolean isFocusableInTouchMode;
            focusedChild->IsFocusableInTouchMode(&isFocusableInTouchMode);
            if (!isFocusableInTouchMode) {
                AutoPtr<IView> newFocus;
                FocusSearch(IView::FOCUS_FORWARD, (IView**)&newFocus);
                if (newFocus != NULL) {
                    newFocus->RequestFocus(&isFocusableInTouchMode);
                }
            }
        }
    } else {
        mGroupFlags &= ~FLAG_TOUCHSCREEN_BLOCKS_FOCUS;
    }
    return NOERROR;
}

/**
 * Check whether this ViewGroup should ignore focus requests for itself and its children.
 */
ECode ViewGroup::GetTouchscreenBlocksFocus(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = (mGroupFlags & FLAG_TOUCHSCREEN_BLOCKS_FOCUS) != 0;
    return NOERROR;
}

ECode ViewGroup::ShouldBlockFocusForTouchscreen(
    /* [out] */ Boolean* should)
{
    VALIDATE_NOT_NULL(should);
    Boolean touchscreenBlocksFocus, hasSystemFeature;
    GetTouchscreenBlocksFocus(&touchscreenBlocksFocus);
    AutoPtr<IPackageManager> pmg;
    mContext->GetPackageManager((IPackageManager**)&pmg);
    pmg->HasSystemFeature(IPackageManager::FEATURE_TOUCHSCREEN, &hasSystemFeature);
    *should = touchscreenBlocksFocus && hasSystemFeature;
    return NOERROR;
}

ECode ViewGroup::FindViewsWithText(
    /* [in, out] */ IArrayList* outViews,
    /* [in] */ ICharSequence* searched,
    /* [in] */ Int32 flags)
{
    View::FindViewsWithText(outViews, searched, flags);
    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* child = (View*)(*mChildren)[i];
        if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE
                && (child->mPrivateFlags & PFLAG_IS_ROOT_NAMESPACE) == 0) {
            child->FindViewsWithText(outViews, searched, flags);
        }
    }

    return NOERROR;
}

ECode ViewGroup::FindViewByAccessibilityIdTraversal(
    /* [in] */ Int32 accessibilityId,
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IView> foundView;
    View::FindViewByAccessibilityIdTraversal(accessibilityId, (IView**)&foundView);
    if (foundView != NULL) {
        *res = foundView;
        REFCOUNT_ADD(*res)
        return NOERROR;
    }

    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* child = (View*)(*mChildren)[i];
        child->FindViewByAccessibilityIdTraversal(accessibilityId, (IView**)&foundView);
        if (foundView != NULL) {
           *res = foundView;
           REFCOUNT_ADD(*res)
           return NOERROR;
        }
    }
    *res = NULL;
    return NOERROR;
}

ECode ViewGroup::DispatchWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    View::DispatchWindowFocusChanged(hasFocus);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->DispatchWindowFocusChanged(hasFocus);
    }

    return NOERROR;
}

ECode ViewGroup::AddTouchables(
    /* [in] */ IArrayList* views)
{
    View::AddTouchables(views);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* temp = (View*)(*mChildren)[i];
        if ((temp->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
            temp->AddTouchables(views);
        }
    }

    return NOERROR;
}

ECode ViewGroup::MakeOptionalFitsSystemWindows()
{
    View::MakeOptionalFitsSystemWindows();
    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->MakeOptionalFitsSystemWindows();
    }

    return NOERROR;
}

ECode ViewGroup::DispatchDisplayHint(
    /* [in] */ Int32 hint)
{
    View::DispatchDisplayHint(hint);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->DispatchDisplayHint(hint);
    }

    return NOERROR;
}

void ViewGroup::OnChildVisibilityChanged(
    /* [in] */ IView* child,
    /* [in] */ Int32 oldVisibility,
    /* [in] */ Int32 newVisibility)
{
    if (mTransition != NULL) {
        if (newVisibility == IView::VISIBLE) {
            mTransition->ShowChild(this, child, oldVisibility);
        }
        else {
            mTransition->HideChild(this, child, newVisibility);
            if (!mTransitioningViews.IsEmpty()) {
                AutoPtr<IView> temp = child;
                List<AutoPtr<IView> >::Iterator iter =
                    Find(mTransitioningViews.Begin(), mTransitioningViews.End(), temp);
                if (iter != mTransitioningViews.End()) {
                    // Only track this on disappearing views - appearing views are already visible
                    // and don't need special handling during drawChild()
                    mVisibilityChangingChildren.PushBack(child);
                    AddDisappearingView(child);
                }
            }
        }
    }

    // in all cases, for drags
    if (mCurrentDrag != NULL) {
        if (newVisibility == IView::VISIBLE) {
            NotifyChildOfDrag(child);
        }
    }
}

void ViewGroup::DispatchVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    View::DispatchVisibilityChanged(changedView, visibility);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        ((View*)(*mChildren)[i])->DispatchVisibilityChanged(
            changedView, visibility);
    }
}

ECode ViewGroup::DispatchWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    View::DispatchWindowVisibilityChanged(visibility);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->DispatchWindowVisibilityChanged(visibility);
    }

    return NOERROR;
}

ECode ViewGroup::DispatchConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    View::DispatchConfigurationChanged(newConfig);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->DispatchConfigurationChanged(newConfig);
    }

    return NOERROR;
}

ECode ViewGroup::RecomputeViewAttributes(
    /* [in] */ IView* child)
{
    if (mAttachInfo != NULL && !mAttachInfo->mRecomputeGlobalAttributes) {
        if (mParent != NULL) {
            return mParent->RecomputeViewAttributes(this);
        }
    }

    return NOERROR;
}

ECode ViewGroup::DispatchCollectViewAttributes(
    /* [in] */ AttachInfo* attachInfo,
    /* [in] */ Int32 visibility)
{
    if ((visibility & VISIBILITY_MASK) == IView::VISIBLE) {
        View::DispatchCollectViewAttributes(attachInfo, visibility);
        for (Int32 i = 0; i < mChildrenCount; i++) {
            View* child = (View*)(*mChildren)[i];
            child->DispatchCollectViewAttributes(attachInfo,
                    visibility | (child->mViewFlags & VISIBILITY_MASK));
        }
    }

    return NOERROR;
}

ECode ViewGroup::BringChildToFront(
    /* [in] */ IView* child)
{
    Int32 index = 0;
    IndexOfChild(child, &index);
    if (index >= 0) {
        RemoveFromArray(index);
        AddInArray(child, mChildrenCount);
        ((View*)child)->mParent = this;
        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

AutoPtr<IPointF> ViewGroup::GetLocalPoint()
{
    if (mLocalPoint == NULL) {
        CPointF::New((IPointF**)&mLocalPoint);
    }
    return mLocalPoint;
}

ECode ViewGroup::DispatchDragEvent(
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean retval = FALSE;
    Float tx = 0.0f;
    event->GetX(&tx);
    Float ty = 0.0f;
    event->GetY(&ty);

    AutoPtr<IViewRootImpl> root;
    GetViewRootImpl((IViewRootImpl**)&root);

    // Dispatch down the view hierarchy
    AutoPtr<IPointF> localPoint = GetLocalPoint();

    Int32 action = 0;
    event->GetAction(&action);
    // Dispatch down the view hierarchy
    switch (action) {
    case IDragEvent::ACTION_DRAG_STARTED: {
        // clear state to recalculate which views we drag over
        mCurrentDragView = NULL;

        // Set up our tracking of drag-started notifications
        //mCurrentDrag = CDragEvent::Obtain(event);
        mDragNotifiedChildren.Clear();

        // Now dispatch down to our children, caching the responses
        mChildAcceptsDrag = FALSE;
        for (Int32 i = 0; i < mChildrenCount; i++) {
            View* child = (View*)(*mChildren)[i];
            child->mPrivateFlags2 &= ~DRAG_MASK;
            Int32 childVisibility;
            child->GetVisibility(&childVisibility);
            if (childVisibility == IView::VISIBLE) {
                Boolean handled = NotifyChildOfDrag((*mChildren)[i]);
                if (handled) {
                    mChildAcceptsDrag = TRUE;
                }
            }
        }

        // Return HANDLED if one of our children can accept the drag
        if (mChildAcceptsDrag) {
            retval = TRUE;
        }
    } break;

    case IDragEvent::ACTION_DRAG_ENDED: {
        // Release the bookkeeping now that the drag lifecycle has ended
        if (!mDragNotifiedChildren.IsEmpty()) {
            List<AutoPtr<IView> >::Iterator iter = mDragNotifiedChildren.Begin();
            for (; iter != mDragNotifiedChildren.End(); ++iter) {
                View* child = VIEW_PROBE(iter->Get());
                assert(child != NULL);
                // If a child was notified about an ongoing drag, it's told that it's over
                Boolean isDispatchDragEvent;
                child->DispatchDragEvent(event, &isDispatchDragEvent);
                child->mPrivateFlags2 &= ~DRAG_MASK;
                child->RefreshDrawableState();
            }

            mDragNotifiedChildren.Clear();
            if (mCurrentDrag != NULL) {
                mCurrentDrag->Recycle();
                mCurrentDrag = NULL;
            }
        }

        // We consider drag-ended to have been handled if one of our children
        // had offered to handle the drag.
        if (mChildAcceptsDrag) {
            retval = TRUE;
        }
    } break;

    case IDragEvent::ACTION_DRAG_LOCATION: {
        // Find the [possibly new] drag target
        View* target = VIEW_PROBE(FindFrontmostDroppableChildAt(tx, ty, localPoint));

        // If we've changed apparent drag target, tell the view root which view
        // we're over now [for purposes of the eventual drag-recipient-changed
        // notifications to the framework] and tell the new target that the drag
        // has entered its bounds.  The root will see setDragFocus() calls all
        // the way down to the final leaf view that is handling the LOCATION event
        // before reporting the new potential recipient to the framework.
        if (mCurrentDragView != target) {
            root->SetDragFocus(IView::Probe(target));

            // If we've dragged off of a child view, send it the EXITED message
            if (mCurrentDragView != NULL) {
                View* view = VIEW_PROBE(mCurrentDragView);
                event->SetAction(IDragEvent::ACTION_DRAG_EXITED);
                Boolean isDispatchDragEvent;
                view->DispatchDragEvent(event, &isDispatchDragEvent);
                view->mPrivateFlags2 &= ~PFLAG2_DRAG_HOVERED;
                view->RefreshDrawableState();
            }
            mCurrentDragView = target;

            // If we've dragged over a new child view, send it the ENTERED message
            if (target != NULL) {
                event->SetAction(IDragEvent::ACTION_DRAG_ENTERED);
                Boolean isDispatchDragEvent;
                target->DispatchDragEvent(event, &isDispatchDragEvent);
                target->mPrivateFlags2 |= PFLAG2_DRAG_HOVERED;
                target->RefreshDrawableState();
            }
            event->SetAction(action);  // restore the event's original state
        }

        // Dispatch the actual drag location notice, localized into its coordinates
        if (target != NULL) {
            Float tmp = 0.0f;
            localPoint->GetX(&tmp);
            event->SetX(tmp);
            localPoint->GetY(&tmp);
            event->SetY(tmp);
            target->DispatchDragEvent(event, &retval);

            event->SetX(tx);
            event->SetY(ty);
        }
    } break;

    /* Entered / exited dispatch
     *
     * DRAG_ENTERED is not dispatched downwards from ViewGroup.  The reason for this is
     * that we're about to get the corresponding LOCATION event, which we will use to
     * determine which of our children is the new target; at that point we will
     * push a DRAG_ENTERED down to the new target child [which may itself be a ViewGroup].
     *
     * DRAG_EXITED *is* dispatched all the way down immediately: once we know the
     * drag has left this ViewGroup, we know by definition that every contained subview
     * is also no longer under the drag point.
     */

    case IDragEvent::ACTION_DRAG_EXITED: {
        if (mCurrentDragView != NULL) {
            Boolean isDispatchDragEvent;
            mCurrentDragView->DispatchDragEvent(event, &isDispatchDragEvent);
            mCurrentDragView->mPrivateFlags2 &= ~PFLAG2_DRAG_HOVERED;
            mCurrentDragView->RefreshDrawableState();

            mCurrentDragView = NULL;
        }
    } break;

    case IDragEvent::ACTION_DROP: {
        // if (ViewDebug.DEBUG_DRAG) Log.d(TAG, "Drop event: " + event);
        View* target = VIEW_PROBE(FindFrontmostDroppableChildAt(tx, ty, localPoint));
        if (target != NULL) {
            Float tmp = 0.0f;
            localPoint->GetX(&tmp);
            event->SetX(tmp);
            localPoint->GetY(&tmp);
            event->SetY(tmp);

            target->DispatchDragEvent(event, &retval);

            event->SetX(tx);
            event->SetY(ty);

        }
        else {
            // if (ViewDebug.DEBUG_DRAG) {
            //     Log.d(TAG, "   not dropped on an accepting view");
            // }
        }
    } break;
    }

    // If none of our children could handle the event, try here
    if (!retval) {
        // Call up to the View implementation that dispatches to installed listeners
        View::DispatchDragEvent(event, &retval);
    }

    *res = retval;
    return NOERROR;
}

AutoPtr<IView> ViewGroup::FindFrontmostDroppableChildAt(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPointF* outLocalPoint)
{
    for (Int32 i = mChildrenCount - 1; i >= 0; i--) {
        AutoPtr<IView> child = (*mChildren)[i];
        if (!((View*)child.Get())->CanAcceptDrag()) {
            continue;
        }

        if (IsTransformedTouchPointInView(x, y, child, outLocalPoint)) {
            return child;
        }
    }
    return NULL;
}

Boolean ViewGroup::NotifyChildOfDrag(
    /* [in] */ IView* child)
{
    // if (ViewDebug.DEBUG_DRAG) {
    //     Log.d(View::TAG, "Sending drag-started to view: " + child);
    // }

    assert(child != NULL);
    Boolean canAccept = FALSE;
    AutoPtr<IView> temp = child;
    List<AutoPtr<IView> >::Iterator iter =
        Find(mDragNotifiedChildren.Begin(), mDragNotifiedChildren.End(), temp);

    if (iter == mDragNotifiedChildren.End()) {
        mDragNotifiedChildren.PushBack(child);
        View* _child = ((View*)child);
         _child->DispatchDragEvent(mCurrentDrag, &canAccept);
        if (canAccept && !_child->CanAcceptDrag()) {
            _child->mPrivateFlags2 |= PFLAG2_DRAG_CAN_ACCEPT;
            _child->RefreshDrawableState();
        }
    }

    return canAccept;
}

ECode ViewGroup::DispatchWindowSystemUiVisiblityChanged(
    /* [in] */ Int32 visible)
{
    View::DispatchWindowSystemUiVisiblityChanged(visible);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->DispatchWindowSystemUiVisiblityChanged(visible);
    }

    return NOERROR;
}

ECode ViewGroup::DispatchSystemUiVisibilityChanged(
    /* [in] */ Int32 visible)
{
    View::DispatchSystemUiVisibilityChanged(visible);

    for (Int32 i=0; i <mChildrenCount; i++) {
        (*mChildren)[i]->DispatchSystemUiVisibilityChanged(visible);
    }

    return NOERROR;
}

Boolean ViewGroup::UpdateLocalSystemUiVisibility(
    /* [in] */ Int32 localValue,
    /* [in] */ Int32 localChanges)
{
    Boolean changed = View::UpdateLocalSystemUiVisibility(localValue, localChanges);

    for (Int32 i=0; i <mChildrenCount; i++) {
        View* child = (View*)(*mChildren)[i];
        changed |= child->UpdateLocalSystemUiVisibility(localValue, localChanges);
    }

    return changed;
}

ECode ViewGroup::DispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if ((mPrivateFlags & (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS)) == (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS)) {
        View::DispatchKeyEventPreIme(event, res);
        return NOERROR;
    }
    else if (mFocused != NULL) {
        View* focused = VIEW_PROBE(mFocused);
        assert(focused);
        if ((focused->mPrivateFlags & PFLAG_HAS_BOUNDS) == PFLAG_HAS_BOUNDS) {
            focused->DispatchKeyEventPreIme(event, res);
            return NOERROR;
        }
    }

    *res = FALSE;
    return NOERROR;
}

ECode ViewGroup::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnKeyEvent(event, 1);
    }

    if ((mPrivateFlags & (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS))
            == (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS)) {
        Boolean isDispatchDragEvent;
        if (View::DispatchKeyEvent(event, &isDispatchDragEvent), isDispatchDragEvent) {
            *res = TRUE;
            return NOERROR;
        }
    }
    else if (mFocused != NULL && (VIEW_PROBE(mFocused)->mPrivateFlags & PFLAG_HAS_BOUNDS)
            == PFLAG_HAS_BOUNDS) {
        Boolean isDispatchDragEvent;
        VIEW_PROBE(mFocused)->DispatchKeyEvent(event, &isDispatchDragEvent);
        if (isDispatchDragEvent) {
            *res = TRUE;
            return NOERROR;
        }
    }

    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnUnhandledEvent(IInputEvent::Probe(event), 1);
    }

    *res = FALSE;
    return NOERROR;
}

ECode ViewGroup::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if ((mPrivateFlags & (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS)) == (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS)) {
        View::DispatchKeyShortcutEvent(event, res);
        return NOERROR;
    }
    else if (mFocused != NULL) {
        View* focused = VIEW_PROBE(mFocused);
        assert(focused);
        if ((focused->mPrivateFlags & PFLAG_HAS_BOUNDS) == PFLAG_HAS_BOUNDS) {
            focused->DispatchKeyShortcutEvent(event, res);
            return NOERROR;
        }
    }

    *res = FALSE;
    return NOERROR;
}

ECode ViewGroup::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnTrackballEvent(event, 1);
    }
    Boolean trackballEvent;
    if ((mPrivateFlags & (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS))
            == (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS)) {

        if (View::DispatchTrackballEvent(event, &trackballEvent), trackballEvent) {
            *res = TRUE;
            return NOERROR;
        }
    }
    else if (mFocused != NULL && (VIEW_PROBE(mFocused)->mPrivateFlags & PFLAG_HAS_BOUNDS)
            == PFLAG_HAS_BOUNDS) {

        if (VIEW_PROBE(mFocused)->DispatchTrackballEvent(event, &trackballEvent), trackballEvent) {
            *res = TRUE;
            return NOERROR;
        }
    }

    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnUnhandledEvent(IInputEvent::Probe(event), 1);
    }

    *res = FALSE;
    return NOERROR;
}

Boolean ViewGroup::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 action = 0;
    event->GetAction(&action);

    // First check whether the view group wants to intercept the hover event.
    Boolean interceptHover;
    OnInterceptHoverEvent(event, &interceptHover);
    event->SetAction(action); // restore action in case it was changed

    AutoPtr<IMotionEvent> eventNoHistory = event;
    Boolean handled = FALSE;

    // Send events to the hovered children and build a new list of hover targets until
    // one is found that handles the event.
    AutoPtr<HoverTarget> firstOldHoverTarget = mFirstHoverTarget;
    mFirstHoverTarget = NULL;
    if (!interceptHover && action != IMotionEvent::ACTION_HOVER_EXIT) {
        Float x = 0.0f;
        event->GetX(&x);
        Float y = 0.0f;
        event->GetY(&y);

        if (mChildrenCount != 0) {
            AutoPtr<IList> preorderedList = BuildOrderedChildList();
            Boolean customOrder = preorderedList == NULL && IsChildrenDrawingOrderEnabled();

            AutoPtr<HoverTarget> lastHoverTarget;
            for (Int32 i = mChildrenCount - 1; i >= 0; i--) {
                Int32 childIndex = customOrder ? GetChildDrawingOrder(mChildrenCount, i) : i;
                AutoPtr<IView> child;
                if (preorderedList == NULL) {
                    child = (*mChildren)[childIndex];
                } else {
                    AutoPtr<IInterface> temp;
                    preorderedList->Get(childIndex, (IInterface**)&temp);
                    child = IView::Probe(temp);
                }

                if (!CanViewReceivePointerEvents(child)
                        || !IsTransformedTouchPointInView(x, y, child, NULL)) {
                    continue;
                }

                // Obtain a hover target for this child.  Dequeue it from the
                // old hover target list if the child was previously hovered.
                AutoPtr<HoverTarget> hoverTarget = firstOldHoverTarget;
                Boolean wasHovered = FALSE;
                for (AutoPtr<HoverTarget> predecessor; ;) {
                    if (hoverTarget == NULL) {
                        hoverTarget = HoverTarget::Obtain(child);
                        wasHovered = FALSE;
                        break;
                    }

                    if (hoverTarget->mChild == child) {
                        if (predecessor != NULL) {
                            predecessor->mNext = hoverTarget->mNext;
                        }
                        else {
                            firstOldHoverTarget = hoverTarget->mNext;
                        }
                        hoverTarget->mNext = NULL;
                        wasHovered = TRUE;
                        break;
                    }

                    predecessor = hoverTarget;
                    hoverTarget = hoverTarget->mNext;
                }

                // Enqueue the hover target onto the new hover target list.
                if (lastHoverTarget != NULL) {
                    lastHoverTarget->mNext = hoverTarget;
                }
                else {
                    mFirstHoverTarget = hoverTarget;
                }
                lastHoverTarget = hoverTarget;

                // Dispatch the event to the child.
                if (action == IMotionEvent::ACTION_HOVER_ENTER) {
                    if (!wasHovered) {
                        // Send the enter as is.
                        handled |= DispatchTransformedGenericPointerEvent(
                                event, child); // enter
                    }
                }
                else if (action == IMotionEvent::ACTION_HOVER_MOVE) {
                    if (!wasHovered) {
                        // Synthesize an enter from a move.
                        eventNoHistory = ObtainMotionEventNoHistoryOrSelf(eventNoHistory);
                        eventNoHistory->SetAction(IMotionEvent::ACTION_HOVER_ENTER);
                        handled |= DispatchTransformedGenericPointerEvent(
                                eventNoHistory, child); // enter
                        eventNoHistory->SetAction(action);

                        handled |= DispatchTransformedGenericPointerEvent(
                                eventNoHistory, child); // move
                    }
                    else {
                        // Send the move as is.
                        handled |= DispatchTransformedGenericPointerEvent(event, child);
                    }
                }
                if (handled) {
                    break;
                }
            }
            if (preorderedList != NULL) preorderedList->Clear();
        }
    }

    // Send exit events to all previously hovered children that are no longer hovered.
    while (firstOldHoverTarget != NULL) {
        AutoPtr<IView> child = firstOldHoverTarget->mChild;

        // Exit the old hovered child.
        if (action == IMotionEvent::ACTION_HOVER_EXIT) {
            // Send the exit as is.
            handled |= DispatchTransformedGenericPointerEvent(
                    event, child); // exit
        }
        else {
            // Synthesize an exit from a move or enter.
            // Ignore the result because hover focus has moved to a different view.
            if (action == IMotionEvent::ACTION_HOVER_MOVE) {
                DispatchTransformedGenericPointerEvent(
                        event, child); // move
            }
            eventNoHistory = ObtainMotionEventNoHistoryOrSelf(eventNoHistory);
            eventNoHistory->SetAction(IMotionEvent::ACTION_HOVER_EXIT);
            DispatchTransformedGenericPointerEvent(
                    eventNoHistory, child); // exit
            eventNoHistory->SetAction(action);
        }

        AutoPtr<HoverTarget> nextOldHoverTarget = firstOldHoverTarget->mNext;
        firstOldHoverTarget->Recycle();
        firstOldHoverTarget = nextOldHoverTarget;
    }

    // Send events to the view group itself if no children have handled it.
    Boolean newHoveredSelf = !handled;
    if (newHoveredSelf == mHoveredSelf) {
        if (newHoveredSelf) {
            // Send event to the view group as before.
            handled |= View::DispatchHoverEvent(event);
        }
    }
    else {
        if (mHoveredSelf) {
            // Exit the view group.
            if (action == IMotionEvent::ACTION_HOVER_EXIT) {
                // Send the exit as is.
                handled |= View::DispatchHoverEvent(event); // exit
            }
            else {
                // Synthesize an exit from a move or enter.
                // Ignore the result because hover focus is moving to a different view.
                if (action == IMotionEvent::ACTION_HOVER_MOVE) {
                    View::DispatchHoverEvent(event); // move
                }
                eventNoHistory = ObtainMotionEventNoHistoryOrSelf(eventNoHistory);
                eventNoHistory->SetAction(IMotionEvent::ACTION_HOVER_EXIT);
                View::DispatchHoverEvent(eventNoHistory); // exit
                eventNoHistory->SetAction(action);
            }
            mHoveredSelf = FALSE;
        }

        if (newHoveredSelf) {
            // Enter the view group.
            if (action == IMotionEvent::ACTION_HOVER_ENTER) {
                // Send the enter as is.
                handled |= View::DispatchHoverEvent(event); // enter
                mHoveredSelf = TRUE;
            }
            else if (action == IMotionEvent::ACTION_HOVER_MOVE) {
                // Synthesize an enter from a move.
                eventNoHistory = ObtainMotionEventNoHistoryOrSelf(eventNoHistory);
                eventNoHistory->SetAction(IMotionEvent::ACTION_HOVER_ENTER);
                handled |= View::DispatchHoverEvent(eventNoHistory); // enter
                eventNoHistory->SetAction(action);

                handled |= View::DispatchHoverEvent(eventNoHistory); // move
                mHoveredSelf = TRUE;
            }
        }
    }

    // Recycle the copy of the event that we made.
    if (eventNoHistory.Get() != event) {
        (IInputEvent::Probe(eventNoHistory))->Recycle();
    }

    // Done.
    return handled;
}

void ViewGroup::ExitHoverTargets()
{
    if (mHoveredSelf || mFirstHoverTarget != NULL) {
        Int64 now = SystemClock::GetUptimeMillis();
        AutoPtr<IMotionEvent> event;
        CMotionEvent::Obtain(
            now, now, IMotionEvent::ACTION_HOVER_EXIT,
            0.0f, 0.0f, 0, (IMotionEvent**)&event);
        IInputEvent::Probe(event)->SetSource(IInputDevice::SOURCE_TOUCHSCREEN);
        DispatchHoverEvent(event);
        IInputEvent::Probe(event)->Recycle();
    }
}

void ViewGroup::CancelHoverTarget(
    /* [in] */ IView* view)
{
    AutoPtr<HoverTarget> predecessor;
    AutoPtr<HoverTarget> target = mFirstHoverTarget;
    while (target != NULL) {
        AutoPtr<HoverTarget> next = target->mNext;
        if (target->mChild.Get() == view) {
            if (predecessor == NULL) {
                mFirstHoverTarget = next;
            }
            else {
                predecessor->mNext = next;
            }

            target->Recycle();

            Int64 now = SystemClock::GetUptimeMillis();
            AutoPtr<IMotionEvent> event;
            CMotionEvent::Obtain(
                now, now, IMotionEvent::ACTION_HOVER_EXIT,
                0.0f, 0.0f, 0, (IMotionEvent**)&event);
            IInputEvent::Probe(event)->SetSource(IInputDevice::SOURCE_TOUCHSCREEN);
            (VIEW_PROBE(view))->DispatchHoverEvent(event);
            IInputEvent::Probe(event)->Recycle();
            return;
        }

        predecessor = target;
        target = next;
    }
}

Boolean ViewGroup::HasHoveredChild()
{
    return mFirstHoverTarget != NULL;
}

ECode ViewGroup::AddChildrenForAccessibility(
    /* [in] */ IArrayList* childrenForAccessibility)
{
    AutoPtr<ChildListForAccessibility> children =
        ChildListForAccessibility::Obtain(this, TRUE);
    const Int32 childrenCount = children->GetChildCount();
    for (Int32 i = 0; i < childrenCount; i++) {
        View* child = VIEW_PROBE(children->GetChildAt(i));
        if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
            Boolean includeForAccessibility;
            if (child->IncludeForAccessibility(&includeForAccessibility), includeForAccessibility) {
                childrenForAccessibility->Add(IView::Probe(child));
            }
            else {
                child->AddChildrenForAccessibility(childrenForAccessibility);
            }
        }
    }
    children->Recycle();

    return NOERROR;
}

ECode ViewGroup::OnInterceptHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

AutoPtr<IMotionEvent> ViewGroup::ObtainMotionEventNoHistoryOrSelf(
    /* [in] */ IMotionEvent* event)
{
    Int32 size = 0;
    event->GetHistorySize(&size);
    if (size == 0) {
        return event;
    }
    AutoPtr<IMotionEvent> motionEvent;
    CMotionEvent::ObtainNoHistory((CMotionEvent*)event, (IMotionEvent**)&motionEvent);

    return motionEvent;
}

Boolean ViewGroup::DispatchGenericPointerEvent(
    /* [in] */ IMotionEvent* event)
{
    // Send the event to the child under the pointer.
    if (mChildrenCount != 0) {
        Float x = 0.0f;
        event->GetX(&x);
        Float y = 0.0f;
        event->GetY(&y);

        AutoPtr<IList> preorderedList = BuildOrderedChildList();
        const Boolean customOrder = preorderedList == NULL && IsChildrenDrawingOrderEnabled();

        for (Int32 i = mChildrenCount - 1; i >= 0; i--) {
            Int32 childIndex = customOrder ? GetChildDrawingOrder(mChildrenCount, i) : i;
            AutoPtr<IView> child;
            if (preorderedList == NULL) {
                child = (*mChildren)[childIndex];
            } else {
                AutoPtr<IInterface> temp;
                preorderedList->Get(childIndex, (IInterface**)&temp);
                child = IView::Probe(temp);
            }
            if (!CanViewReceivePointerEvents(child)
                    || !IsTransformedTouchPointInView(x, y, child, NULL)) {
                continue;
            }

            if (DispatchTransformedGenericPointerEvent(event, child)) {
                if (preorderedList != NULL) preorderedList->Clear();
                return TRUE;
            }
        }
        if (preorderedList != NULL) preorderedList->Clear();
    }

    // No child handled the event.  Send it to this view group.
    return View::DispatchGenericPointerEvent(event);
}

Boolean ViewGroup::DispatchGenericFocusedEvent(
    /* [in] */ IMotionEvent* event)
{
    // Send the event to the focused child or to this view group if it has focus.
    if ((mPrivateFlags & (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS))
            == (PFLAG_FOCUSED | PFLAG_HAS_BOUNDS)) {
        return View::DispatchGenericFocusedEvent(event);
    }
    else if (mFocused != NULL && ((VIEW_PROBE(mFocused))->mPrivateFlags & PFLAG_HAS_BOUNDS)
            == PFLAG_HAS_BOUNDS) {
        Boolean res;
        (VIEW_PROBE(mFocused))->DispatchGenericMotionEvent(event, &res);
        return res;
    }
    return FALSE;
}

Boolean ViewGroup::DispatchTransformedGenericPointerEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IView* _child)
{
    View* child = VIEW_PROBE(_child);
    Float offsetX = mScrollX - child->mLeft;
    Float offsetY = mScrollY - child->mTop;

    Boolean handled = FALSE;
    if (!child->HasIdentityMatrix()) {
        AutoPtr<IMotionEvent> transformedEvent;
        CMotionEvent::Obtain(event, (IMotionEvent**)&transformedEvent);
        transformedEvent->OffsetLocation(offsetX, offsetY);

        AutoPtr<IMatrix> matrix;
        child->GetInverseMatrix((IMatrix**)&matrix);
        transformedEvent->Transform(matrix);
        child->DispatchGenericMotionEvent(transformedEvent, &handled);
        IInputEvent::Probe(transformedEvent)->Recycle();
    }
    else {
        event->OffsetLocation(offsetX, offsetY);
        child->DispatchGenericMotionEvent(event, &handled);
        event->OffsetLocation(-offsetX, -offsetY);
    }

    return handled;
}

ECode ViewGroup::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnTouchEvent(ev, 1);
    }

    Boolean handled = FALSE, bval;
    if (OnFilterTouchEventForSecurity(ev, &bval), bval) {
        Int32 action = 0;
        ev->GetAction(&action);
        Int32 actionMasked = action & IMotionEvent::ACTION_MASK;
        // Handle an initial down.
        if (actionMasked == IMotionEvent::ACTION_DOWN) {
            // Throw away all previous state when starting a new touch gesture.
            // The framework may have dropped the up or cancel event for the previous gesture
            // due to an app switch, ANR, or some other state change.
            CancelAndClearTouchTargets(ev);
            ResetTouchState();
        }

        // Check for interception.
        Boolean intercepted = FALSE;
        if (actionMasked == IMotionEvent::ACTION_DOWN
                || mFirstTouchTarget != NULL) {
            Boolean disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;
            if (!disallowIntercept) {
                OnInterceptTouchEvent(ev, &intercepted);
                ev->SetAction(action); // restore action in case it was changed
            }
            else {
                intercepted = FALSE;
            }
        }
        else {
            // There are no touch targets and this action is not an initial down
            // so this view group continues to intercept touches.
            intercepted = TRUE;
        }

        // Check for cancelation.
        Boolean canceled = ResetCancelNextUpFlag(this)
                || actionMasked == IMotionEvent::ACTION_CANCEL;
        // Update list of touch targets for pointer down, if needed.
        Boolean split = (mGroupFlags & FLAG_SPLIT_MOTION_EVENTS) != 0;
        AutoPtr<TouchTarget> newTouchTarget;
        Boolean alreadyDispatchedToNewTouchTarget = FALSE;
        if (!canceled && !intercepted) {
            if (actionMasked == IMotionEvent::ACTION_DOWN
                    || (split && actionMasked == IMotionEvent::ACTION_POINTER_DOWN)
                    || actionMasked == IMotionEvent::ACTION_HOVER_MOVE) {
                Int32 actionIndex = 0;
                ev->GetActionIndex(&actionIndex); // always 0 for down

                Int32 idBitsToAssign = TouchTarget::ALL_POINTER_IDS;
                if (split) {
                    Int32 tmp = 0;
                    ev->GetPointerId(actionIndex, &tmp);
                    idBitsToAssign = 1 << tmp;
                }

                // Clean up earlier touch targets for this pointer id in case they
                // have become out of sync.
                RemovePointersFromTouchTargets(idBitsToAssign);

                const Int32 childrenCount = mChildrenCount;
                if (newTouchTarget == NULL && childrenCount != 0) {
                    // Find a child that can receive the event.
                    // Scan children from front to back.
                    Float x, y;
                    ev->GetX(actionIndex, &x);
                    ev->GetY(actionIndex, &y);

                    AutoPtr<IList> preorderedList = BuildOrderedChildList();
                    Boolean customOrder = preorderedList == NULL && IsChildrenDrawingOrderEnabled();

                    for (Int32 i = childrenCount - 1; i >= 0; i--) {
                        Int32 childIndex = customOrder ?
                                GetChildDrawingOrder(childrenCount, i) : i;
                        AutoPtr<IView> child;
                        if (preorderedList == NULL) {
                            child = (*mChildren)[childIndex];
                        } else {
                            AutoPtr<IInterface> temp;
                            preorderedList->Get(childIndex, (IInterface**)&temp);
                            child = IView::Probe(temp);
                        }
                        if (!CanViewReceivePointerEvents(child)
                                || !IsTransformedTouchPointInView(x, y, child, NULL)) {
                            continue;
                        }

                        newTouchTarget = GetTouchTarget(child);
                        if (newTouchTarget != NULL) {
                            // Child is already receiving touch within its bounds.
                            // Give it the new pointer in addition to the ones it is handling.
                            newTouchTarget->mPointerIdBits |= idBitsToAssign;
                            break;
                        }

                        ResetCancelNextUpFlag(child);
                        if (DispatchTransformedTouchEvent(ev, FALSE, child, idBitsToAssign)) {
                            // Child wants to receive touch within its bounds.
                            ev->GetDownTime(&mLastTouchDownTime);

                            if (preorderedList != NULL) {
                                // childIndex points into presorted list, find original index
                                for (Int32 j = 0; j < childrenCount; j++) {
                                    if ((*mChildren)[childIndex] == (*mChildren)[j]) {
                                        mLastTouchDownIndex = j;
                                        break;
                                    }
                                }
                            }
                            else {
                                mLastTouchDownIndex = childIndex;
                            }
                            ev->GetX(&mLastTouchDownX);
                            ev->GetY(&mLastTouchDownY);
                            newTouchTarget = AddTouchTarget(child, idBitsToAssign);
                            alreadyDispatchedToNewTouchTarget = TRUE;
                            break;
                        }
                    }
                    if (preorderedList != NULL) preorderedList->Clear();
                }

                if (newTouchTarget == NULL && mFirstTouchTarget != NULL) {
                    // Did not find a child to receive the event.
                    // Assign the pointer to the least recently added target.
                    newTouchTarget = mFirstTouchTarget;
                    while (newTouchTarget->mNext != NULL) {
                        newTouchTarget = newTouchTarget->mNext;
                    }
                    newTouchTarget->mPointerIdBits |= idBitsToAssign;
                }
            }
        }

        // Dispatch to touch targets.
        if (mFirstTouchTarget == NULL) {
            // No touch targets so treat this as an ordinary view.
            handled = DispatchTransformedTouchEvent(ev, canceled, NULL,
                    TouchTarget::ALL_POINTER_IDS);
        }
        else {
            // Dispatch to touch targets, excluding the new touch target if we already
            // dispatched to it.  Cancel touch targets if necessary.
            AutoPtr<TouchTarget> predecessor;
            AutoPtr<TouchTarget> target = mFirstTouchTarget;
            while (target != NULL) {
                AutoPtr<TouchTarget> next = target->mNext;
                if (alreadyDispatchedToNewTouchTarget && target == newTouchTarget) {
                    handled = TRUE;
                }
                else {
                    Boolean cancelChild = ResetCancelNextUpFlag(target->mChild) || intercepted;
                    if (DispatchTransformedTouchEvent(ev, cancelChild,
                            target->mChild, target->mPointerIdBits)) {
                        handled = TRUE;
                    }
                    if (cancelChild) {
                        if (predecessor == NULL) {
                            mFirstTouchTarget = next;
                        }
                        else {
                            predecessor->mNext = next;
                        }
                        target->Recycle();
                        target = next;
                        continue;
                    }
                }
                predecessor = target;
                target = next;
            }
        }

        // Update list of touch targets for pointer up or cancel, if needed.
        if (canceled
                || actionMasked == IMotionEvent::ACTION_UP
                || actionMasked == IMotionEvent::ACTION_HOVER_MOVE) {
            ResetTouchState();
        }
        else if (split && actionMasked == IMotionEvent::ACTION_POINTER_UP) {
            Int32 actionIndex = 0;
            ev->GetActionIndex(&actionIndex);

            Int32 idBitsToRemove = 0, tmp = 0;
            ev->GetPointerId(actionIndex, &tmp);
            idBitsToRemove = 1 << tmp;
            RemovePointersFromTouchTargets(idBitsToRemove);
        }
    }
    if (!handled && mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnUnhandledEvent(IInputEvent::Probe(ev), 1);
    }
    *res = handled;
    return NOERROR;
}

void ViewGroup::ResetTouchState()
{
    ClearTouchTargets();
    ResetCancelNextUpFlag(this);
    mGroupFlags &= ~FLAG_DISALLOW_INTERCEPT;
    mNestedScrollAxes = IView::SCROLL_AXIS_NONE;
}

Boolean ViewGroup::ResetCancelNextUpFlag(
    /* [in] */ IView* inView)
{
    View* view = (View*)inView;
    if ((view->mPrivateFlags & PFLAG_CANCEL_NEXT_UP_EVENT) != 0) {
        view->mPrivateFlags &= ~PFLAG_CANCEL_NEXT_UP_EVENT;
        return TRUE;
    }
    return FALSE;
}

void ViewGroup::ClearTouchTargets()
{
    AutoPtr<TouchTarget> target = mFirstTouchTarget;
    if (target != NULL) {
        do {
            AutoPtr<TouchTarget> next = target->mNext;
            target->Recycle();
            target = next;
        } while (target != NULL);
        mFirstTouchTarget = NULL;
    }
}

void ViewGroup::CancelAndClearTouchTargets(
    /* [in] */IMotionEvent* event)
{
    if (mFirstTouchTarget != NULL) {
        Boolean syntheticEvent = FALSE;
        AutoPtr<IMotionEvent> temp = event;
        if (temp == NULL) {
            Int64 now = SystemClock::GetUptimeMillis();
            CMotionEvent::Obtain(
                now, now, IMotionEvent::ACTION_CANCEL,
                0.0f, 0.0f, 0, (IMotionEvent**)&temp);
            IInputEvent::Probe(temp)->SetSource(IInputDevice::SOURCE_TOUCHSCREEN);
            syntheticEvent = TRUE;
        }

        AutoPtr<TouchTarget> target = mFirstTouchTarget;
        for (; target != NULL; target = target->mNext) {
            ResetCancelNextUpFlag(target->mChild);
            DispatchTransformedTouchEvent(temp, TRUE, target->mChild, target->mPointerIdBits);
        }
        ClearTouchTargets();

        if (syntheticEvent) {
            if (temp) (IInputEvent::Probe(temp))->Recycle();
        }
    }
}

AutoPtr<ViewGroup::TouchTarget> ViewGroup::GetTouchTarget(
    /* [in] */ IView* child)
{
    for (AutoPtr<TouchTarget> target = mFirstTouchTarget; target != NULL; target = target->mNext) {
        if (target->mChild.Get() == child) {
            return target;
        }
    }
    return NULL;
}

AutoPtr<ViewGroup::TouchTarget> ViewGroup::AddTouchTarget(
    /* [in] */ IView* child,
    /* [in] */ Int32 pointerIdBits)
{
    AutoPtr<TouchTarget> target = TouchTarget::Obtain(child, pointerIdBits);
    target->mNext = mFirstTouchTarget;
    mFirstTouchTarget = target;
    return target;
}

void ViewGroup::RemovePointersFromTouchTargets(
    /* [in] */ Int32 pointerIdBits)
{
    AutoPtr<TouchTarget> predecessor;
    AutoPtr<TouchTarget> target = mFirstTouchTarget;
    while (target != NULL) {
        AutoPtr<TouchTarget> next = target->mNext;
        if ((target->mPointerIdBits & pointerIdBits) != 0) {
            target->mPointerIdBits &= ~pointerIdBits;
            if (target->mPointerIdBits == 0) {
                if (predecessor == NULL) {
                    mFirstTouchTarget = next;
                }
                else {
                    predecessor->mNext = next;
                }
                target->Recycle();
                target = next;
                continue;
            }
        }
        predecessor = target;
        target = next;
    }
}

void ViewGroup::CancelTouchTarget(
    /* [in] */ IView* view)
{
    AutoPtr<TouchTarget> predecessor;
    AutoPtr<TouchTarget> target = mFirstTouchTarget;
    while (target != NULL) {
        AutoPtr<TouchTarget> next = target->mNext;
        if (target->mChild.Get() == view) {
            if (predecessor == NULL) {
                mFirstTouchTarget = next;
            }
            else {
                predecessor->mNext = next;
            }
            target->Recycle();

            Int64 now = SystemClock::GetUptimeMillis();
            AutoPtr<IMotionEvent> event;
            CMotionEvent::Obtain(
                now, now, IMotionEvent::ACTION_CANCEL,
                0.0f, 0.0f, 0, (IMotionEvent**)&event);
            IInputEvent::Probe(event)->SetSource(IInputDevice::SOURCE_TOUCHSCREEN);
            Boolean res;
            view->DispatchTouchEvent(event, &res);
            IInputEvent::Probe(event)->Recycle();
            return;
        }
        predecessor = target;
        target = next;
    }
}

Boolean ViewGroup::CanViewReceivePointerEvents(
    /* [in] */ IView* child)
{
    View* _child = VIEW_PROBE(child);
    assert(_child != NULL);
    AutoPtr<IAnimation> animation;
    _child->GetAnimation((IAnimation**)&animation);
    return (_child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE || animation != NULL;
}

Boolean ViewGroup::IsTransformedTouchPointInView(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IView* child,
    /* [in] */ IPointF* outLocalPoint)
{
    View* _child = VIEW_PROBE(child);
    Float localX = x + mScrollX - _child->mLeft;
    Float localY = y + mScrollY - _child->mTop;
    if (! _child->HasIdentityMatrix() && mAttachInfo != NULL) {
        AutoPtr<ArrayOf<Float> > locations = ArrayOf<Float>::Alloc(2);
        (*locations)[0] = localX;
        (*locations)[1] = localY;
        AutoPtr<IMatrix> temp;
        _child->GetInverseMatrix((IMatrix**)&temp);
        temp->MapPoints(locations);
        localX = (*locations)[0];
        localY = (*locations)[1];
        (*mAttachInfo->mTmpTransformLocation)[0] = localX;
        (*mAttachInfo->mTmpTransformLocation)[1] = localY;
    }
    Boolean isInView = _child->PointInView(localX, localY);
    if (isInView && outLocalPoint != NULL) {
        outLocalPoint->Set(localX, localY);
    }
    return isInView;
}

Boolean ViewGroup::DispatchTransformedTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Boolean cancel,
    /* [in] */ IView* child,
    /* [in] */ Int32 desiredPointerIdBits)
{
    Boolean handled = FALSE;
    View* _child = (View*)child;

    // Canceling motions is a special case.  We don't need to perform any transformations
    // or filtering.  The important part is the action, not the contents.
    Int32 oldAction = 0;
    event->GetAction(&oldAction);
    if (cancel || oldAction == IMotionEvent::ACTION_CANCEL) {
        event->SetAction(IMotionEvent::ACTION_CANCEL);
        if (_child == NULL) {
            View::DispatchTouchEvent(event, &handled);
        }
        else {
            _child->DispatchTouchEvent(event, &handled);
        }
        event->SetAction(oldAction);
        return handled;
    }

    // Calculate the number of pointers to deliver.
    Int32 oldPointerIdBits = 0;
    event->GetPointerIdBits(&oldPointerIdBits);
    Int32 newPointerIdBits = oldPointerIdBits & desiredPointerIdBits;

    // If for some reason we ended up in an inconsistent state where it looks like we
    // might produce a motion event with no pointers in it, then drop the event.
    if (newPointerIdBits == 0) {
        return FALSE;
    }

    // If the number of pointers is the same and we don't need to perform any fancy
    // irreversible transformations, then we can reuse the motion event for this
    // dispatch as long as we are careful to revert any changes we make.
    // Otherwise we need to make a copy.
    AutoPtr<IMotionEvent> transformedEvent;
    if (newPointerIdBits == oldPointerIdBits) {
        if (_child == NULL || _child->HasIdentityMatrix()) {
            if (_child == NULL) {
                View::DispatchTouchEvent(event, &handled);
            }
            else {
                Float offsetX = mScrollX - _child->mLeft;
                Float offsetY = mScrollY - _child->mTop;
                event->OffsetLocation(offsetX, offsetY);

                _child->DispatchTouchEvent(event, &handled);

                event->OffsetLocation(-offsetX, -offsetY);
            }
            return handled;
        }
        CMotionEvent::Obtain(event, (IMotionEvent**)&transformedEvent);
    }
    else {
        event->Split(newPointerIdBits, (IMotionEvent**)&transformedEvent);
    }

    // Perform any necessary transformations and dispatch.
    if (_child == NULL) {
        View::DispatchTouchEvent(transformedEvent, &handled);
    }
    else {
        Float offsetX = mScrollX - _child->mLeft;
        Float offsetY = mScrollY - _child->mTop;
        transformedEvent->OffsetLocation(offsetX, offsetY);
        if (!_child->HasIdentityMatrix()) {
            AutoPtr<IMatrix> temp;
            _child->GetInverseMatrix((IMatrix**)&temp);
            transformedEvent->Transform(temp);
        }

        _child->DispatchTouchEvent(transformedEvent, &handled);
    }

    // Done.
    IInputEvent::Probe(transformedEvent)->Recycle();
    return handled;
}

ECode ViewGroup::SetMotionEventSplittingEnabled(
    /* [in] */ Boolean split)
{
    // TODO Applications really shouldn't change this setting mid-touch event,
    // but perhaps this should handle that case and send ACTION_CANCELs to any child views
    // with gestures in progress when this is changed.
    if (split) {
        mGroupFlags |= FLAG_SPLIT_MOTION_EVENTS;
    }
    else {
        mGroupFlags &= ~FLAG_SPLIT_MOTION_EVENTS;
    }

    return NOERROR;
}

ECode ViewGroup::IsMotionEventSplittingEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = (mGroupFlags & FLAG_SPLIT_MOTION_EVENTS) == FLAG_SPLIT_MOTION_EVENTS;
    return NOERROR;
}

/**
 * Returns true if this ViewGroup should be considered as a single entity for removal
 * when executing an Activity transition. If this is false, child elements will move
 * individually during the transition.
 * @return True if the ViewGroup should be acted on together during an Activity transition.
 * The default value is false when the background is null and true when the background
 * is not null or if {@link #getTransitionName()} is not null.
 */
ECode ViewGroup::IsTransitionGroup(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if ((mGroupFlags & FLAG_IS_TRANSITION_GROUP_SET) != 0) {
        *res = ((mGroupFlags & FLAG_IS_TRANSITION_GROUP) != 0);
        return NOERROR;
    } else {
        AutoPtr<IDrawable> background;
        GetBackground((IDrawable**)&background);
        String str;
        GetTransitionName(&str);

        *res = (background != NULL || str.IsNullOrEmpty());
        return NOERROR;
    }
}

/**
 * Changes whether or not this ViewGroup should be treated as a single entity during
 * Activity Transitions.
 * @param isTransitionGroup Whether or not the ViewGroup should be treated as a unit
 *                          in Activity transitions. If false, the ViewGroup won't transition,
 *                          only its children. If true, the entire ViewGroup will transition
 *                          together.
 * @see android.app.ActivityOptions#makeSceneTransitionAnimation(android.app.Activity,
 * android.util.Pair[])
 */
ECode ViewGroup::SetTransitionGroup(
    /* [in] */ Boolean isTransitionGroup)
{
    mGroupFlags |= FLAG_IS_TRANSITION_GROUP_SET;
    if (isTransitionGroup) {
        mGroupFlags |= FLAG_IS_TRANSITION_GROUP;
    } else {
        mGroupFlags &= ~FLAG_IS_TRANSITION_GROUP;
    }
    return NOERROR;
}

ECode ViewGroup::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    if (disallowIntercept == ((mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0)) {
        // We're already in this state, assume our ancestors are too
        return NOERROR;
    }

    if (disallowIntercept) {
        mGroupFlags |= FLAG_DISALLOW_INTERCEPT;
    }
    else {
        mGroupFlags &= ~FLAG_DISALLOW_INTERCEPT;
    }

    // Pass it up to our parent
    if (mParent != NULL) {
        return mParent->RequestDisallowInterceptTouchEvent(disallowIntercept);
    }

    return NOERROR;
}

/**
 * Implement this method to intercept all touch screen motion events.  This
 * allows you to watch events as they are dispatched to your children, and
 * take ownership of the current gesture at any point.
 *
 * <p>Using this function takes some care, as it has a fairly complicated
 * interaction with {@link View#onTouchEvent(MotionEvent)
 * View.onTouchEvent(MotionEvent)}, and using it requires implementing
 * that method as well as this one in the correct way.  Events will be
 * received in the following order:
 *
 * <ol>
 * <li> You will receive the down event here.
 * <li> The down event will be handled either by a child of this view
 * group, or given to your own onTouchEvent() method to handle; this means
 * you should implement onTouchEvent() to return TRUE, so you will
 * continue to see the rest of the gesture (instead of looking for
 * a parent view to handle it).  Also, by returning TRUE from
 * onTouchEvent(), you will not receive any following
 * events in onInterceptTouchEvent() and all touch processing must
 * happen in onTouchEvent() like normal.
 * <li> For as long as you return FALSE from this function, each following
 * event (up to and including the final up) will be delivered first here
 * and then to the target's onTouchEvent().
 * <li> If you return TRUE from here, you will not receive any
 * following events: the target view will receive the same event but
 * with the action {@link MotionEvent#ACTION_CANCEL}, and all further
 * events will be delivered to your onTouchEvent() method and no longer
 * appear here.
 * </ol>
 *
 * @param ev The motion event being dispatched down the hierarchy.
 * @return Return TRUE to steal motion events from the children and have
 * them dispatched to this ViewGroup through onTouchEvent().
 * The current target will receive an ACTION_CANCEL event, and no further
 * messages will be delivered here.
 */
ECode ViewGroup::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

/**
 *
 * Looks for a view to give focus to respecting the setting specified by
 * {@link #getDescendantFocusability()}.
 *
 * Uses {@link #onRequestFocusInDescendants(Int32, android.graphics.Rect)} to
 * find focus within the children of this group when appropriate.
 *
 * @see #FOCUS_BEFORE_DESCENDANTS
 * @see #FOCUS_AFTER_DESCENDANTS
 * @see #FOCUS_BLOCK_DESCENDANTS
 * @see #onRequestFocusInDescendants
 */
ECode ViewGroup::RequestFocus(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (DBG) {
        Slogger::D(TAG,
            "0x%08x ViewGroup.requestFocus direction = %d", this, direction);
    }

    Int32 descendantFocusability = 0;
    GetDescendantFocusability(&descendantFocusability);

    switch (descendantFocusability) {
    case FOCUS_BLOCK_DESCENDANTS:
        View::RequestFocus(direction, previouslyFocusedRect, res);
        return NOERROR;
    case FOCUS_BEFORE_DESCENDANTS:
        {
            Boolean took;
            View::RequestFocus(direction, previouslyFocusedRect, &took);
            if (took) {
                *res = took;
            } else {
                *res = OnRequestFocusInDescendants(direction, previouslyFocusedRect);
            }
            return NOERROR;
        }
    case FOCUS_AFTER_DESCENDANTS:
        {
            Boolean took = OnRequestFocusInDescendants(
                direction, previouslyFocusedRect);
            if (took) {
                *res = took;
            } else {
                View::RequestFocus(direction, previouslyFocusedRect, res);
            }
            return NOERROR;
        }
    default:
        {
            Slogger::E(TAG, "descendant focusability must be "
                "one of FOCUS_BEFORE_DESCENDANTS, FOCUS_AFTER_DESCENDANTS"
                ", FOCUS_BLOCK_DESCENDANTS but is %d", descendantFocusability);

        }
    }
    *res = FALSE;
    return NOERROR;
}

/**
  * Look for a descendant to call {@link View#requestFocus} on.
  * Called by {@link ViewGroup#requestFocus(Int32, android.graphics.Rect)}
  * when it wants to request focus within its children.  Override this to
  * customize how your {@link ViewGroup} requests focus within its children.
  * @param direction One of FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, and FOCUS_RIGHT
  * @param previouslyFocusedRect The rectangle (in this View's coordinate system)
  *        to give a finer grained hint about where focus is coming from.  May be NULL
  *        if there is no hint.
  * @return Whether focus was taken.
  */
Boolean ViewGroup::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    Int32 index;
    Int32 increment;
    Int32 end;
    Int32 count = mChildrenCount;
    if (count == 0) {
        return FALSE;
    }

    if ((direction & IView::FOCUS_FORWARD) != 0) {
        index = 0;
        increment = 1;
        end = count;
    }
    else {
        index = count - 1;
        increment = -1;
        end = -1;
    }

    for (Int32 i = index; i != end; i += increment) {
        View* child = (View*)(*mChildren)[i];
        if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
            Boolean requestFocus;
            child->RequestFocus(direction, previouslyFocusedRect, &requestFocus);
            if (requestFocus) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

ECode ViewGroup::DispatchStartTemporaryDetach()
{
    View::DispatchStartTemporaryDetach();

    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->DispatchStartTemporaryDetach();
    }

    return NOERROR;
}

ECode ViewGroup::DispatchFinishTemporaryDetach()
{
    View::DispatchFinishTemporaryDetach();

    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->DispatchFinishTemporaryDetach();
    }

    return NOERROR;
}

ECode ViewGroup::DispatchAttachedToWindow(
    /* [in] */ AttachInfo* info,
    /* [in] */ Int32 visibility)
{
    mGroupFlags |= FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW;
    View::DispatchAttachedToWindow(info, visibility);
    mGroupFlags &= ~FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW;

    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* child = (View*)(*mChildren)[i];
        child->DispatchAttachedToWindow(info,
                visibility | (child->mViewFlags&VISIBILITY_MASK));
    }
    return NOERROR;
}

//@Override
void ViewGroup::DispatchScreenStateChanged(
    /* [in] */ Int32 screenState)
{
    View::DispatchScreenStateChanged(screenState);

    for (Int32 i = 0; i < mChildrenCount; i++) {
        ((View*)(*mChildren)[i])->DispatchScreenStateChanged(screenState);
    }
}

//@Override
Boolean ViewGroup::DispatchPopulateAccessibilityEventInternal(
    /* [in] */ IAccessibilityEvent* event)
{
    Boolean handled = FALSE, includeForAccessibility;
    if (IncludeForAccessibility(&includeForAccessibility), includeForAccessibility) {
        handled = View::DispatchPopulateAccessibilityEventInternal(event);
        if (handled) {
            return handled;
        }
    }
    // Let our children have a shot in populating the event.
    AutoPtr<ChildListForAccessibility> children = ChildListForAccessibility::Obtain(
        this, TRUE);
    Int32 childCount = children->GetChildCount();
    for (Int32 i = 0; i < childCount; i++) {
        View* child = VIEW_PROBE(children->GetChildAt(i));
        if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
            child->DispatchPopulateAccessibilityEvent(event, &handled);
            if (handled) {
                return handled;
            }
        }
    }

    children->Recycle();

    return FALSE;
}

//@Override
void ViewGroup::OnInitializeAccessibilityNodeInfoInternal(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    View::OnInitializeAccessibilityNodeInfoInternal(info);
    if (mAttachInfo != NULL) {
        mAttachInfo->mTempArrayList->Clear();

        AutoPtr<IArrayList> childrenForAccessibility = mAttachInfo->mTempArrayList;
        childrenForAccessibility->Clear();
        AddChildrenForAccessibility(childrenForAccessibility);

        Int32 childrenForAccessibilityCount;
        childrenForAccessibility->GetSize(&childrenForAccessibilityCount);

        for (Int32 i = 0; i < childrenForAccessibilityCount; i++) {
            AutoPtr<IInterface> temp;
            childrenForAccessibility->Get(i, (IInterface**)&temp);
            AutoPtr<IView> child = IView::Probe(temp);
            info->AddChildUnchecked(child);
        }
        childrenForAccessibility->Clear();
    }
}

//@Override
void ViewGroup::OnInitializeAccessibilityEventInternal(
    /* [in] */ IAccessibilityEvent* event)
{
    View::OnInitializeAccessibilityEventInternal(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("ViewGroup"), (ICharSequence**)&seq);
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
}

//@Override
ECode ViewGroup::NotifySubtreeAccessibilityStateChanged(
    /* [in] */ IView* child,
    /* [in] */ IView* source,
    /* [in] */ Int32 changeType)
{
    // If this is a live region, we should send a subtree change event
    // from this view. Otherwise, we can let it propagate up.
    Int32 accessibilityLiveRegion = 0;
    GetAccessibilityLiveRegion(&accessibilityLiveRegion);
    if (accessibilityLiveRegion != IView::ACCESSIBILITY_LIVE_REGION_NONE) {
        NotifyViewAccessibilityStateChangedIfNeeded(changeType);
    } else if (mParent != NULL) {
        //try {
        ECode ec = mParent->NotifySubtreeAccessibilityStateChanged(this, source, changeType);
        //} catch (AbstractMethodError e) {
        //    Log.e(TAG, mParent.getClass().getSimpleName() +
        //            " does not fully implement ViewParent", e);
        //}
        return ec;
    }
    return NOERROR;
}

//@Override
ECode ViewGroup::ResetSubtreeAccessibilityStateChanged()
{
    View::ResetSubtreeAccessibilityStateChanged();
    for (Int32 i = 0; i < mChildrenCount; i++) {
        AutoPtr<IView> child = (*mChildren)[i];
        VIEW_PROBE(child)->ResetSubtreeAccessibilityStateChanged();
    }

    return NOERROR;
}

ECode ViewGroup::DispatchDetachedFromWindow()
{
    // If we still have a touch target, we are still in the process of
    // dispatching motion events to a child; we need to get rid of that
    // child to avoid dispatching events to it after the window is torn
    // down. To make sure we keep the child in a consistent state, we
    // first send it an ACTION_CANCEL motion event.
    CancelAndClearTouchTargets(NULL);

    // Similarly, set ACTION_EXIT to all hover targets and clear them.
    ExitHoverTargets();

    // In case view is detached while transition is running
    mLayoutCalledWhileSuppressed = FALSE;

    // Tear down our drag tracking
    mDragNotifiedChildren.Clear();
    if (mCurrentDrag != NULL) {
        mCurrentDrag->Recycle();
        mCurrentDrag = NULL;
    }

    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* tmp = (View*)(*mChildren)[i];
        FAIL_RETURN(tmp->DispatchDetachedFromWindow());
    }
    ClearDisappearingChildren();
    return View::DispatchDetachedFromWindow();
}

void ViewGroup::InternalSetPadding(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    View::InternalSetPadding(left, top, right, bottom);

    if ((mPaddingLeft | mPaddingTop | mPaddingRight | mPaddingBottom) != 0) {
        mGroupFlags |= FLAG_PADDING_NOT_NULL;
    }
    else {
        mGroupFlags &= ~FLAG_PADDING_NOT_NULL;
    }
}

ECode ViewGroup::DispatchSaveInstanceState(
    /* [in] */ ISparseArray* container)
{
    FAIL_RETURN(View::DispatchSaveInstanceState(container));

    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* c = (View*)(*mChildren)[i];
        if ((c->mViewFlags & PARENT_SAVE_DISABLED_MASK) != PARENT_SAVE_DISABLED) {
            c->DispatchSaveInstanceState(container);
        }
    }

    return NOERROR;
}

ECode ViewGroup::DispatchFreezeSelfOnly(
    /* [in] */ ISparseArray* container)
{
    return View::DispatchSaveInstanceState(container);
}

ECode ViewGroup::DispatchRestoreInstanceState(
    /* [in] */ ISparseArray* container)
{
    FAIL_RETURN(View::DispatchRestoreInstanceState(container));

    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* c = (View*)(*mChildren)[i];
        if ((c->mViewFlags & PARENT_SAVE_DISABLED_MASK) != PARENT_SAVE_DISABLED) {
            c->DispatchRestoreInstanceState(container);
        }
    }

    return NOERROR;
}

ECode ViewGroup::DispatchThawSelfOnly(
    /* [in] */ ISparseArray* container)
{
    return View::DispatchRestoreInstanceState(container);
}

/**
  * Enables or disables the drawing cache for each child of this view group.
  *
  * @param enabled TRUE to enable the cache, FALSE to dispose of it
  */
void ViewGroup::SetChildrenDrawingCacheEnabled(
    /* [in] */ Boolean enabled)
{
    if (enabled || (mPersistentDrawingCache & PERSISTENT_ALL_CACHES) != PERSISTENT_ALL_CACHES) {
        for (Int32 i = 0; i < mChildrenCount; i++) {
            (*mChildren)[i]->SetDrawingCacheEnabled(enabled);
        }
    }
}

void ViewGroup::OnAnimationStart()
{
    View::OnAnimationStart();

    // When this ViewGroup's animation starts, build the cache for the children
    if ((mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE) {
        Boolean temp;
        IsHardwareAccelerated(&temp);
        Boolean buildCache = !temp;
        for (Int32 i = 0; i < mChildrenCount; i++) {
            View* child = (View*)(*mChildren)[i];
            if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
                child->SetDrawingCacheEnabled(TRUE);
                if (buildCache) {
                    child->BuildDrawingCache(TRUE);
                }
            }
        }

        mGroupFlags |= FLAG_CHILDREN_DRAWN_WITH_CACHE;
    }
}

void ViewGroup::OnAnimationEnd()
{
    View::OnAnimationEnd();

    // When this ViewGroup's animation ends, destroy the cache of the children
    if ((mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE) {
        mGroupFlags &= ~FLAG_CHILDREN_DRAWN_WITH_CACHE;

        if ((mPersistentDrawingCache & PERSISTENT_ANIMATION_CACHE) == 0) {
            SetChildrenDrawingCacheEnabled(FALSE);
        }
    }
}

ECode ViewGroup::CreateSnapshot(
    /* [in] */ BitmapConfig quality,
    /* [in] */ Int32 backgroundColor,
    /* [in] */ Boolean skipChildren,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    Int32 count = mChildrenCount;
    Int32* visibilities = NULL;

    if (skipChildren) {
        visibilities = new Int32[count];
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            child->GetVisibility(&visibilities[i]);
            if (visibilities[i] == IView::VISIBLE) {
                child->SetVisibility(IView::INVISIBLE);
            }
        }
    }

    AutoPtr<IBitmap> b;
    FAIL_RETURN(View::CreateSnapshot(quality, backgroundColor,
            skipChildren, (IBitmap**)&b));

    if (skipChildren) {
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            temp->SetVisibility(visibilities[i]);
        }
    }

    if (visibilities != NULL) {
        delete[] visibilities;
    }
    *bitmap = b;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

/** Return true if this ViewGroup is laying out using optical bounds. */
Boolean ViewGroup::IsLayoutModeOptical()
{
    return mLayoutMode == LAYOUT_MODE_OPTICAL_BOUNDS;
}

AutoPtr<IInsets> ViewGroup::ComputeOpticalInsets()
{
    if (IsLayoutModeOptical()) {
        Int32 left = 0, top = 0, right = 0, bottom = 0;
        for (Int32 i = 0; i < mChildrenCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility == IView::VISIBLE) {
                AutoPtr<IInsets> insets;
                child->GetOpticalInsets((IInsets**)&insets);
                Insets* temp = (Insets*)insets.Get();
                left = Elastos::Core::Math::Max(left, temp->mLeft);
                top = Elastos::Core::Math::Max(top,    temp->mTop);
                right = Elastos::Core::Math::Max(right,  temp->mRight);
                bottom = Elastos::Core::Math::Max(bottom, temp->mBottom);
            }
        }
        return Insets::Of(left, top, right, bottom);
    } else {
        return Insets::NONE;
    }
}

void ViewGroup::FillRect(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 x1,
    /* [in] */ Int32 y1,
    /* [in] */ Int32 x2,
    /* [in] */ Int32 y2)
{
    if (x1 != x2 && y1 != y2) {
        if (x1 > x2) {
            Int32 tmp = x1;
            x1 = x2;
            x2 = tmp;
        }
        if (y1 > y2) {
            Int32 tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        canvas->DrawRect(x1, y1, x2, y2, paint);
    }
}

Int32 ViewGroup::Sign(
    /* [in] */ Int32 x)
{
    return (x >= 0) ? 1 : -1;
}

void ViewGroup::DrawCorner(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 x1,
    /* [in] */ Int32 y1,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ Int32 lw)
{
    FillRect(canvas, paint, x1, y1, x1 + dx, y1 + lw * Sign(dy));
    FillRect(canvas, paint, x1, y1, x1 + lw * Sign(dx), y1 + dy);
}

Int32 ViewGroup::DipsToPixels(
    /* [in] */ Int32 dips)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float scale;
    metrics->GetDensity(&scale);
    return (Int32) (dips * scale + 0.5f);
}

void ViewGroup::DrawRectCorners(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 x1,
    /* [in] */ Int32 y1,
    /* [in] */ Int32 x2,
    /* [in] */ Int32 y2,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 lineLength,
    /* [in] */ Int32 lineWidth)
{
    DrawCorner(canvas, paint, x1, y1, lineLength, lineLength, lineWidth);
    DrawCorner(canvas, paint, x1, y2, lineLength, -lineLength, lineWidth);
    DrawCorner(canvas, paint, x2, y1, -lineLength, lineLength, lineWidth);
    DrawCorner(canvas, paint, x2, y2, -lineLength, -lineLength, lineWidth);
}

void ViewGroup::FillDifference(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 x2,
    /* [in] */ Int32 y2,
    /* [in] */ Int32 x3,
    /* [in] */ Int32 y3,
    /* [in] */ Int32 dx1,
    /* [in] */ Int32 dy1,
    /* [in] */ Int32 dx2,
    /* [in] */ Int32 dy2,
    /* [in] */ IPaint* paint)
{
    Int32 x1 = x2 - dx1;
    Int32 y1 = y2 - dy1;

    Int32 x4 = x3 + dx2;
    Int32 y4 = y3 + dy2;

    FillRect(canvas, paint, x1, y1, x4, y2);
    FillRect(canvas, paint, x1, y2, x2, y3);
    FillRect(canvas, paint, x3, y2, x4, y3);
    FillRect(canvas, paint, x1, y3, x4, y4);
}

void ViewGroup::DrawRect(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 x1,
    /* [in] */ Int32 y1,
    /* [in] */ Int32 x2,
    /* [in] */ Int32 y2)
{
    if (sDebugLines == NULL) {
        // TODO: This won't work with multiple UI threads in a single process
        sDebugLines = ArrayOf<Float>::Alloc(16);
    }

    (*sDebugLines)[0] = x1;
    (*sDebugLines)[1] = y1;
    (*sDebugLines)[2] = x2;
    (*sDebugLines)[3] = y1;

    (*sDebugLines)[4] = x2;
    (*sDebugLines)[5] = y1;
    (*sDebugLines)[6] = x2;
    (*sDebugLines)[7] = y2;

    (*sDebugLines)[8] = x2;
    (*sDebugLines)[9] = y2;
    (*sDebugLines)[10] = x1;
    (*sDebugLines)[11] = y2;

    (*sDebugLines)[12] = x1;
    (*sDebugLines)[13] = y2;
    (*sDebugLines)[14] = x1;
    (*sDebugLines)[15] = y1;

    canvas->DrawLines(sDebugLines, paint);
}

void ViewGroup::OnDebugDrawMargins(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    Int32 childrenCount;
    GetChildCount(&childrenCount);
    for (Int32 i = 0; i < childrenCount; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        assert(c != NULL);
        AutoPtr<IViewGroupLayoutParams> params;
        c->GetLayoutParams((IViewGroupLayoutParams**)&params);
        params->OnDebugDraw(c, canvas, paint);
    }
}

void ViewGroup::OnDebugDraw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IPaint> paint = GetDebugPaint();

    // Draw optical bounds
    {
        paint->SetColor(IColor::RED);
        paint->SetStyle(Elastos::Droid::Graphics::PaintStyle_STROKE);
        Int32 childrenCount;
        GetChildCount(&childrenCount);
        for (Int32 i = 0; i < childrenCount; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            AutoPtr<IInsets> insets;
            temp->GetOpticalInsets((IInsets**)&insets);
            Int32 l, r, t, b, tl, tr, tt, tb;
            insets->GetLeft(&l);
            insets->GetRight(&r);
            insets->GetTop(&t);
            insets->GetBottom(&b);
            temp->GetLeft(&tl);
            temp->GetRight(&tr);
            temp->GetTop(&tt);
            temp->GetBottom(&tb);
            DrawRect(canvas, paint, tl + l, tt + t, tr - r - 1, tb - b - 1);
        }
    }
    // Draw margins
    {
        Int32 marginsColor = Color::Argb(63, 255, 0, 255);
        paint->SetColor(marginsColor);
        paint->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL);

        OnDebugDrawMargins(canvas, paint);
    }

    // Draw clip bounds
    {
        Int32 clipColor = Color::Rgb(63, 127, 255);
        paint->SetColor(clipColor);
        paint->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL);

        Int32 lineLength = DipsToPixels(8);
        Int32 lineWidth = DipsToPixels(1);
        Int32 childrenCount;
        GetChildCount(&childrenCount);
        for (Int32 i = 0; i < childrenCount; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            Int32 tl, tr, tt, tb;
            temp->GetLeft(&tl);
            temp->GetRight(&tr);
            temp->GetTop(&tt);
            temp->GetBottom(&tb);
            DrawRectCorners(canvas, tl, tt, tr, tb, paint, lineLength, lineWidth);
        }
    }
}

ECode ViewGroup::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Logger::I(TAG, " >>>>> == %s DispatchDraw ==", TO_CSTR(this));
    Boolean usingRenderNodeProperties;
    canvas->IsRecordingFor(mRenderNode, &usingRenderNodeProperties);
    Int32 childrenCount = mChildrenCount;
    AutoPtr<ArrayOf<IView*> > children = mChildren;
    Int32 flags = mGroupFlags;

    Boolean bval;
    if ((flags & FLAG_RUN_ANIMATION) != 0 && CanAnimate()) {
        Boolean cache = (mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE;
        IsHardwareAccelerated(&bval);
        Boolean buildCache = !bval;

        for (Int32 i = 0; i < childrenCount; i++) {
            View* child = (View*)(*children)[i];
            if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
                AutoPtr<IViewGroupLayoutParams> params;
                child->GetLayoutParams((IViewGroupLayoutParams**)&params);
                AttachLayoutAnimationParameters((*children)[i], params, i, childrenCount);
                BindLayoutAnimation((*children)[i]);
                if (cache) {
                    child->SetDrawingCacheEnabled(TRUE);
                    if (buildCache) {
                        child->BuildDrawingCache(TRUE);
                    }
                }
            }
        }

        mLayoutAnimationController->WillOverlap(&bval);
        if (bval) {
            mGroupFlags |= FLAG_OPTIMIZE_INVALIDATE;
        }

        mLayoutAnimationController->Start();

        mGroupFlags &= ~FLAG_RUN_ANIMATION;
        mGroupFlags &= ~FLAG_ANIMATION_DONE;

        if (cache) {
            mGroupFlags |= FLAG_CHILDREN_DRAWN_WITH_CACHE;
        }

        if (mAnimationListener != NULL) {
            AutoPtr<IAnimation> animation;
            mLayoutAnimationController->GetAnimation((IAnimation**)&animation);
            mAnimationListener->OnAnimationStart(animation);
        }
    }

    Int32 clipSaveCount = 0;
    const Boolean clipToPadding = (flags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK;
    if (clipToPadding) {
        canvas->Save(&clipSaveCount);
        canvas->ClipRect(mScrollX + mPaddingLeft, mScrollY + mPaddingTop,
                mScrollX + mRight - mLeft - mPaddingRight,
                mScrollY + mBottom - mTop - mPaddingBottom, &bval);
    }

    // We will draw our child's animation, let's reset the flag
    mPrivateFlags &= ~PFLAG_DRAW_ANIMATION;
    mGroupFlags &= ~FLAG_INVALIDATE_REQUIRED;

    Boolean more = FALSE;
    Int64 drawingTime = 0;
    GetDrawingTime(&drawingTime);

    if (usingRenderNodeProperties) canvas->InsertReorderBarrier();
    // Only use the preordered list if not HW accelerated, since the HW pipeline will do the
    // draw reordering internally
    AutoPtr<IList> preorderedList = usingRenderNodeProperties ? NULL : BuildOrderedChildList();
    Boolean customOrder = preorderedList == NULL && IsChildrenDrawingOrderEnabled();
    for (Int32 i = 0; i < childrenCount; i++) {
        Int32 childIndex = customOrder ? GetChildDrawingOrder(childrenCount, i) : i;
        AutoPtr<IView> child;
        if (preorderedList == NULL) {
            child = (*children)[childIndex];
        }
        else {
            AutoPtr<IInterface> temp;
            preorderedList->Get(childIndex, (IInterface**)&temp);
            child = IView::Probe(temp);
        }
        AutoPtr<IAnimation> animation;
        if ((((View*)child.Get())->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE
            || (child->GetAnimation((IAnimation**)&animation), animation) != NULL) {
            more |= DrawChild(canvas, child, drawingTime);
        }
    }
    if (preorderedList != NULL) preorderedList->Clear();

    // Draw any disappearing views that have animations
    if (mDisappearingChildren != NULL) {
        Int32 disappearingCount;
        mDisappearingChildren->GetSize(&disappearingCount);
        // Go backwards -- we may delete as animations finish
        for (Int32 i = disappearingCount - 1; i >= 0; i--) {
            AutoPtr<IInterface> temp;
            mDisappearingChildren->Get(i, (IInterface**)&temp);
            AutoPtr<IView> child = IView::Probe(temp);
            Logger::I(TAG, "  DispatchDraw child: %s", TO_CSTR(child));
            more |= DrawChild(canvas, child, drawingTime);
        }
    }
    if (usingRenderNodeProperties) canvas->InsertInorderBarrier();

    if (DebugDraw()) {
        OnDebugDraw(canvas);
    }

    if (clipToPadding) {
        canvas->RestoreToCount(clipSaveCount);
    }

    // mGroupFlags might have been updated by drawChild()
    flags = mGroupFlags;

    if ((flags & FLAG_INVALIDATE_REQUIRED) == FLAG_INVALIDATE_REQUIRED) {
        Invalidate(TRUE);
    }

    Boolean isDone = FALSE;
    if (mLayoutAnimationController) {
        mLayoutAnimationController->IsDone(&isDone);
    }
    if ((flags & FLAG_ANIMATION_DONE) == 0 &&
        (flags & FLAG_NOTIFY_ANIMATION_LISTENER) == 0 && isDone && !more) {
        Logger::I(TAG, "  DispatchDraw drawChild() after the animation is over: %s", TO_CSTR(this));
        // We want to erase the drawing cache and notify the listener after the
        // next frame is drawn because one extra invalidate() is caused by
        // drawChild() after the animation is over
        mGroupFlags |= FLAG_NOTIFY_ANIMATION_LISTENER;
        AutoPtr<DispatchDrawRunnable> end = new DispatchDrawRunnable(this);
        Post(end, &bval);
    }

    Logger::I(TAG, " <<<<< == %s DispatchDraw ==", TO_CSTR(this));
    return NOERROR;
}

/**
 * Returns the ViewGroupOverlay for this view group, creating it if it does
 * not yet exist. In addition to {@link ViewOverlay}'s support for drawables,
 * {@link ViewGroupOverlay} allows views to be added to the overlay. These
 * views, like overlay drawables, are visual-only; they do not receive input
 * events and should not be used as anything other than a temporary
 * representation of a view in a parent container, such as might be used
 * by an animation effect.
 *
 * <p>Note: Overlays do not currently work correctly with {@link
 * SurfaceView} or {@link TextureView}; contents in overlays for these
 * types of views may not display correctly.</p>
 *
 * @return The ViewGroupOverlay object for this view.
 * @see ViewGroupOverlay
 */
//@Override
ECode ViewGroup::GetOverlay(
    /* [out] */ IViewOverlay** overlay)
{
    VALIDATE_NOT_NULL(overlay)
    if (mOverlay == NULL) {
        AutoPtr<IViewGroupOverlay> temp;
        CViewGroupOverlay::New(mContext, this, (IViewGroupOverlay**)&temp);
        mOverlay = IViewOverlay::Probe(temp);
        *overlay = mOverlay;
        REFCOUNT_ADD(*overlay);
    }
    return NOERROR;
}

/**
  * Returns the index of the child to draw for this iteration. Override this
  * if you want to change the drawing order of children. By default, it
  * returns i.
  * <p>
  * NOTE: In order for this method to be called, you must enable child ordering
  * first by calling {@link #setChildrenDrawingOrderEnabled(Boolean)}.
  *
  * @param i The current iteration.
  * @return The index of the child to draw this iteration.
  *
  * @see #setChildrenDrawingOrderEnabled(Boolean)
  * @see #isChildrenDrawingOrderEnabled()
  */
Int32 ViewGroup::GetChildDrawingOrder(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 i)
{
    return i;
}

Boolean ViewGroup::HasChildWithZ()
{
    for (Int32 i = 0; i < mChildrenCount; i++) {
        Float z;
        (*mChildren)[i]->GetZ(&z);
        if (z != 0) return TRUE;
    }
    return FALSE;
}

/**
 * Populates (and returns) mPreSortedChildren with a pre-ordered list of the View's children,
 * sorted first by Z, then by child drawing order (if applicable).
 *
 * Uses a stable, insertion sort which is commonly O(n) for ViewGroups with very few elevated
 * children.
 */
AutoPtr<IList> ViewGroup::BuildOrderedChildList()
{
    Int32 count = mChildrenCount;
    if (count <= 1 || !HasChildWithZ()) return NULL;

    if (mPreSortedChildren == NULL) {
        CArrayList::New(count, (IList**)&mPreSortedChildren);
    } else {
        IArrayList::Probe(mPreSortedChildren)->EnsureCapacity(count);
    }

    Boolean useCustomOrder = IsChildrenDrawingOrderEnabled();
    for (Int32 i = 0; i < mChildrenCount; i++) {
        // add next child (in child order) to end of list
        Int32 childIndex = useCustomOrder ? GetChildDrawingOrder(mChildrenCount, i) : i;
        AutoPtr<IView> nextChild = (*mChildren)[childIndex];
        float currentZ;
        nextChild->GetZ(&currentZ);

        // insert ahead of any Views with greater Z
        Int32 insertIndex = i;
        while (insertIndex > 0) {
            AutoPtr<IInterface> temp;
            mPreSortedChildren->Get(insertIndex - 1, (IInterface**)&temp);
            Float z;
            VIEW_PROBE(temp)->GetZ(&z);
            if (z > currentZ) {
                insertIndex--;
            }
            else break;
        }
        mPreSortedChildren->Add(insertIndex, nextChild);
    }
    return mPreSortedChildren;
}

void ViewGroup::NotifyAnimationListener()
{
    mGroupFlags &= ~FLAG_NOTIFY_ANIMATION_LISTENER;
    mGroupFlags |= FLAG_ANIMATION_DONE;

    if (mAnimationListener != NULL) {
        AutoPtr<IRunnable> end = new NotifyAnimationListenerRunnable(this);
        Boolean isPost;
        Post(end, &isPost);
    }

    if ((mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE) {
        mGroupFlags &= ~FLAG_CHILDREN_DRAWN_WITH_CACHE;
        if ((mPersistentDrawingCache & PERSISTENT_ANIMATION_CACHE) == 0) {
            SetChildrenDrawingCacheEnabled(FALSE);
        }
    }

    Invalidate(TRUE);
}

void ViewGroup::DispatchGetDisplayList()
{
    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* child = (View*)(*mChildren)[i];
        AutoPtr<IAnimation> animation;

        if (((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE
            || (child->GetAnimation((IAnimation**)&animation), animation) != NULL) &&
                child->HasStaticLayer()) {
            RecreateChildDisplayList(child);
        }
    }
    if (mOverlay != NULL) {
        AutoPtr<IViewGroup> temp;
        mOverlay->GetOverlayView((IViewGroup**)&temp);
        AutoPtr<IView> overlayView = IView::Probe(temp);
        RecreateChildDisplayList(overlayView);
    }
    if (mDisappearingChildren != NULL) {
        AutoPtr<IList> disappearingChildren = mDisappearingChildren;
        Int32 disappearingCount;
        disappearingChildren->GetSize(&disappearingCount);
        for (Int32 i = 0; i < disappearingCount; ++i) {
            AutoPtr<IInterface> temp;
            disappearingChildren->Get(i, (IInterface**)&temp);
            AutoPtr<IView> child = IView::Probe(temp);
            RecreateChildDisplayList(child);
        }
    }
}

void ViewGroup::RecreateChildDisplayList(
    /* [in] */ IView* child)
{
    View* temp = VIEW_PROBE(child);
    temp->mRecreateDisplayList = (temp->mPrivateFlags & PFLAG_INVALIDATED) == PFLAG_INVALIDATED;
    temp->mPrivateFlags &= ~PFLAG_INVALIDATED;
    AutoPtr<IRenderNode> node;
    temp->GetDisplayList((IRenderNode**)&node);
    temp->mRecreateDisplayList = FALSE;
}

/**
  * Draw one child of this View Group. This method is responsible for getting
  * the canvas in the right state. This includes clipping, translating so
  * that the child's scrolled origin is at 0, 0, and applying any animation
  * transformations.
  *
  * @param canvas The canvas on which to draw the child
  * @param child Who to draw
  * @param drawingTime The time at which draw is occuring
  * @return True if an invalidate() was issued
  */
Boolean ViewGroup::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    return ((View*)child)->Draw(canvas, this, drawingTime);
}

/**
 * Returns whether this group's children are clipped to their bounds before drawing.
 * The default value is true.
 * @see #setClipChildren(boolean)
 *
 * @return True if the group's children will be clipped to their bounds,
 * false otherwise.
 */
//@ViewDebug.ExportedProperty(category = "drawing")
ECode ViewGroup::GetClipChildren(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = ((mGroupFlags & FLAG_CLIP_CHILDREN) != 0);
    return NOERROR;
}

/**
  * By default, children are clipped to their bounds before drawing. This
  * allows view groups to override this behavior for animations, etc.
  *
  * @param clipChildren TRUE to clip children to their bounds,
  *        FALSE otherwise
  * @attr ref android.R.styleable#ViewGroup_clipChildren
  */
ECode ViewGroup::SetClipChildren(
    /* [in] */ Boolean clipChildren)
{
    Boolean previousValue = (mGroupFlags & FLAG_CLIP_CHILDREN) == FLAG_CLIP_CHILDREN;
    if (clipChildren != previousValue) {
        SetBooleanFlag(FLAG_CLIP_CHILDREN, clipChildren);
        for (Int32 i = 0; i < mChildrenCount; ++i) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            View* child = VIEW_PROBE(temp);
            if (child->mRenderNode != NULL) {
                Boolean r;
                child->mRenderNode->SetClipToBounds(clipChildren, &r);
            }
        }
        Invalidate(TRUE);
    }
    return NOERROR;
}

/**
  * By default, children are clipped to the padding of the ViewGroup. This
  * allows view groups to override this behavior
  *
  * @param clipToPadding TRUE to clip children to the padding of the
  *        group, FALSE otherwise
  * @attr ref android.R.styleable#ViewGroup_clipToPadding
  */
ECode ViewGroup::SetClipToPadding(
    /* [in] */ Boolean clipToPadding)
{
    if (HasBooleanFlag(FLAG_CLIP_TO_PADDING) != clipToPadding) {
        SetBooleanFlag(FLAG_CLIP_TO_PADDING, clipToPadding);
        Invalidate(TRUE);
    }
    return NOERROR;
}

/**
 * Check if this ViewGroup is configured to clip child views to its padding.
 *
 * @return true if this ViewGroup clips children to its padding, false otherwise
 *
 * @attr ref android.R.styleable#ViewGroup_clipToPadding
 */
//@ViewDebug.ExportedProperty(category = "drawing")
ECode ViewGroup::GetClipToPadding(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = HasBooleanFlag(FLAG_CLIP_TO_PADDING);
    return NOERROR;
}

/**
  * {@inheritDoc}
  */
//@Override
ECode ViewGroup::DispatchSetSelected(
    /* [in] */ Boolean selected)
{
    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->SetSelected(selected);
    }
    return NOERROR;
}

ECode ViewGroup::DispatchSetActivated(
    /* [in] */ Boolean activated)
{
    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->SetActivated(activated);
    }
    return NOERROR;
}

//@Override
void ViewGroup::DispatchSetPressed(
    /* [in] */ Boolean pressed)
{
    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* child = (View*)(*mChildren)[i];
        // Children that are clickable on their own should not
        // show a pressed state when their parent view does.
        // Clearing a pressed state always propagates.
        Boolean isClickable, isLongClickable;
        if (!pressed || ((child->IsClickable(&isClickable), !isClickable)
            && (child->IsLongClickable(&isLongClickable), !isLongClickable))) {
            child->SetPressed(pressed);
        }
    }
}

//@Override
void ViewGroup::DispatchCancelPendingInputEvents()
{
    View::DispatchCancelPendingInputEvents();
    for (Int32 i = 0; i < mChildrenCount; i++) {
        ((View*)(*mChildren)[i])->DispatchCancelPendingInputEvents();
    }
}

/**
  * When this property is set to TRUE, this ViewGroup supports static transformations on
  * children; this causes
  * {@link #getChildStaticTransformation(View, android.view.animation.Transformation)} to be
  * invoked when a child is drawn.
  *
  * Any subclass overriding
  * {@link #getChildStaticTransformation(View, android.view.animation.Transformation)} should
  * set this property to TRUE.
  *
  * @param enabled True to enable static transformations on children, FALSE otherwise.
  *
  * @see #FLAG_SUPPORT_STATIC_TRANSFORMATIONS
  */
void ViewGroup::SetStaticTransformationsEnabled(
    /* [in] */ Boolean enabled)
{
    SetBooleanFlag(FLAG_SUPPORT_STATIC_TRANSFORMATIONS, enabled);
}

Boolean ViewGroup::GetChildStaticTransformation(
    /* [in] */ IView* child,
    /* [in] */ ITransformation* t)
{
    return FALSE;
}

AutoPtr<ITransformation> ViewGroup::GetChildTransformation()
{
    if (mChildTransformation == NULL) {
        CTransformation::New((ITransformation**)&mChildTransformation);
    }
    return mChildTransformation;
}

AutoPtr<IView> ViewGroup::FindViewTraversal(
    /* [in] */ Int32 id)
{
    if (id == mID) {
        return this;
    }

    for (Int32 i = 0; i < mChildrenCount; i++) {
        Boolean isRootNS;
        (*mChildren)[i]->IsRootNamespace(&isRootNS);
        if (!isRootNS) {
            AutoPtr<IView> vv;
            (*mChildren)[i]->FindViewById(id, (IView**)&vv);
            if (vv != NULL) {
                return vv;
            }
        }
    }

    return NULL;
}

/**
   * {@hide}
   */
//@Override
AutoPtr<IView> ViewGroup::FindViewWithTagTraversal(
    /* [in] */ IInterface* tag)
{
   if (tag != NULL && tag == mTag) {
       return this;
   }

   for (Int32 i = 0; i < mChildrenCount; i++) {
       View* v = (View*)(*mChildren)[i];

       if ((v->mPrivateFlags & PFLAG_IS_ROOT_NAMESPACE) == 0) {
           AutoPtr<IView> view;
           v->FindViewWithTag(tag, (IView**)&view);
           if (view != NULL) {
               return view;
           }
       }
   }

   return NULL;
}

AutoPtr<IView> ViewGroup::FindViewByPredicateTraversal(
    /* [in] */ IPredicate* predicate,
    /* [in] */ IView* childToSkip)
{
    Boolean apply = FALSE;
    if ((predicate->Apply((IViewGroup*)this, &apply), apply)) {
        return this;
    }

    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* v = (View*)(*mChildren)[i];

        if ((*mChildren)[i] != childToSkip &&
            (v->mPrivateFlags & PFLAG_IS_ROOT_NAMESPACE) == 0) {
            AutoPtr<IView> view;
            v->FindViewByPredicate(predicate, (IView**)&view);
            if (view != NULL) {
                return view;
            }
        }
    }

    return NULL;
}

/**
 * Adds a child view. If no layout parameters are already set on the child, the
 * default parameters for this ViewGroup are set on the child.
 *
 * @param child the child view to add
 *
 * @see #generateDefaultLayoutParams()
 */
ECode ViewGroup::AddView(
    /* [in] */ IView* child)
{
    return AddView(child, -1);
}

/**
 * Adds a child view. If no layout parameters are already set on the child, the
 * default parameters for this ViewGroup are set on the child.
 *
 * @param child the child view to add
 * @param index the position at which to add the child
 *
 * @see #generateDefaultLayoutParams()
 */
ECode ViewGroup::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    if (params == NULL) {
        GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&params);
        if (params == NULL) {
//            throw new IllegalArgumentException("generateDefaultLayoutParams() cannot return NULL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return AddView(child, index, params);
}

/**
 * Adds a child view with this ViewGroup's default layout parameters and the
 * specified width and height.
 *
 * @param child the child view to add
 */
ECode ViewGroup::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IViewGroupLayoutParams> params;
    GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&params);

    params->SetWidth(width);
    params->SetHeight(height);
    return AddView(child, -1, params);
}

/**
 * Adds a child view with the specified layout parameters.
 *
 * @param child the child view to add
 * @param params the layout parameters to set on the child
 */
ECode ViewGroup::AddView(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params)
{
    return AddView(child, -1, params);
}

/**
 * Adds a child view with the specified layout parameters.
 *
 * @param child the child view to add
 * @param index the position at which to add the child
 * @param params the layout parameters to set on the child
 */
ECode ViewGroup::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    //if (DBG) {
    //    System.out.println(this + " addView");
    //}

    // addViewInner() will call child.requestLayout() when setting the new LayoutParams
    // therefore, we call requestLayout() on ourselves before, so that the child's request
    // will be blocked at our level
    RequestLayout();
    Invalidate(TRUE);
    return AddViewInner(child, index, params, FALSE);
}

/**
  * {@inheritDoc}
  */
ECode ViewGroup::UpdateViewLayout(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    if (!CheckLayoutParams(params)) {
//        throw new IllegalArgumentException("Invalid LayoutParams supplied to " + this);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IViewParent> parent;
    view->GetParent((IViewParent**)&parent);
    if (parent.Get() != (IViewParent*)this) {
//        throw new IllegalArgumentException("Given view not a child of " + this);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return view->SetLayoutParams(params);
}

Boolean ViewGroup::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return  p != NULL;
}

ECode ViewGroup::SetOnHierarchyChangeListener(
    /* [in] */ IViewGroupOnHierarchyChangeListener* listener)
{
    mOnHierarchyChangeListener = listener;
    return NOERROR;
}

void ViewGroup::OnViewAdded(
    /* [in] */ IView* child)
{
    if (mOnHierarchyChangeListener != NULL) {
        mOnHierarchyChangeListener->OnChildViewAdded(this, child);
    }
}

void ViewGroup::OnViewRemoved(
    /* [in] */ IView* child)
{
    if (mOnHierarchyChangeListener != NULL) {
        mOnHierarchyChangeListener->OnChildViewRemoved(this, child);
    }
}

void ViewGroup::ClearCachedLayoutMode()
{
    if (!HasBooleanFlag(FLAG_LAYOUT_MODE_WAS_EXPLICITLY_SET)) {
       mLayoutMode = LAYOUT_MODE_UNDEFINED;
    }
}

//@Override
ECode ViewGroup::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    ClearCachedLayoutMode();
    return NOERROR;
}

//@Override
ECode ViewGroup::OnDetachedFromWindow()
{
    View::OnDetachedFromWindow();
    ClearCachedLayoutMode();
    return NOERROR;
}

Boolean ViewGroup::AddViewInLayout(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    return AddViewInLayout(child, index, params, FALSE);
}

Boolean ViewGroup::AddViewInLayout(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params,
    /* [in] */ Boolean preventRequestLayout)
{
    View* v = ((View*)child);
    v->mParent = NULL;
    AddViewInner(child, index, params, preventRequestLayout);
    v->mPrivateFlags = (v->mPrivateFlags & ~PFLAG_DIRTY_MASK) | PFLAG_DRAWN;
    return TRUE;
}


void ViewGroup::CleanupLayoutState(
    /* [in] */ IView* child)
{
    View* v = ((View*)child);
    v->mPrivateFlags &= ~View::PFLAG_FORCE_LAYOUT;
}

ECode ViewGroup::AddViewInner(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* _params,
    /* [in] */ Boolean preventRequestLayout)
{
    assert(child != NULL && "Child view cannot be NULL.");

    if (mTransition != NULL) {
        // Don't prevent other add transitions from completing, but cancel remove
        // transitions to let them complete the process before we add to the container
        mTransition->Cancel(ILayoutTransition::DISAPPEARING);
    }

    AutoPtr<IViewGroupLayoutParams> params = _params;
    View* v = ((View*)child);
    AutoPtr<IViewParent> parent;
    v->GetParent((IViewParent**)&parent);
    if (parent != NULL) {
//        throw new IllegalStateException("The specified child already has a parent. " +
//                "You must call removeView() on the child's parent first.");
        Slogger::E("ViewGroup", "The specified child already has a parent. " \
            "You must call removeView() on the child's parent first.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mTransition != NULL) {
        mTransition->AddChild(this, child);
    }

    if (!CheckLayoutParams(params)) {
        params = GenerateLayoutParams(params);
    }

    if (preventRequestLayout) {
        v->mLayoutParams = params;
    }
    else {
        v->SetLayoutParams(params);
    }

    if (index < 0) {
        index = mChildrenCount;
    }

    AddInArray(child, index);

    // tell our children
    if (preventRequestLayout) {
        v->AssignParent(this);
    }
    else {
        v->mParent = this;
    }
    Boolean hasFocus;
    if (v->HasFocus(&hasFocus), hasFocus) {
        AutoPtr<IView> focus;
        v->FindFocus((IView**)&focus);
        RequestChildFocus(child, focus);
    }

    AttachInfo* ai = mAttachInfo;
    if (ai != NULL && (mGroupFlags & FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW) == 0) {
        Boolean lastKeepOn = ai->mKeepScreenOn;
        ai->mKeepScreenOn = FALSE;
        v->DispatchAttachedToWindow(mAttachInfo, (mViewFlags & VISIBILITY_MASK));
        if (ai->mKeepScreenOn) {
            NeedGlobalAttributesUpdate(TRUE);
        }
        ai->mKeepScreenOn = lastKeepOn;
    }
    Boolean isLayoutDirectionInherited;
    v->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
    if (isLayoutDirectionInherited) {
        child->ResetRtlProperties();
    }

    OnViewAdded(child);

    if ((v->mViewFlags & DUPLICATE_PARENT_STATE) == DUPLICATE_PARENT_STATE) {
        mGroupFlags |= FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE;
    }
    Boolean hasTransientState;
    v->HasTransientState(&hasTransientState);
    if (hasTransientState) {
        ChildHasTransientStateChanged(child, TRUE);
    }

    Int32 lastVisibility;
    child->GetVisibility(&lastVisibility);
    if (lastVisibility != IView::GONE) {
        NotifySubtreeAccessibilityStateChangedIfNeeded();
    }

    return NOERROR;
}

ECode ViewGroup::AddInArray(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    AutoPtr<ArrayOf<IView*> > children = mChildren;
    const Int32 count = mChildrenCount;
    const Int32 size = children->GetLength();
    if (index == count) {
        if (size == count) {
            mChildren = ArrayOf<IView*>::Alloc(size + ARRAY_CAPACITY_INCREMENT);
            for (Int32 i = 0; i < size; i++) {
                mChildren->Set(i, (*children)[i]);
            }
            children = mChildren;
        }
        children->Set(mChildrenCount++, child);
    }
    else if (index < count) {
        if (size == count) {
            mChildren = ArrayOf<IView*>::Alloc(size + ARRAY_CAPACITY_INCREMENT);
            for (Int32 i = 0; i < index; i++) {
                mChildren->Set(i, (*children)[i]);
            }

            for (Int32 i = index; i < count - index; i++) {
                mChildren->Set(i + 1, (*children)[i]);
            }
            children = mChildren;
        }
        else {
            for (Int32 i = count - 1; i >= index; --i) {
                children->Set(i + 1, (*children)[i]);
            }
        }
        children->Set(index, child);
        mChildrenCount++;

        if (mLastTouchDownIndex >= index) {
            mLastTouchDownIndex++;
        }
    }
    else {
        Slogger::E(TAG, "index = %d, count = %d", index, count);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return NOERROR;
}

// This method also sets the child's mParent to NULL
ECode ViewGroup::RemoveFromArray(
    /* [in] */ Int32 index)
{
    AutoPtr<IView> v = (*mChildren)[index];
    if (Find(mTransitioningViews.Begin(), mTransitioningViews.End(), v)
            == mTransitioningViews.End()) {
        VIEW_PROBE(v)->mParent = NULL;
    }

    const Int32 count = mChildrenCount;
    if (index == count - 1) {
        mChildren->Set(--mChildrenCount, NULL);
    }
    else if (index >= 0 && index < count) {
        for (Int32 i = index; i < count - 1; i++) {
            mChildren->Set(i, (*mChildren)[i + 1]);
        }
        mChildren->Set(--mChildrenCount, NULL);
    }
    else {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (mLastTouchDownIndex == index) {
        mLastTouchDownTime = 0;
        mLastTouchDownIndex = -1;
    }
    else if (mLastTouchDownIndex > index) {
        mLastTouchDownIndex--;
    }

    return NOERROR;
}

// This method also sets the children's mParent to NULL
ECode ViewGroup::RemoveFromArray(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    AutoPtr<ArrayOf<IView*> > children = mChildren;
    const Int32 childrenCount = mChildrenCount;

    start = Elastos::Core::Math::Max(0, start);
    const Int32 end = Elastos::Core::Math::Min(childrenCount, start + count);

    if (start == end) {
        return NOERROR;
    }

    if (end == childrenCount) {
        for (Int32 i = start; i < end; i++) {
            ((View*)(*mChildren)[i])->mParent = NULL;
            mChildren->Set(i, NULL);
        }
    }
    else {
        for (Int32 i = start; i < end; i++) {
            ((View*)(*mChildren)[i])->mParent = NULL;
        }

        // Since we're looping above, we might as well do the copy, but is arraycopy()
        // faster than the extra 2 bounds checks we would do in the loop?
        //TODO
        //System.arraycopy(children, end, children, start, childrenCount - end);
        for (Int32 i = end; i < childrenCount; i++) {
            mChildren->Set(start + i - end, (*mChildren)[i]);
        }

        for (Int32 i = childrenCount - (end - start); i < childrenCount; i++) {
            mChildren->Set(i, NULL);
        }
    }

    mChildrenCount -= (end - start);
    return NOERROR;
}

void ViewGroup::BindLayoutAnimation(
    /* [in] */ IView* child)
{
    AutoPtr<IAnimation> a;
    mLayoutAnimationController->GetAnimationForView(child, (IAnimation**)&a);
    child->SetAnimation(a);
}


ECode ViewGroup::AttachLayoutAnimationParameters(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count)
{
    AutoPtr<IAnimationParameters> animationParams;
    params->GetLayoutAnimationParameters((IAnimationParameters**)&animationParams);
    if (animationParams == NULL) {
        animationParams = new LayoutAnimationController::AnimationParameters();
        params->SetLayoutAnimationParameters(animationParams);
    }

    animationParams->SetCount(count);
    animationParams->SetIndex(index);

    return NOERROR;
}

ECode ViewGroup::RemoveView(
    /* [in] */ IView* view)
{
    RemoveViewInternal(view);
    RequestLayout();
    Invalidate(TRUE);
    return NOERROR;
}

ECode ViewGroup::RemoveViewInLayout(
    /* [in] */ IView* view)
{
    RemoveViewInternal(view);

    return NOERROR;
}

ECode ViewGroup::RemoveViewsInLayout(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    RemoveViewsInternal(start, count);

    return NOERROR;
}


ECode ViewGroup::RemoveViewAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IView> temp;
    GetChildAt(index, (IView**)&temp);
    RemoveViewInternal(index, temp);
    RequestLayout();
    Invalidate(TRUE);
    return NOERROR;
}

ECode ViewGroup::RemoveViews(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    RemoveViewsInternal(start, count);
    RequestLayout();
    Invalidate(TRUE);

    return NOERROR;
}

void ViewGroup::RemoveViewInternal(
    /* [in] */ IView* view)
{
    Int32 index;
    IndexOfChild(view, &index);
    if (index >= 0) {
        RemoveViewInternal(index, view);
    }
}

void ViewGroup::RemoveViewInternal(
    /* [in] */ Int32 index,
    /* [in] */ IView* view)
{
    if (mTransition != NULL) {
        mTransition->RemoveChild(this, view);
    }

    View* v = (View*)view;
    Boolean clearChildFocus = FALSE;
    if (view == mFocused) {
        v->UnFocus(NULL);
        clearChildFocus = TRUE;
    }
    Boolean isAccessibilityFocused;
    v->IsAccessibilityFocused(&isAccessibilityFocused);
    if (isAccessibilityFocused) {
        view->ClearAccessibilityFocus();
    }

    CancelTouchTarget(view);
    CancelHoverTarget(view);

    Boolean contains = FALSE;
    List<AutoPtr<IView> >::Iterator ator = mTransitioningViews.Begin();
    for (; ator != mTransitioningViews.End(); ++ator) {
        if (ator->Get() == view) {
            contains = TRUE;
            break;
        }
    }
    AutoPtr<IAnimation> animation;
    if ((v->GetAnimation((IAnimation**)&animation), animation) != NULL || contains) {
        AddDisappearingView(view);
    }
    else if (v->mAttachInfo != NULL) {
        v->DispatchDetachedFromWindow();
    }
    Boolean hasTransientState;
    v->HasTransientState(&hasTransientState);
    if (hasTransientState) {
        ChildHasTransientStateChanged(view, FALSE);
    }

    NeedGlobalAttributesUpdate(FALSE);

    RemoveFromArray(index);

    if (clearChildFocus) {
        ClearChildFocus(view);
        Boolean rootViewRequestFocus;
        RootViewRequestFocus(&rootViewRequestFocus);
        if (!rootViewRequestFocus) {
            NotifyGlobalFocusCleared(this);
        }
    }

    OnViewRemoved(view);

    Int32 lastVisibility;
    view->GetVisibility(&lastVisibility);
    if (lastVisibility != IView::GONE) {
        NotifySubtreeAccessibilityStateChangedIfNeeded();
    }
}

ECode ViewGroup::SetLayoutTransition(
    /* [in] */ ILayoutTransition* transition)
{
    if (mTransition != NULL) {
        AutoPtr<ILayoutTransition> previousTransition = mTransition;
        previousTransition->Cancel();
        previousTransition->RemoveTransitionListener(mLayoutTransitionListener);
    }

    mTransition = transition;
    if (mTransition != NULL) {
        mTransition->AddTransitionListener(mLayoutTransitionListener);
    }

    return NOERROR;
}

ECode ViewGroup::GetLayoutTransition(
    /* [out] */ ILayoutTransition** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mTransition;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

void ViewGroup::RemoveViewsInternal(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    AutoPtr<IView> focused = mFocused;
    Boolean detach = mAttachInfo != NULL;
    Boolean clearChildFocus = FALSE;

    const Int32 end = start + count;

    for (Int32 i = start; i < end; i++) {
        AutoPtr<IView> _view = (*mChildren)[i];
        View* view = VIEW_PROBE(_view);

        if (mTransition != NULL) {
            mTransition->RemoveChild(this, _view);
        }

        if (_view == focused) {
            view->UnFocus(NULL);
            clearChildFocus = TRUE;
        }

        Boolean isAccessibilityFocused;
        view->IsAccessibilityFocused(&isAccessibilityFocused);
        if (isAccessibilityFocused) {
            view->ClearAccessibilityFocus();
        }

        CancelTouchTarget(_view);
        CancelHoverTarget(_view);

        Boolean contains = FALSE;
        List<AutoPtr<IView> >::Iterator ator = mTransitioningViews.Begin();
        for (; ator != mTransitioningViews.End(); ++ator) {
            if (ator->Get() == _view) {
                contains = TRUE;
            }
        }
        AutoPtr<IAnimation> animation;
        if ((view->GetAnimation((IAnimation**)&animation), animation) != NULL || contains) {
            AddDisappearingView(_view);
        }
        else if (detach) {
           view->DispatchDetachedFromWindow();
        }
        Boolean hasTransientState;
        view->HasTransientState(&hasTransientState);
        if (hasTransientState) {
            ChildHasTransientStateChanged(_view, FALSE);
        }

        NeedGlobalAttributesUpdate(FALSE);

        OnViewRemoved(_view);
    }

    RemoveFromArray(start, count);

    if (clearChildFocus) {
        ClearChildFocus(focused);
        Boolean rootViewRequestFocus;
        RootViewRequestFocus(&rootViewRequestFocus);
        if (!rootViewRequestFocus) {
            NotifyGlobalFocusCleared(focused);
        }
    }
}

ECode ViewGroup::RemoveAllViews()
{
    RemoveAllViewsInLayout();
    RequestLayout();
    Invalidate(TRUE);
    return NOERROR;
}


ECode ViewGroup::RemoveAllViewsInLayout()
{
    const Int32 count = mChildrenCount;
    if (count <= 0) {
        return NOERROR;
    }

    mChildrenCount = 0;

    AutoPtr<IView> focused = mFocused;
    Boolean detach = mAttachInfo != NULL;
    Boolean clearChildFocus = FALSE;

    NeedGlobalAttributesUpdate(FALSE);

    for (Int32 i = count - 1; i >= 0; i--) {
        AutoPtr<IView> view = (*mChildren)[i];

        if (mTransition != NULL) {
            mTransition->RemoveChild(this, view);
        }

        if (view == focused) {
            (VIEW_PROBE(view))->UnFocus(NULL);
            clearChildFocus = TRUE;
        }
        Boolean isAccessibilityFocused;
        view->IsAccessibilityFocused(&isAccessibilityFocused);
        if (isAccessibilityFocused) {
            view->ClearAccessibilityFocus();
        }

        CancelTouchTarget(view);
        CancelHoverTarget(view);

        List<AutoPtr<IView> >::Iterator ator =
            Find(mTransitioningViews.Begin(), mTransitioningViews.End(), view);
        Boolean contains = (ator != mTransitioningViews.End());

        View* v = (View*)view.Get();
        AutoPtr<IAnimation> animation;
        if ((v->GetAnimation((IAnimation**)&animation), animation) != NULL || contains) {
            AddDisappearingView(view);
        }
        else if (detach) {
           v->DispatchDetachedFromWindow();
        }
        Boolean hasTransientState;
        v->HasTransientState(&hasTransientState);
        if (hasTransientState) {
            ChildHasTransientStateChanged(view, FALSE);
        }

        OnViewRemoved(view);

        v->mParent = NULL;
        mChildren->Set(i, NULL);
    }

    if (clearChildFocus) {
        ClearChildFocus(focused);
        Boolean rootViewRequestFocus;
        RootViewRequestFocus(&rootViewRequestFocus);
        if (!rootViewRequestFocus) {
            NotifyGlobalFocusCleared(focused);
        }
    }

    return NOERROR;
}

void ViewGroup::RemoveDetachedView(
    /* [in] */ IView* child,
    /* [in] */ Boolean animate)
{
    if (mTransition != NULL) {
        mTransition->RemoveChild(this, child);
    }

    if (child == mFocused) {
        child->ClearFocus();
    }

    child->ClearAccessibilityFocus();

    CancelTouchTarget(child);
    CancelHoverTarget(child);

    View* _child = ((View*)child);
    assert(_child != NULL);

    Boolean contains = FALSE;
    List<AutoPtr<IView> >::Iterator ator = mTransitioningViews.Begin();
    for (; ator != mTransitioningViews.End(); ++ator) {
        if (ator->Get() == child) {
            contains = TRUE;
        }
    }
    AutoPtr<IAnimation> animation;
    if ((animate && (_child->GetAnimation((IAnimation**)&animation) ,animation) != NULL) || contains) {
        AddDisappearingView(child);
    }
    else if (_child->mAttachInfo != NULL) {
        _child->DispatchDetachedFromWindow();
    }
    Boolean hasTransientState;
    _child->HasTransientState(&hasTransientState);
    if (hasTransientState) {
        ChildHasTransientStateChanged(child, FALSE);
    }

    OnViewRemoved(child);
}

void ViewGroup::AttachViewToParent(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    assert(child != NULL);
    View* v = ((View*)child);

    v->mLayoutParams = params;

    if (index < 0) {
        index = mChildrenCount;
    }

    AddInArray(child, index);

    v->mParent = this;
    v->mPrivateFlags = (v->mPrivateFlags & ~PFLAG_DIRTY_MASK
                    & ~PFLAG_DRAWING_CACHE_VALID)
            | PFLAG_DRAWN | PFLAG_INVALIDATED;
    mPrivateFlags |= PFLAG_INVALIDATED;
    Boolean hasFocus;
    v->HasFocus(&hasFocus);
    if (hasFocus) {
        AutoPtr<IView> temp;
        v->FindFocus((IView**)&temp);
        RequestChildFocus(child, temp);
    }
}

void ViewGroup::DetachViewFromParent(
    /* [in] */ IView* child)
{
    Int32 index;
    IndexOfChild(child, &index);
    RemoveFromArray(index);
}

void ViewGroup::DetachViewFromParent(
    /* [in] */ Int32 index)
{
    RemoveFromArray(index);
}

void ViewGroup::DetachViewsFromParent(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    RemoveFromArray(start, count);
}

void ViewGroup::DetachAllViewsFromParent()
{
    Int32 count = mChildrenCount;
    if (count <= 0) {
        return;
    }

    mChildrenCount = 0;

    for (int i = count - 1; i >= 0; i--) {
        ((View*)(*mChildren)[i])->mParent = NULL;
        mChildren->Set(i, NULL);
    }
}

ECode ViewGroup::InvalidateChild(
    /* [in] */ IView* child,
    /* [in] */ IRect* dirty)
{
    assert(child != NULL);
    View* v = ((View*)child);

    AutoPtr<IViewParent> parent = this;

    AttachInfo* attachInfo = mAttachInfo;
    if (attachInfo != NULL) {
        // If the child is drawing an animation, we want to copy this flag onto
        // ourselves and the parent to make sure the invalidate request goes
        // through
        Boolean drawAnimation = (v->mPrivateFlags & PFLAG_DRAW_ANIMATION)
                == PFLAG_DRAW_ANIMATION;

        // Check whether the child that requests the invalidate is fully opaque
        // Views being animated or transformed are not considered opaque because we may
        // be invalidating their old position and need the parent to paint behind them.
        AutoPtr<IMatrix> childMatrix;
        v->GetMatrix((IMatrix**)&childMatrix);
        Boolean isIdentity = FALSE;
        childMatrix->IsIdentity(&isIdentity);
        Boolean isOpaque;
        v->IsOpaque(&isOpaque);
        AutoPtr<IAnimation> animation;
        v->GetAnimation((IAnimation**)&animation);
        isOpaque = isOpaque && !drawAnimation && animation == NULL && isIdentity;
        // Mark the child as dirty, using the appropriate flag
        // Make sure we do not set both flags at the same time
        Int32 opaqueFlag = isOpaque ? PFLAG_DIRTY_OPAQUE : PFLAG_DIRTY;

        if (v->mLayerType != IView::LAYER_TYPE_NONE) {
            mPrivateFlags |= PFLAG_INVALIDATED;
            mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
        }

        AutoPtr<ArrayOf<Int32> > location = ArrayOf<Int32>::Alloc(2);
        (*location)[CHILD_LEFT_INDEX] = v->mLeft;
        (*location)[CHILD_TOP_INDEX] = v->mTop;
        if (!isIdentity || (mGroupFlags & FLAG_SUPPORT_STATIC_TRANSFORMATIONS) != 0) {
            AutoPtr<IRectF> boundingRect = attachInfo->mTmpTransformRect;
            boundingRect->Set(dirty);
            AutoPtr<IMatrix> transformMatrix;
            if ((mGroupFlags & FLAG_SUPPORT_STATIC_TRANSFORMATIONS) != 0) {
                AutoPtr<ITransformation> t = attachInfo->mTmpTransformation;
                Boolean transformed = GetChildStaticTransformation(child, t);
                if (transformed) {
                    transformMatrix = attachInfo->mTmpMatrix;

                    AutoPtr<IMatrix> tmpMatrix;
                    t->GetMatrix((IMatrix**)&tmpMatrix);

                    transformMatrix->Set(tmpMatrix);
                    if (!isIdentity) {
                        Boolean res;
                        transformMatrix->PreConcat(childMatrix, &res);
                    }
                }
                else {
                    transformMatrix = childMatrix;
                }
            }
            else {
                transformMatrix = childMatrix;
            }

            Boolean res;
            transformMatrix->MapRect(boundingRect, &res);
            Float l, r, t, b;
            boundingRect->Get(&l, &t, &r, &b);
            dirty->Set(
                (Int32)(l - 0.5f), (Int32)(t - 0.5f),
                (Int32)(r + 0.5f), (Int32)(b + 0.5f));
        }

        IView* viewObj;
        View* view = NULL;
        IViewRootImpl* vriObj;
        ViewRootImpl* vri;
        do {
            view = NULL;
            viewObj = IView::Probe(parent);
            if (viewObj != NULL) {
                view = (View*)viewObj;
            }

            if (drawAnimation) {
                if (view != NULL) {
                    view->mPrivateFlags |= PFLAG_DRAW_ANIMATION;
                }
                else if ((vriObj = IViewRootImpl::Probe(parent)) != NULL) {
                    vri = (ViewRootImpl*)vriObj;
                    vri->mIsAnimating = TRUE;
                }
            }

            // If the parent is dirty opaque or not dirty, mark it dirty with the opaque
            // flag coming from the child that initiated the invalidate
            if (view != NULL) {
                Int32 solidColor;
                view->GetSolidColor(&solidColor);
                if ((view->mViewFlags & FADING_EDGE_MASK) != 0 && solidColor == 0) {
                    opaqueFlag = PFLAG_DIRTY;
                }
                if ((view->mPrivateFlags & PFLAG_DIRTY_MASK) != PFLAG_DIRTY) {
                    view->mPrivateFlags = (view->mPrivateFlags & ~PFLAG_DIRTY_MASK) | opaqueFlag;
                }
            }

            AutoPtr<IViewParent> tempParent;
            parent->InvalidateChildInParent(location, dirty, (IViewParent**)&tempParent);
            parent = tempParent;
            if (view != NULL) {
                // Account for transform on current parent
                AutoPtr<IMatrix> m;
                view->GetMatrix((IMatrix**)&m);
                m->IsIdentity(&isIdentity);
                if (!isIdentity) {
                    AutoPtr<IRectF> boundingRect = attachInfo->mTmpTransformRect;
                    boundingRect->Set(dirty);
                    Boolean res;
                    m->MapRect(boundingRect, &res);

                    Float l, r, t, b;
                    boundingRect->Get(&l, &t, &r, &b);
                    dirty->Set(
                        (Int32)(l - 0.5f), (Int32)(t - 0.5f),
                        (Int32)(r + 0.5f), (Int32)(b + 0.5f));
                }
            }
        } while (parent != NULL);

        attachInfo->mInvalidateChildLocation[0] = (*location)[0];
        attachInfo->mInvalidateChildLocation[1] = (*location)[1];
    }

    return NOERROR;
}

ECode ViewGroup::InvalidateChildInParent(
    /* [in] */ ArrayOf<Int32>* location,
    /* [in] */ IRect* dirty,
    /* [out] */ IViewParent** parent)
{
    VALIDATE_NOT_NULL(parent);
    if ((mPrivateFlags & PFLAG_DRAWN) == PFLAG_DRAWN ||
            (mPrivateFlags & PFLAG_DRAWING_CACHE_VALID) == PFLAG_DRAWING_CACHE_VALID) {
        if ((mGroupFlags & (FLAG_OPTIMIZE_INVALIDATE | FLAG_ANIMATION_DONE)) !=
                    FLAG_OPTIMIZE_INVALIDATE) {
            dirty->Offset((*location)[CHILD_LEFT_INDEX] - mScrollX,
                    (*location)[CHILD_TOP_INDEX] - mScrollY);

            if ((mGroupFlags & FLAG_CLIP_CHILDREN) == 0) {
                dirty->Union(0, 0, mRight - mLeft, mBottom - mTop);
            }

            const Int32 left = mLeft;
            const Int32 top = mTop;

            if ((mGroupFlags & FLAG_CLIP_CHILDREN) == FLAG_CLIP_CHILDREN) {
                Boolean tmp = FALSE;
                if (!(dirty->Intersect(0, 0, mRight - left, mBottom - top, &tmp), tmp)) {
                    dirty->SetEmpty();
                }
            }

            mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;

            (*location)[CHILD_LEFT_INDEX] = left;
            (*location)[CHILD_TOP_INDEX] = top;

            if (mLayerType != IView::LAYER_TYPE_NONE) {
                mPrivateFlags |= PFLAG_INVALIDATED;
            }

            *parent = mParent;
            REFCOUNT_ADD(*parent);
            return NOERROR;
        }
        else {
            mPrivateFlags &= ~PFLAG_DRAWN & ~PFLAG_DRAWING_CACHE_VALID;

            (*location)[CHILD_LEFT_INDEX] = mLeft;
            (*location)[CHILD_TOP_INDEX] = mTop;
            if ((mGroupFlags & FLAG_CLIP_CHILDREN) == FLAG_CLIP_CHILDREN) {
                dirty->Set(0, 0, mRight - mLeft, mBottom - mTop);
            }
            else {
                // in case the dirty rect extends outside the bounds of this container
                dirty->Union(0, 0, mRight - mLeft, mBottom - mTop);
            }

            if (mLayerType != IView::LAYER_TYPE_NONE) {
                mPrivateFlags |= PFLAG_INVALIDATED;
            }

            *parent = mParent;
            REFCOUNT_ADD(*parent);
            return NOERROR;
        }
    }

    *parent = NULL;
    return NOERROR;
}

/**
 * Native-calculated damage path
 * Returns false if this path was unable to complete successfully. This means
 * it hit a ViewParent it doesn't recognize and needs to fall back to calculating
 * damage area
 * @hide
 */
ECode ViewGroup::DamageChildDeferred(
    /* [in] */ IView* child,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IViewParent> parent, temp;
    IViewRootImpl* vriObj;
    GetParent((IViewParent**)&parent);
    while (parent != NULL) {
        if (IViewGroup::Probe(parent)) {
            temp = parent;
            parent = NULL;
            temp->GetParent((IViewParent**)&parent);
        }
        else if ((vriObj = IViewRootImpl::Probe(parent)) != NULL) {
            ViewRootImpl* vri = (ViewRootImpl*)vriObj;
            vri->Invalidate();
            *res = TRUE;
            return NOERROR;
        }
        else {
            parent = NULL;
        }
    }
    *res = FALSE;
    return NOERROR;
}

ECode ViewGroup::DamageChild(
    /* [in] */ IView* child,
    /* [in] */ IRect* dirty)
{
    assert(child != NULL);
    Boolean damageChildDeferred;
    DamageChildDeferred(child, &damageChildDeferred);
    if (damageChildDeferred) {
        return NOERROR;
    }
    AutoPtr<IViewParent> parent = this;

    AttachInfo* attachInfo = mAttachInfo;
    View* v = ((View*)child);
    if (attachInfo != NULL) {

        Int32 left = v->mLeft;
        Int32 top = v->mTop;
        AutoPtr<IMatrix> tmpMatrix;
        v->GetMatrix((IMatrix**)&tmpMatrix);
        Boolean identity = FALSE;
        tmpMatrix->IsIdentity(&identity);
        if (!identity) {
            v->TransformRect(dirty);
        }

        IViewGroup* vgObj;
        do {
            vgObj = IViewGroup::Probe(parent);
            if (vgObj != NULL) {
                ViewGroup* parentVG = (ViewGroup*)vgObj;
                if (parentVG->mLayerType != IView::LAYER_TYPE_NONE) {
                    // Layered parents should be recreated, not just re-issued
                    parentVG->Invalidate();
                    parent = NULL;
                }
                else {
                    parent = parentVG->DamageChildInParent(left, top, dirty);
                    left = parentVG->mLeft;
                    top = parentVG->mTop;
                }
            }
            else {
                // Reached the top; this calls into the usual invalidate method in
                // ViewRootImpl, which schedules a traversal
                AutoPtr<ArrayOf<Int32> > location = ArrayOf<Int32>::Alloc(2);

                (*location)[0] = left;
                (*location)[1] = top;
                AutoPtr<IViewParent> tempParent;
                parent->InvalidateChildInParent(location, dirty, (IViewParent**)&tempParent);
                parent = tempParent;
                attachInfo->mInvalidateChildLocation[0] = (*location)[0];
                attachInfo->mInvalidateChildLocation[1] = (*location)[1];
            }
        } while (parent != NULL);
    }

    return NOERROR;
}

AutoPtr<IViewParent> ViewGroup::DamageChildInParent(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ IRect* dirty)
{
    if ((mPrivateFlags & PFLAG_DRAWN) != 0 ||
            (mPrivateFlags & PFLAG_DRAWING_CACHE_VALID) != 0) {
        dirty->Offset(left - mScrollX, top - mScrollY);

        if ((mGroupFlags & FLAG_CLIP_CHILDREN) == 0) {
            dirty->Union(0, 0, mRight - mLeft, mBottom - mTop);
        }

        Boolean isIntersect = FALSE;
        if ((mGroupFlags & FLAG_CLIP_CHILDREN) == 0 ||
            (dirty->Intersect(0, 0, mRight - mLeft, mBottom - mTop, &isIntersect), isIntersect)) {

            AutoPtr<IMatrix> tmpMatrix;
            GetMatrix((IMatrix**)&tmpMatrix);
            assert(tmpMatrix != NULL);
            Boolean identity = FALSE;
            tmpMatrix->IsIdentity(&identity);
            if (!identity) {
                TransformRect(dirty);
            }

            return mParent;
        }
    }

    return NULL;
}

ECode ViewGroup::OffsetDescendantRectToMyCoords(
    /* [in] */ IView* descendant,
    /* [in] */ IRect* rect)
{
    return OffsetRectBetweenParentAndChild(descendant, rect, TRUE, FALSE);
}

ECode ViewGroup::OffsetRectIntoDescendantCoords(
    /* [in] */ IView* descendant,
    /* [in] */ IRect* rect)
{
    return OffsetRectBetweenParentAndChild(descendant, rect, FALSE, FALSE);
}

ECode ViewGroup::OffsetRectBetweenParentAndChild(
    /* [in] */ IView* descendant,
    /* [in] */ IRect* rect,
    /* [in] */ Boolean offsetFromChildToParent,
    /* [in] */ Boolean clipToBounds)
{
    assert(descendant != NULL);

    // already in the same coord system :)
    if (descendant == this) {
        return NOERROR;
    }

    View* v = VIEW_PROBE(descendant);
    AutoPtr<IViewParent> theParent = v->mParent;
    IViewParent* thisParent = this;
    Boolean result;

    // search and offset up to the parent
    while ((theParent != NULL)
            && (VIEW_PROBE(theParent) != NULL)
            && (theParent.Get() != thisParent)) {

        if (offsetFromChildToParent) {
            rect->Offset(v->mLeft - v->mScrollX, v->mTop - v->mScrollY);
            if (clipToBounds) {
                View* p = VIEW_PROBE(theParent);
                rect->Intersect(0, 0, p->mRight - p->mLeft, p->mBottom - p->mTop, &result);
            }
        }
        else {
            if (clipToBounds) {
                View* p = VIEW_PROBE(theParent);
                rect->Intersect(0, 0, p->mRight - p->mLeft, p->mBottom - p->mTop, &result);
            }
            rect->Offset(v->mScrollX - v->mLeft, v->mScrollY - v->mTop);
        }

        v = VIEW_PROBE(theParent);
        theParent = v->mParent;
    }

    // now that we are up to this view, need to offset one more time
    // to get into our coordinate space
    if (theParent.Get() == thisParent) {
        if (offsetFromChildToParent) {
            rect->Offset(v->mLeft - v->mScrollX, v->mTop - v->mScrollY);
        }
        else {
            rect->Offset(v->mScrollX - v->mLeft, v->mScrollY - v->mTop);
        }
    }
    else {
        Slogger::E(TAG, "parameter must be a descendant of this view");
//        throw new IllegalArgumentException("parameter must be a descendant of this view");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode ViewGroup::OffsetChildrenTopAndBottom(
    /* [in] */ Int32 offset)
{
    Boolean invalidate = FALSE;
    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* v = (View*)(*mChildren)[i];
        v->mTop += offset;
        v->mBottom += offset;
        if (v->mRenderNode != NULL) {
            invalidate = TRUE;
            Boolean res;
            v->mRenderNode->OffsetTopAndBottom(offset, &res);
        }
    }

    if (invalidate) {
        InvalidateViewProperty(FALSE, FALSE);
    }
    NotifySubtreeAccessibilityStateChangedIfNeeded();

    return NOERROR;
}

ECode ViewGroup::GetChildVisibleRect(
    /* [in] */ IView* child,
    /* [in] */ IRect* r,
    /* [in] */ IPoint* offset,
    /* [out] */ Boolean* result)
{
    // It doesn't make a whole lot of sense to call this on a view that isn't attached,
    // but for some simple tests it can be useful. If we don't have attach info this
    // will allocate memory.
    AutoPtr<IRectF> rect;
    if (mAttachInfo != NULL) {
        rect = mAttachInfo->mTmpTransformRect;
    }
    else {
        CRectF::New((IRectF**)&rect);
    }

    rect->Set(r);

    assert(child != NULL && result != NULL);
    View* _child = ((View*)child);
    if (!_child->HasIdentityMatrix()) {
        AutoPtr<IMatrix> tempMatrix;
        _child->GetMatrix((IMatrix**)&tempMatrix);
        Boolean res;
        tempMatrix->MapRect(rect, &res);
    }

    Int32 dx = _child->mLeft - mScrollX;
    Int32 dy = _child->mTop - mScrollY;

    rect->Offset(dx, dy);

    if (offset != NULL) {
        if (!_child->HasIdentityMatrix()) {
            AutoPtr<ArrayOf<Float> > position = ArrayOf<Float>::Alloc(2);
            Int32 x, y;
            offset->GetX(&x);
            offset->GetY(&y);
            (*position)[0] = x;
            (*position)[1] = y;

            AutoPtr<IMatrix> tempMatrix;
            _child->GetMatrix((IMatrix**)&tempMatrix);
            tempMatrix->MapPoints(position);
            offset->Set((Int32)((*position)[0] + 0.5f), (Int32)((*position)[1] + 0.5f));

            if (mAttachInfo != NULL) {
                (*mAttachInfo->mTmpTransformLocation)[0] = (*position)[0];
                (*mAttachInfo->mTmpTransformLocation)[1] = (*position)[1];
            }
        }
        offset->Offset(dx, dy);
    }

    Boolean tmp = FALSE;
    if ((rect->Intersect(0, 0, mRight - mLeft, mBottom - mTop, &tmp), tmp)) {
        if (mParent == NULL) {
            *result = FALSE;
            return NOERROR;
        }

        Float l, rr, t, b;
        rect->Get(&l, &t, &rr, &b);
        r->Set((Int32)(l + 0.5f), (Int32)(t + 0.5f),
            (Int32)(rr + 0.5f), (Int32)(b + 0.5f));

        return mParent->GetChildVisibleRect(this, r, offset, result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode ViewGroup::Layout(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Boolean changing = FALSE;
    if (!mSuppressLayout && (mTransition == NULL || !(mTransition->IsChangingLayout(&changing), changing))) {
        if (mTransition != NULL) {
            mTransition->LayoutChange(this);
        }

        View::Layout(l, t, r, b);
    }
    else {
        // record the fact that we noop'd it; request layout when transition finishes
        mLayoutCalledWhileSuppressed = TRUE;
    }

    return NOERROR;
}

Boolean ViewGroup::CanAnimate()
{
    return mLayoutAnimationController != NULL;
}

ECode ViewGroup::StartLayoutAnimation()
{
    if (mLayoutAnimationController != NULL) {
        mGroupFlags |= FLAG_RUN_ANIMATION;
        RequestLayout();
    }
    return NOERROR;
}

ECode ViewGroup::ScheduleLayoutAnimation()
{
    mGroupFlags |= FLAG_RUN_ANIMATION;
    return NOERROR;
}

ECode ViewGroup::SetLayoutAnimation(
    /* [in] */ ILayoutAnimationController* controller)
{
    mLayoutAnimationController = controller;
    if (mLayoutAnimationController != NULL) {
        mGroupFlags |= FLAG_RUN_ANIMATION;
    }
    return NOERROR;
}

ECode ViewGroup::GetLayoutAnimation(
    /* [out] */ ILayoutAnimationController** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mLayoutAnimationController;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode ViewGroup::IsAnimationCacheEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = (mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE;
    return NOERROR;
}

ECode ViewGroup::SetAnimationCacheEnabled(
    /* [in] */ Boolean enabled)
{
    SetBooleanFlag(FLAG_ANIMATION_CACHE, enabled);
    return NOERROR;
}

ECode ViewGroup::IsAlwaysDrawnWithCacheEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = (mGroupFlags & FLAG_ALWAYS_DRAWN_WITH_CACHE) == FLAG_ALWAYS_DRAWN_WITH_CACHE;
    return NOERROR;
}

ECode ViewGroup::SetAlwaysDrawnWithCacheEnabled(
    /* [in] */ Boolean always)
{
    SetBooleanFlag(FLAG_ALWAYS_DRAWN_WITH_CACHE, always);
    return NOERROR;
}

Boolean ViewGroup::IsChildrenDrawnWithCacheEnabled()
{
    return (mGroupFlags & FLAG_CHILDREN_DRAWN_WITH_CACHE) == FLAG_CHILDREN_DRAWN_WITH_CACHE;
}

void ViewGroup::SetChildrenDrawnWithCacheEnabled(
    /* [in] */ Boolean enabled)
{
    SetBooleanFlag(FLAG_CHILDREN_DRAWN_WITH_CACHE, enabled);
}

Boolean ViewGroup::IsChildrenDrawingOrderEnabled()
{
    return (mGroupFlags & FLAG_USE_CHILD_DRAWING_ORDER) == FLAG_USE_CHILD_DRAWING_ORDER;
}

void ViewGroup::SetChildrenDrawingOrderEnabled(
    /* [in] */ Boolean enabled)
{
    SetBooleanFlag(FLAG_USE_CHILD_DRAWING_ORDER, enabled);
}

Boolean ViewGroup::HasBooleanFlag(
    /* [in] */ Int32 flag)
{
    return (mGroupFlags & flag) == flag;
}

void ViewGroup::SetBooleanFlag(
    /* [in] */ Int32 flag,
    /* [in] */ Boolean value)
{
    if (value) {
        mGroupFlags |= flag;
    }
    else {
        mGroupFlags &= ~flag;
    }
}

ECode ViewGroup::GetPersistentDrawingCache(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mPersistentDrawingCache;
    return NOERROR;
}

ECode ViewGroup::SetPersistentDrawingCache(
    /* [in] */ Int32 drawingCacheToKeep)
{
    mPersistentDrawingCache = drawingCacheToKeep & PERSISTENT_ALL_CACHES;
    return NOERROR;
}

void ViewGroup::SetLayoutMode(
    /* [in] */ Int32 layoutMode,
    /* [in] */ Boolean explicitly)
{
    mLayoutMode = layoutMode;
    SetBooleanFlag(FLAG_LAYOUT_MODE_WAS_EXPLICITLY_SET, explicitly);
}

/**
 * Recursively traverse the view hierarchy, resetting the layoutMode of any
 * descendants that had inherited a different layoutMode from a previous parent.
 * Recursion terminates when a descendant's mode is:
 * <ul>
 *     <li>Undefined</li>
 *     <li>The same as the root node's</li>
 *     <li>A mode that had been explicitly set</li>
 * <ul/>
 * The first two clauses are optimizations.
 * @param layoutModeOfRoot
 */
//@Override
void ViewGroup::InvalidateInheritedLayoutMode(
    /* [in] */ Int32 layoutModeOfRoot)
{
    if (mLayoutMode == LAYOUT_MODE_UNDEFINED ||
        mLayoutMode == layoutModeOfRoot ||
        HasBooleanFlag(FLAG_LAYOUT_MODE_WAS_EXPLICITLY_SET)) {
        return;
    }
    SetLayoutMode(LAYOUT_MODE_UNDEFINED, FALSE);

    // apply recursively
    Int32 childrenCount;
    GetChildCount(&childrenCount);
    for (Int32 i = 0; i < childrenCount; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        VIEW_PROBE(temp)->InvalidateInheritedLayoutMode(layoutModeOfRoot);
    }
}

/**
 * Returns the basis of alignment during layout operations on this ViewGroup:
 * either {@link #LAYOUT_MODE_CLIP_BOUNDS} or {@link #LAYOUT_MODE_OPTICAL_BOUNDS}.
 * <p>
 * If no layoutMode was explicitly set, either programmatically or in an XML resource,
 * the method returns the layoutMode of the view's parent ViewGroup if such a parent exists,
 * otherwise the method returns a default value of {@link #LAYOUT_MODE_CLIP_BOUNDS}.
 *
 * @return the layout mode to use during layout operations
 *
 * @see #setLayoutMode(int)
 */
ECode ViewGroup::GetLayoutMode(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    if (mLayoutMode == LAYOUT_MODE_UNDEFINED) {
        Int32 inheritedLayoutMode = LAYOUT_MODE_DEFAULT;
        if (IViewGroup::Probe(mParent)) {
            (IViewGroup::Probe(mParent))->GetLayoutMode(&inheritedLayoutMode);
        }
        SetLayoutMode(inheritedLayoutMode, FALSE);
    }
    *res = mLayoutMode;
    return NOERROR;
}

ECode ViewGroup::SetLayoutMode(
    /* [in] */ Int32 layoutMode)
{
    if (mLayoutMode != layoutMode) {
        InvalidateInheritedLayoutMode(layoutMode);
        SetLayoutMode(layoutMode, layoutMode != LAYOUT_MODE_UNDEFINED);
        RequestLayout();
    }

    return NOERROR;
}

ECode ViewGroup::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IViewGroupLayoutParams> temp;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CViewGroupLayoutParams::New(ctx, attrs, (IViewGroupLayoutParams**)&temp);
    *params = temp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> ViewGroup::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return AutoPtr<IViewGroupLayoutParams>(p);
}

/**
 * Returns a set of default layout parameters. These parameters are requested
 * when the View passed to {@link #addView(View)} has no layout parameters
 * already set. If NULL is returned, an exception is thrown from addView.
 *
 * @return a set of default layout parameters or NULL
 */
ECode ViewGroup::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IViewGroupLayoutParams> temp;
    CViewGroupLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&temp);
    *params = temp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
void ViewGroup::Debug(
    /* [in] */ Int32 depth)
{
    View::Debug(depth);
    String output;

    if (mFocused != NULL) {
        output = DebugIndent(depth);
        output += "mFocused";
        Logger::D(TAG, "%s", output.string());
    }
    if (mChildrenCount != 0) {
        output = DebugIndent(depth);
        output += "{";
        Logger::D(TAG, "%s", output.string());
    }
    Int32 count = mChildrenCount;
    for (Int32 i = 0; i < count; i++) {
        View* child = (View*)(*mChildren)[i];
        child->Debug(depth + 1);
    }

    if (mChildrenCount != 0) {
        output = DebugIndent(depth);
        output += "}";
        Logger::D(TAG, "%s", output.string());
    }
}

/**
  * Returns the position in the group of the specified child view.
  *
  * @param child the view for which to get the position
  * @return a positive integer representing the position of the view in the
  *         group, or -1 if the view does not exist in the group
  */
ECode ViewGroup::IndexOfChild(
    /* [in] */ IView* child,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    for (Int32 i = 0; i < mChildrenCount; i++) {
        if ((*mChildren)[i] == child) {
            *res = i;
            return NOERROR;
        }
    }

    *res = -1;
    return NOERROR;
}

/**
 * Returns the number of children in the group.
 *
 * @return a positive integer representing the number of children in
 *         the group
 */
ECode ViewGroup::GetChildCount(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mChildrenCount;
    return NOERROR;
}

/**
 * Returns the view at the specified position in the group.
 *
 * @param index the position at which to get the view from
 * @return the view at the specified position or NULL if the position
 *         does not exist within the group
 */
ECode ViewGroup::GetChildAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res)
    if (index < 0 || index >= mChildrenCount) {
        *res = NULL;
        return NOERROR;
    }
    *res = (*mChildren)[index];
    REFCOUNT_ADD(*res)
    return NOERROR;
}

void ViewGroup::MeasureChildren(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* v = (View*)(*mChildren)[i];
        if ((v->mViewFlags & VISIBILITY_MASK) != IView::GONE) {
            MeasureChild((*mChildren)[i], widthMeasureSpec, heightMeasureSpec);
        }
    }
}

void ViewGroup::MeasureChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthMeasureSpec,
    /* [in] */ Int32 parentHeightMeasureSpec)
{
    assert(child != NULL);

    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 width, height;
    lp->GetWidth(&width);
    lp->GetHeight(&height);

    const Int32 childWidthMeasureSpec = GetChildMeasureSpec(parentWidthMeasureSpec,
            mPaddingLeft + mPaddingRight, width);
    const Int32 childHeightMeasureSpec = GetChildMeasureSpec(parentHeightMeasureSpec,
            mPaddingTop + mPaddingBottom, height);

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

/**
 * Ask one of the children of this view to measure itself, taking into
 * account both the MeasureSpec requirements for this view and its padding
 * and margins. The child must have MarginLayoutParams The heavy lifting is
 * done in getChildMeasureSpec.
 *
 * @param child The child to measure
 * @param parentWidthMeasureSpec The width requirements for this view
 * @param widthUsed Extra space that has been used up by the parent
 *        horizontally (possibly by other children of the parent)
 * @param parentHeightMeasureSpec The height requirements for this view
 * @param heightUsed Extra space that has been used up by the parent
 *        vertically (possibly by other children of the parent)
 */
void ViewGroup::MeasureChildWithMargins(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthMeasureSpec,
    /* [in] */ Int32 widthUsed,
    /* [in] */ Int32 parentHeightMeasureSpec,
    /* [in] */ Int32 heightUsed)
{
    AutoPtr<IViewGroupLayoutParams> lv;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lv);

    IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(lv);

    Int32 width, height, left, top, right, bottom;
    IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);
    IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);
    lp->GetMargins(&left, &top, &right, &bottom);

    Int32 childWidthMeasureSpec = GetChildMeasureSpec(parentWidthMeasureSpec,
            mPaddingLeft + mPaddingRight + left + right + widthUsed, width);
    Int32 childHeightMeasureSpec = GetChildMeasureSpec(parentHeightMeasureSpec,
            mPaddingTop + mPaddingBottom + top + bottom + heightUsed, height);

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

/**
 * Does the hard part of measureChildren: figuring out the MeasureSpec to
 * pass to a particular child. This method figures out the right MeasureSpec
 * for one dimension (height or width) of one child view.
 *
 * The goal is to combine information from our MeasureSpec with the
 * LayoutParams of the child to get the best possible results. For example,
 * if the this view knows its size (because its MeasureSpec has a mode of
 * EXACTLY), and the child has indicated in its LayoutParams that it wants
 * to be the same size as the parent, the parent should ask the child to
 * layout given an exact size.
 *
 * @param spec The requirements for this view
 * @param padding The padding of this view for the current dimension and
 *        margins, if applicable
 * @param childDimension How big the child wants to be in the current
 *        dimension
 * @return a MeasureSpec integer for the child
 */
Int32 ViewGroup::GetChildMeasureSpec(
    /* [in] */ Int32 spec,
    /* [in] */ Int32 padding,
    /* [in] */ Int32 childDimension)
{
    Int32 specMode = MeasureSpec::GetMode(spec);
    Int32 specSize = MeasureSpec::GetSize(spec);

    Int32 size = Elastos::Core::Math::Max(0, specSize - padding);

    Int32 resultSize = 0;
    Int32 resultMode = 0;

    switch (specMode) {
    // Parent has imposed an exact size on us
    case MeasureSpec::EXACTLY:
        if (childDimension >= 0) {
            resultSize = childDimension;
            resultMode = MeasureSpec::EXACTLY;
        }
        else if (childDimension == IViewGroupLayoutParams::MATCH_PARENT) {
            // Child wants to be our size. So be it.
            resultSize = size;
            resultMode = MeasureSpec::EXACTLY;
        }
        else if (childDimension == IViewGroupLayoutParams::WRAP_CONTENT) {
            // Child wants to determine its own size. It can't be
            // bigger than us.
            resultSize = size;
            resultMode = MeasureSpec::AT_MOST;
        }
        break;

    // Parent has imposed a maximum size on us
    case MeasureSpec::AT_MOST:
        if (childDimension >= 0) {
            // Child wants a specific size... so be it
            resultSize = childDimension;
            resultMode = MeasureSpec::EXACTLY;
        }
        else if (childDimension == IViewGroupLayoutParams::MATCH_PARENT) {
            // Child wants to be our size, but our size is not fixed.
            // Constrain child to not be bigger than us.
            resultSize = size;
            resultMode = MeasureSpec::AT_MOST;
        }
        else if (childDimension == IViewGroupLayoutParams::WRAP_CONTENT) {
            // Child wants to determine its own size. It can't be
            // bigger than us.
            resultSize = size;
            resultMode = MeasureSpec::AT_MOST;
        }
        break;

    // Parent asked to see how big we want to be
    case MeasureSpec::UNSPECIFIED:
        if (childDimension >= 0) {
            // Child wants a specific size... let him have it
            resultSize = childDimension;
            resultMode = MeasureSpec::EXACTLY;
        }
        else if (childDimension == IViewGroupLayoutParams::MATCH_PARENT) {
            // Child wants to be our size... find out how big it should
            // be
            resultSize = 0;
            resultMode = MeasureSpec::UNSPECIFIED;
        }
        else if (childDimension == IViewGroupLayoutParams::WRAP_CONTENT) {
            // Child wants to determine its own size.... find out how
            // big it should be
            resultSize = 0;
            resultMode = MeasureSpec::UNSPECIFIED;
        }
        break;
    }
    return MeasureSpec::MakeMeasureSpec(resultSize, resultMode);
}

ECode ViewGroup::ClearDisappearingChildren()
{
    if (mDisappearingChildren != NULL) {
        Int32 count;
        mDisappearingChildren->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> temp;
            mDisappearingChildren->Get(i, (IInterface**)&temp);
            View* view = VIEW_PROBE(temp);
            if (view->mAttachInfo != NULL) {
                view->DispatchDetachedFromWindow();
            }
            view->ClearAnimation();
        }
        mDisappearingChildren->Clear();
        Invalidate();
    }
    return NOERROR;
}

void ViewGroup::AddDisappearingView(
    /* [in] */ IView* v)
{
    if (mDisappearingChildren == NULL) {
        CArrayList::New((IList**)&mDisappearingChildren);
    }
    mDisappearingChildren->Add(v);

}

void ViewGroup::FinishAnimatingView(
    /* [in] */ IView* view,
    /* [in] */ IAnimation* animation)
{
    assert(view != NULL);
    View* v = VIEW_PROBE(view);

    if (mDisappearingChildren != NULL) {
        Boolean hasElement;
        mDisappearingChildren->Contains(view, &hasElement);
        if (hasElement) {
            mDisappearingChildren->Remove(view);

            if (v->mAttachInfo != NULL) {
                v->DispatchDetachedFromWindow();
            }

            view->ClearAnimation();
            mGroupFlags |= FLAG_INVALIDATE_REQUIRED;
        }
    }


    if (animation != NULL) {
        Boolean result;
        animation->GetFillAfter(&result);
        if (!result) {
            v->ClearAnimation();
        }
    }

    if ((v->mPrivateFlags & PFLAG_ANIMATION_STARTED) == PFLAG_ANIMATION_STARTED) {
        v->OnAnimationEnd();
        // Should be performed by onAnimationEnd() but this avoid an infinite loop,
        // so we'd rather be safe than sorry
        v->mPrivateFlags &= ~PFLAG_ANIMATION_STARTED;
        // Draw one more frame after the animation is done
        mGroupFlags |= FLAG_INVALIDATE_REQUIRED;
    }
}

Boolean ViewGroup::IsViewTransitioning(
    /* [in] */ IView* view)
{
    Boolean contains = FALSE;
    List<AutoPtr<IView> >::Iterator ator = mTransitioningViews.Begin();
    for (; ator != mTransitioningViews.End(); ++ator) {
        if (ator->Get() == view) {
            contains = TRUE;
        }
    }

    return contains;
}

ECode ViewGroup::StartViewTransition(
    /* [in] */ IView* view)
{
    View* v = VIEW_PROBE(view);
    if (v->mParent == (IViewParent*)this) {
        mTransitioningViews.PushBack(view);
    }

    return NOERROR;
}

ECode ViewGroup::EndViewTransition(
    /* [in] */ IView* view)
{
    if (mTransitioningViews.GetSize() != 0) {
        mTransitioningViews.Remove(view);
        Boolean contains;
        if (mDisappearingChildren != NULL && (mDisappearingChildren->Contains(view, &contains), contains)) {
            mDisappearingChildren->Remove(view);
            if (mVisibilityChangingChildren.GetSize() != 0){
                AutoPtr<IView> param = view;
                if (Find(mVisibilityChangingChildren.Begin(), mVisibilityChangingChildren.End(), param)
                    != mVisibilityChangingChildren.End())
                {
                    mVisibilityChangingChildren.Remove(view);
                }
            } else {
                View* impl = (View*)view;
                if (impl->mAttachInfo != NULL) {
                    impl->DispatchDetachedFromWindow();
                }
                if (impl->mParent != NULL) {
                    impl->mParent = NULL;
                }
            }
            Invalidate();
        }
    }
    return NOERROR;
}

/**
 * Tells this ViewGroup to suppress all layout() calls until layout
 * suppression is disabled with a later call to suppressLayout(false).
 * When layout suppression is disabled, a requestLayout() call is sent
 * if layout() was attempted while layout was being suppressed.
 *
 * @hide
 */
ECode ViewGroup::SuppressLayout(
    /* [in] */ Boolean suppress)
{
    mSuppressLayout = suppress;
    if (!suppress) {
        if (mLayoutCalledWhileSuppressed) {
            RequestLayout();
            mLayoutCalledWhileSuppressed = FALSE;
        }
    }
    return NOERROR;
}

/**
 * Returns whether layout calls on this container are currently being
 * suppressed, due to an earlier call to {@link #suppressLayout(boolean)}.
 *
 * @return true if layout calls are currently suppressed, false otherwise.
 *
 * @hide
 */
ECode ViewGroup::IsLayoutSuppressed(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mSuppressLayout;
    return NOERROR;
}

ECode ViewGroup::GatherTransparentRegion(
    /* [in, out] */ IRegion* region,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    // If no transparent regions requested, we are always opaque.
    const Boolean meOpaque = (mPrivateFlags & View::PFLAG_REQUEST_TRANSPARENT_REGIONS) == 0;
    if (meOpaque && region == NULL) {
        // The caller doesn't care about the region, so stop now.
        *res = TRUE;
        return NOERROR;
    }
    View::GatherTransparentRegion(region, res);
    Boolean noneOfTheChildrenAreTransparent = TRUE;
    for (Int32 i = 0; i < mChildrenCount; i++) {
        View* v = (View*)(*mChildren)[i];
        AutoPtr<IAnimation> animation;
        if ((v->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE
            || (v->GetAnimation((IAnimation**)&animation), animation) != NULL) {
            Boolean transparentRegion;
            v->GatherTransparentRegion(region, &transparentRegion);
            if (!transparentRegion) {
                noneOfTheChildrenAreTransparent = FALSE;
            }
        }
    }
    *res = meOpaque || noneOfTheChildrenAreTransparent;
    return NOERROR;
}

ECode ViewGroup::RequestTransparentRegion(
    /* [in] */ IView* child)
{
    if (child != NULL) {
        View* v = ((View*)child);
        v->mPrivateFlags |= View::PFLAG_REQUEST_TRANSPARENT_REGIONS;
        if (mParent != NULL) {
            mParent->RequestTransparentRegion(this);
        }
    }
    return NOERROR;
}

ECode ViewGroup::DispatchApplyWindowInsets(
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IWindowInsets> wi;
    FAIL_RETURN(View::DispatchApplyWindowInsets(insets, (IWindowInsets**)&wi))
    Boolean isConsumed;
    if ((wi->IsConsumed(&isConsumed), !isConsumed)) {
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            AutoPtr<IWindowInsets> wiTemp = wi;
            wi = NULL;
            temp->DispatchApplyWindowInsets(wiTemp, (IWindowInsets**)&wi);
            if ((wi->IsConsumed(&isConsumed), isConsumed)) {
                break;
            }
        }
    }
    *res = wi;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode ViewGroup::GetLayoutAnimationListener(
    /* [out] */ IAnimationAnimationListener** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mAnimationListener;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode ViewGroup::DrawableStateChanged()
{
    View::DrawableStateChanged();

    if ((mGroupFlags & FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE) != 0) {
        if ((mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) != 0) {
            SLOGGERE("ViewGroup", "addStateFromChildren cannot be enabled if a" \
                " child has duplicateParentState set to TRUE");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        for (Int32 i = 0; i < mChildrenCount; i++) {
            View* v = (View*)(*mChildren)[i];
            if ((v->mViewFlags & DUPLICATE_PARENT_STATE) != 0) {
                v->RefreshDrawableState();
            }
        }
    }
    return NOERROR;
}

ECode ViewGroup::JumpDrawablesToCurrentState()
{
    View::JumpDrawablesToCurrentState();
    for (Int32 i = 0; i < mChildrenCount; i++) {
        (*mChildren)[i]->JumpDrawablesToCurrentState();
    }

    return NOERROR;
}

//@Override
ECode ViewGroup::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    View::DrawableHotspotChanged(x, y);

    if ((mGroupFlags & FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE) != 0) {
        if ((mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) != 0) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        for (Int32 i = 0; i < mChildrenCount; i++) {
            View* child = (View*)(*mChildren)[i];
            if ((child->mViewFlags & DUPLICATE_PARENT_STATE) != 0) {
                child->DrawableHotspotChanged(x, y);
            }
        }
    }
    return NOERROR;
}

ECode ViewGroup::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState);
    *drawableState = NULL;

    if ((mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) == 0) {
        return View::OnCreateDrawableState(extraSpace, drawableState);
    }

    Int32 need = 0;
    Int32 n = 0;
    GetChildCount(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* v = VIEW_PROBE(temp);
        AutoPtr<ArrayOf<Int32> > childState;
        v->GetDrawableState((ArrayOf<Int32>**)&childState);

        if (childState != NULL) {
            need += childState->GetLength();
        }
    }

    AutoPtr<ArrayOf<Int32> > state;
    ASSERT_SUCCEEDED(View::OnCreateDrawableState(extraSpace + need, (ArrayOf<Int32>**)&state));

    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* v = VIEW_PROBE(temp);
        AutoPtr<ArrayOf<Int32> > childState;
        v->GetDrawableState((ArrayOf<Int32>**)&childState);

        if (childState != NULL) {
            MergeDrawableStates(state, childState);
        }
    }

    *drawableState = state;
    REFCOUNT_ADD(*drawableState);
    return NOERROR;
}

ECode ViewGroup::SetAddStatesFromChildren(
    /* [in] */ Boolean addsStates)
{
    if (addsStates) {
        mGroupFlags |= FLAG_ADD_STATES_FROM_CHILDREN;
    }
    else {
        mGroupFlags &= ~FLAG_ADD_STATES_FROM_CHILDREN;
    }

    return RefreshDrawableState();
}

ECode ViewGroup::AddStatesFromChildren(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = (mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) != 0;
    return NOERROR;
}

ECode ViewGroup::ChildDrawableStateChanged(
    /* [in] */ IView* child)
{
    if ((mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) != 0) {
        return RefreshDrawableState();
    }
    return NOERROR;
}

ECode ViewGroup::SetLayoutAnimationListener(
    /* [in] */ IAnimationAnimationListener* animationListener)
{
    mAnimationListener = animationListener;
    return NOERROR;
}

ECode ViewGroup::RequestTransitionStart(
    /* [in] */ ILayoutTransition* transition)
{
    AutoPtr<IViewRootImpl> viewAncestor;
    GetViewRootImpl((IViewRootImpl**)&viewAncestor);
    if (viewAncestor != NULL) {
        viewAncestor->RequestTransitionStart(transition);
    }

    return NOERROR;
}

ECode ViewGroup::ResolveRtlPropertiesIfNeeded(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    View::ResolveRtlPropertiesIfNeeded(res);
    if (*res) {
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            View* child = VIEW_PROBE(temp);
            Boolean isLayoutDirectionInherited;
            child->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
            if (isLayoutDirectionInherited) {
                child->ResolveRtlPropertiesIfNeeded(&isLayoutDirectionInherited);
            }
        }
    }
    return NOERROR;
}

ECode ViewGroup::ResolveLayoutDirection(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean result;
    View::ResolveLayoutDirection(&result);
    if (result) {
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            View* child = VIEW_PROBE(temp);
            Boolean isLayoutDirectionInherited;
            child->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
            if (isLayoutDirectionInherited) {
                child->ResolveLayoutDirection(&isLayoutDirectionInherited);
            }
        }
    }

    *res = result;
    return NOERROR;
}

ECode ViewGroup::ResolveTextDirection(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean result;
    View::ResolveTextDirection(&result);
    if (result) {
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            View* child = VIEW_PROBE(temp);
            Boolean isTextDirectionInherited;
            child->IsTextDirectionInherited(&isTextDirectionInherited);
            if (isTextDirectionInherited) {
                child->ResolveTextDirection(&isTextDirectionInherited);
            }
        }
    }
    *res = result;
    return NOERROR;
}

ECode ViewGroup::ResolveTextAlignment(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean result;
    View::ResolveTextAlignment(&result);
    if (result) {
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> temp;
            GetChildAt(i, (IView**)&temp);
            View* child = VIEW_PROBE(temp);
            Boolean isTextAlignmentInherited;
            child->IsTextAlignmentInherited(&isTextAlignmentInherited);
            if (isTextAlignmentInherited) {
                child->ResolveTextAlignment(&isTextAlignmentInherited);
            }
        }
    }
    *res = result;
    return NOERROR;
}

ECode ViewGroup::ResolvePadding()
{
    View::ResolvePadding();
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* child = VIEW_PROBE(temp);
        Boolean isLayoutDirectionInherited;
        child->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
        if (isLayoutDirectionInherited) {
            child->ResolvePadding();
        }
    }

    return NOERROR;
}

void ViewGroup::ResolveDrawables()
{
    View::ResolveDrawables();
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* child = VIEW_PROBE(temp);
        Boolean isLayoutDirectionInherited;
        child->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
        if (isLayoutDirectionInherited) {
            child->ResolvePadding();
        }
    }
}

ECode ViewGroup::ResolveLayoutParams()
{
    View::ResolveLayoutParams();
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        temp->ResolveLayoutParams();
    }

    return NOERROR;
}

ECode ViewGroup::ResetResolvedLayoutDirection()
{
    View::ResetResolvedLayoutDirection();

    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* child = VIEW_PROBE(temp);
        Boolean isLayoutDirectionInherited;
        child->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
        if (isLayoutDirectionInherited) {
            child->ResetResolvedLayoutDirection();
        }
    }

    return NOERROR;
}

ECode ViewGroup::ResetResolvedTextDirection()
{
    View::ResetResolvedTextDirection();

    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* child = VIEW_PROBE(temp);
        Boolean isTextDirectionInherited;
        child->IsTextDirectionInherited(&isTextDirectionInherited);
        if (isTextDirectionInherited) {
            child->ResetResolvedTextDirection();
        }
    }

    return NOERROR;
}

ECode ViewGroup::ResetResolvedTextAlignment()
{
    View::ResetResolvedTextAlignment();

    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* child = VIEW_PROBE(temp);
        Boolean IsTextAlignmentInherited;
        child->IsTextAlignmentInherited(&IsTextAlignmentInherited);
        if (IsTextAlignmentInherited) {
            child->ResetResolvedTextAlignment();
        }
    }

    return NOERROR;
}

ECode ViewGroup::ResetResolvedPadding()
{
    View::ResetResolvedPadding();

    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* child = VIEW_PROBE(temp);
        Boolean isLayoutDirectionInherited;
        child->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
        if (isLayoutDirectionInherited) {
            child->ResetResolvedPadding();
        }
    }

    return NOERROR;
}

void ViewGroup::ResetResolvedDrawables()
{
    View::ResetResolvedDrawables();

    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> temp;
        GetChildAt(i, (IView**)&temp);
        View* child = VIEW_PROBE(temp);
        Boolean isLayoutDirectionInherited;
        child->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
        if (isLayoutDirectionInherited) {
            child->ResetResolvedDrawables();
        }
    }
}

ECode ViewGroup::ShouldDelayChildPressedState(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = TRUE;
    return NOERROR;
}

/**
 * @inheritDoc
 */
//@Override
ECode ViewGroup::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 nestedScrollAxes,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

/**
 * @inheritDoc
 */
//@Override
ECode ViewGroup::OnNestedScrollAccepted(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 axes)
{
    mNestedScrollAxes = axes;
    return NOERROR;
}

/**
 * @inheritDoc
 *
 * <p>The default implementation of onStopNestedScroll calls
 * {@link #stopNestedScroll()} to halt any recursive nested scrolling in progress.</p>
 */
//@Override
ECode ViewGroup::OnStopNestedScroll(
    /* [in] */ IView* child)
{
    // Stop any recursive nested scrolling.
    StopNestedScroll();
    return NOERROR;
}

/**
 * @inheritDoc
 */
//@Override
ECode ViewGroup::OnNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dxConsumed,
    /* [in] */ Int32 dyConsumed,
    /* [in] */ Int32 dxUnconsumed,
    /* [in] */ Int32 dyUnconsumed)
{
    // Do nothing
    return NOERROR;
}

/**
 * @inheritDoc
 */
//@Override
ECode ViewGroup::OnNestedPreScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ ArrayOf<Int32>* consumed)
{
    // Do nothing
    return NOERROR;
}

/**
 * @inheritDoc
 */
//@Override
ECode ViewGroup::OnNestedFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [in] */ Boolean consumed,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

/**
 * @inheritDoc
 */
//@Override
ECode ViewGroup::OnNestedPreFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

/**
 * Return the current axes of nested scrolling for this ViewGroup.
 *
 * <p>A ViewGroup returning something other than {@link #SCROLL_AXIS_NONE} is currently
 * acting as a nested scrolling parent for one or more descendant views in the hierarchy.</p>
 *
 * @return Flags indicating the current axes of nested scrolling
 * @see #SCROLL_AXIS_HORIZONTAL
 * @see #SCROLL_AXIS_VERTICAL
 * @see #SCROLL_AXIS_NONE
 */
ECode ViewGroup::GetNestedScrollAxes(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mNestedScrollAxes;
    return NOERROR;
}

void ViewGroup::OnSetLayoutParams(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
}

/** @hide */
//@Override
ECode ViewGroup::CaptureTransitioningViews(
    /* [in] */ IList* transitioningViews)
{
    Int32 visibility;
    GetVisibility(&visibility);
    if (visibility != IView::VISIBLE) {
        return NOERROR;
    }
    Boolean isTransitionGroup;
    if (IsTransitionGroup(&isTransitionGroup), isTransitionGroup) {
        transitioningViews->Add(TO_IINTERFACE(this));
    } else {
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            child->CaptureTransitioningViews(transitioningViews);
        }
    }
    return NOERROR;
}

/** @hide */
//@Override
ECode ViewGroup::FindNamedViews(
    /* [in] */ IMap* namedElements)
{
    Int32 visibility;
    GetVisibility(&visibility);
    if (visibility != IView::VISIBLE && mGhostView == NULL) {
        return NOERROR;
    }
    View::FindNamedViews(namedElements);
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->FindNamedViews(namedElements);
    }
    return NOERROR;
}

AutoPtr<IPaint> ViewGroup::GetDebugPaint()
{
    if (sDebugPaint == NULL) {
        CPaint::New((IPaint**)&sDebugPaint);
        sDebugPaint->SetAntiAlias(FALSE);
    }

    return sDebugPaint;
}

AutoPtr<ArrayOf<Float> > ViewGroup::GetDebugLines(
    /* [in] */ Int32 x1,
    /* [in] */ Int32 y1,
    /* [in] */ Int32 x2,
    /* [in] */ Int32 y2)
{
    if (sDebugLines== NULL) {
        sDebugLines = ArrayOf<Float>::Alloc(16);
    }

    x2--;
    y2--;

    (*sDebugLines)[0] = x1;
    (*sDebugLines)[1] = y1;
    (*sDebugLines)[2] = x2;
    (*sDebugLines)[3] = y1;

    (*sDebugLines)[4] = x2;
    (*sDebugLines)[5] = y1;
    (*sDebugLines)[6] = x2;
    (*sDebugLines)[7] = y2 + 1;

    (*sDebugLines)[8] = x2 + 1;
    (*sDebugLines)[9] = y2;
    (*sDebugLines)[10] = x1;
    (*sDebugLines)[11] = y2;

    (*sDebugLines)[12]  = x1;
    (*sDebugLines)[13]  = y2;
    (*sDebugLines)[14] = x1;
    (*sDebugLines)[15] = y1;

    return sDebugLines;
}

ECode ViewGroup::RequestLayout()
{
    return View::RequestLayout();
}

ECode ViewGroup::GetParent(
    /* [out] */ IViewParent** res)
{
    return View::GetParent(res);
}

ECode ViewGroup::IsLayoutRequested(
    /* [out] */ Boolean* result)
{
    return View::IsLayoutRequested(result);
}

ECode ViewGroup::CreateContextMenu(
    /* [in] */ IContextMenu* menu)
{
    return View::CreateContextMenu(menu);
}

ECode ViewGroup::RequestFitSystemWindows()
{
    return View::RequestFitSystemWindows();
}

ECode ViewGroup::GetParentForAccessibility(
    /* [out] */ IViewParent** parent)
{
    return View::GetParentForAccessibility(parent);
}

ECode ViewGroup::CanResolveLayoutDirection(
    /* [out] */ Boolean* result)
{
    return View::CanResolveLayoutDirection(result);
}

ECode ViewGroup::IsLayoutDirectionResolved(
    /* [out] */ Boolean* result)
{
    return View::IsLayoutDirectionResolved(result);
}

ECode ViewGroup::GetLayoutDirection(
    /* [out] */ Int32* direction)
{
    return View::GetLayoutDirection(direction);
}

ECode ViewGroup::CanResolveTextDirection(
    /* [out] */ Boolean* result)
{
    return View::CanResolveTextDirection(result);
}

ECode ViewGroup::IsTextDirectionResolved(
    /* [out] */ Boolean* result)
{
    return View::IsTextDirectionResolved(result);
}

ECode ViewGroup::GetTextDirection(
    /* [out] */ Int32* direction)
{
    return View::GetTextDirection(direction);
}

ECode ViewGroup::CanResolveTextAlignment(
    /* [out] */ Boolean* result)
{
    return View::CanResolveTextAlignment(result);
}

ECode ViewGroup::IsTextAlignmentResolved(
    /* [out] */ Boolean* result)
{
    return View::IsTextAlignmentResolved(result);
}

ECode ViewGroup::GetTextAlignment(
    /* [out] */ Int32* result)
{
    return View::GetTextAlignment(result);
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
