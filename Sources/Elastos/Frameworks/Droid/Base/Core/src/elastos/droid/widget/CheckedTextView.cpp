
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/CheckedTextView.h"
#include "elastos/droid/graphics/drawable/CDrawableHelper.h"
#include "elastos/droid/view/CGravity.h"

using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::IDrawableHelper;
using Elastos::Droid::Graphics::Drawable::CDrawableHelper;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

//========================================================================================
//              CheckedTextView::
//========================================================================================
AutoPtr<ArrayOf<Int32> > CheckedTextView::CHECKED_STATE_SET = ArrayOf<Int32>::Alloc(1);

CAR_INTERFACE_IMPL(CheckedTextView, TextView, ICheckedTextView)

CheckedTextView::CheckedTextView()
    : mChecked(FALSE)
    , mCheckMarkResource(0)
    , mHasCheckMarkTint(FALSE)
    , mHasCheckMarkTintMode(FALSE)
    , mBasePadding(0)
    , mCheckMarkWidth(0)
    , mCheckMarkGravity(IGravity::END)
    , mNeedRequestlayout(FALSE)
{
    (*CHECKED_STATE_SET)[0] = R::attr::state_checked;
}

ECode CheckedTextView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CheckedTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::checkedTextViewStyle);
}

ECode CheckedTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CheckedTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    TextView::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::CheckedTextView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<IDrawable> d;
    a->GetDrawable(R::styleable::CheckedTextView_checkMark, (IDrawable**)&d);
    if (d != NULL) {
        SetCheckMarkDrawable(d);
    }

    Boolean bMode = FALSE;
    if ((a->HasValue(R::styleable::CheckedTextView_checkMarkTintMode, &bMode), bMode)) {
        Int32 m = 0;
        a->GetInt32(R::styleable::CheckedTextView_checkMarkTintMode, -1, &m);
        AutoPtr<IDrawableHelper> hlp;
        CDrawableHelper::AcquireSingleton((IDrawableHelper**)&hlp);
        hlp->ParseTintMode(m, mCheckMarkTintMode, &mCheckMarkTintMode);
        mHasCheckMarkTintMode = TRUE;
    }

    Boolean bTint = FALSE;
    if ((a->HasValue(R::styleable::CheckedTextView_checkMarkTint, &bTint), bTint)) {
        a->GetColorStateList(R::styleable::CheckedTextView_checkMarkTint, (IColorStateList**)&mCheckMarkTintList);
        mHasCheckMarkTint = TRUE;
    }

    a->GetInt32(R::styleable::CheckedTextView_checkMarkGravity, IGravity::END, &mCheckMarkGravity);

    Boolean checked = FALSE;
    a->GetBoolean(R::styleable::CheckedTextView_checked, FALSE, &checked);
    SetChecked(checked);

    a->Recycle();

    ApplyCheckMarkTint();

    return NOERROR;
}

ECode CheckedTextView::Toggle()
{
    SetChecked(!mChecked);

    return NOERROR;
}

ECode CheckedTextView::IsChecked(
    /* [out] */ Boolean* isChecked)
{
    VALIDATE_NOT_NULL(isChecked)
    *isChecked = mChecked;
    return NOERROR;
}

ECode CheckedTextView::SetChecked(
    /* [in] */ Boolean checked)
{
    if (mChecked != checked) {
        mChecked = checked;
        RefreshDrawableState();
        NotifyViewAccessibilityStateChangedIfNeeded(
            IAccessibilityEvent::CONTENT_CHANGE_TYPE_UNDEFINED);
    }

    return NOERROR;
}

ECode CheckedTextView::SetCheckMarkDrawable(
    /* [in] */ Int32 resid)
{
    if (resid != 0 && resid == mCheckMarkResource) {
        return NOERROR;
    }

    mCheckMarkResource = resid;

    AutoPtr<IDrawable> d;
    if (mCheckMarkResource != 0) {
        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        cxt->GetDrawable(mCheckMarkResource, (IDrawable**)&d);
    }
    SetCheckMarkDrawable(d);

    return NOERROR;
}

ECode CheckedTextView::SetCheckMarkDrawable(
    /* [in] */ IDrawable* d)
{
    if (mCheckMarkDrawable != NULL) {
        mCheckMarkDrawable->SetCallback(NULL);
        UnscheduleDrawable(mCheckMarkDrawable);
    }
    mNeedRequestlayout = (d != mCheckMarkDrawable);
    if (d != NULL) {
        d->SetCallback(this);

        Int32 vis = 0;
        GetVisibility(&vis);
        Boolean res = FALSE;
        d->SetVisible(vis == IView::VISIBLE, FALSE, &res);

        d->SetState(CHECKED_STATE_SET, &res);

        Int32 height = 0;
        d->GetIntrinsicHeight(&height);
        SetMinHeight(height);

        d->GetIntrinsicWidth(&mCheckMarkWidth);

        AutoPtr<ArrayOf<Int32> > drawableState;
        GetDrawableState((ArrayOf<Int32>**)&drawableState);
        d->SetState(drawableState, &res);
        ApplyCheckMarkTint();
    }
    else {
        mCheckMarkWidth = 0;
    }
    mCheckMarkDrawable = d;
    // Do padding resolution. This will call internalSetPadding() and do a requestLayout() if needed.
    ResolvePadding();

    return NOERROR;
}

ECode CheckedTextView::SetCheckMarkTintList(
    /* [in] */ IColorStateList* tint)
{
    mCheckMarkTintList = tint;
    mHasCheckMarkTint = TRUE;

    ApplyCheckMarkTint();
    return NOERROR;
}

ECode CheckedTextView::GetCheckMarkTintList(
    /* [out] */ IColorStateList** tint)
{
    VALIDATE_NOT_NULL(tint)

    *tint = mCheckMarkTintList;
    REFCOUNT_ADD(*tint)
    return NOERROR;
}

ECode CheckedTextView::SetCheckMarkTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    mCheckMarkTintMode = tintMode;
    mHasCheckMarkTintMode = TRUE;

    ApplyCheckMarkTint();
    return NOERROR;
}

ECode CheckedTextView::GetCheckMarkTintMode(
    /* [out] */ PorterDuffMode* tintMode)
{
    VALIDATE_NOT_NULL(tintMode)

    *tintMode = mCheckMarkTintMode;
    return NOERROR;
}

void CheckedTextView::ApplyCheckMarkTint()
{
    if (mCheckMarkDrawable != NULL && (mHasCheckMarkTint || mHasCheckMarkTintMode)) {
        mCheckMarkDrawable->Mutate();

        if (mHasCheckMarkTint) {
            mCheckMarkDrawable->SetTintList(mCheckMarkTintList);
        }

        if (mHasCheckMarkTintMode) {
            mCheckMarkDrawable->SetTintMode(mCheckMarkTintMode);
        }
    }
}

ECode CheckedTextView::SetVisibility(
    /* [in] */ Int32 visibility)
{
    View::SetVisibility(visibility);

    if (mCheckMarkDrawable != NULL) {
        Boolean b = FALSE;
        mCheckMarkDrawable->SetVisible(visibility == VISIBLE, FALSE, &b);
    }
    return NOERROR;
}

ECode CheckedTextView::JumpDrawablesToCurrentState()
{
    View::JumpDrawablesToCurrentState();

    if (mCheckMarkDrawable != NULL) {
        mCheckMarkDrawable->JumpToCurrentState();
    }
    return NOERROR;
}

Boolean CheckedTextView::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return who == mCheckMarkDrawable || View::VerifyDrawable(who);
}

ECode CheckedTextView::GetCheckMarkDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = mCheckMarkDrawable;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

void CheckedTextView::InternalSetPadding(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    TextView::InternalSetPadding(left, top, right, bottom);
    SetBasePadding(IsCheckMarkAtStart());
}

//@Override
ECode CheckedTextView::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    TextView::OnRtlPropertiesChanged(layoutDirection);
    UpdatePadding();
    return NOERROR;
}

void CheckedTextView::UpdatePadding()
{
    ResetPaddingToInitialValues();
    Int32 newPadding = (mCheckMarkDrawable != NULL) ?
            mCheckMarkWidth + mBasePadding : mBasePadding;
    if (IsCheckMarkAtStart()) {
        mNeedRequestlayout |= (mPaddingLeft != newPadding);
        mPaddingLeft = newPadding;
    }
    else {
        mNeedRequestlayout |= (mPaddingRight != newPadding);
        mPaddingRight = newPadding;
    }
    if (mNeedRequestlayout) {
        RequestLayout();
        mNeedRequestlayout = FALSE;
    }
}

void CheckedTextView::SetBasePadding(
    /* [in] */ Boolean checkmarkAtStart)
{
    if (checkmarkAtStart) {
        mBasePadding = mPaddingLeft;
    }
    else {
        mBasePadding = mPaddingRight;
    }
}

Boolean CheckedTextView::IsCheckMarkAtStart()
{
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    AutoPtr<IGravity> gra;
    CGravity::AcquireSingleton((IGravity**)&gra);
    Int32 gravity = 0;
    gra->GetAbsoluteGravity(mCheckMarkGravity, dir, &gravity);
    Int32 hgrav = gravity & IGravity::HORIZONTAL_GRAVITY_MASK;
    return hgrav == IGravity::LEFT;
}

void CheckedTextView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    TextView::OnDraw(canvas);

    AutoPtr<IDrawable> checkMarkDrawable = mCheckMarkDrawable;
    if (checkMarkDrawable != NULL) {
        Int32 gravity = 0;
        GetGravity(&gravity);
        Int32 verticalGravity = gravity & IGravity::VERTICAL_GRAVITY_MASK;
        Int32 height = 0;
        checkMarkDrawable->GetIntrinsicHeight(&height);

        Int32 y = 0;

        switch (verticalGravity) {
            case IGravity::BOTTOM: {
                Int32 h = 0;
                GetHeight(&h);
                y = h - height;
            }
                break;
            case IGravity::CENTER_VERTICAL: {
                Int32 h = 0;
                GetHeight(&h);
                y = (h - height) / 2;
            }
                break;
        }

        Boolean checkMarkAtStart = IsCheckMarkAtStart();
        Int32 width = 0;
        GetWidth(&width);
        Int32 top = y;
        Int32 bottom = top + height;
        Int32 left = 0;
        Int32 right = 0;
        if (checkMarkAtStart) {
            left = mBasePadding;
            right = left + mCheckMarkWidth;
        }
        else {
            right = width - mBasePadding;
            left = right - mCheckMarkWidth;
        }
        checkMarkDrawable->SetBounds(mScrollX + left, top, mScrollX + right, bottom);

        checkMarkDrawable->Draw(canvas);

        AutoPtr<IDrawable> background;
        GetBackground((IDrawable**)&background);
        if (background != NULL) {
            background->SetHotspotBounds(mScrollX + left, top, mScrollX + right, bottom);
        }
    }
}

ECode CheckedTextView::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState);
    *drawableState = NULL;

    AutoPtr<ArrayOf<Int32> > ds;
    TextView::OnCreateDrawableState(extraSpace + 1, (ArrayOf<Int32>**)&ds);
    Boolean bIsChecked = FALSE;
    if ((IsChecked(&bIsChecked), bIsChecked)) {
        MergeDrawableStates(ds, CHECKED_STATE_SET);
    }
    *drawableState = ds;
    REFCOUNT_ADD(*drawableState);
    return NOERROR;
}

ECode CheckedTextView::DrawableStateChanged()
{
    TextView::DrawableStateChanged();

    if (mCheckMarkDrawable != NULL) {
        AutoPtr<ArrayOf<Int32> > myDrawableState;
        GetDrawableState((ArrayOf<Int32>**)&myDrawableState);

        Boolean res = FALSE;
        // Set the state of the Drawable
        mCheckMarkDrawable->SetState(myDrawableState, &res);

        Invalidate();
    }

    return NOERROR;
}

ECode CheckedTextView::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    TextView::DrawableHotspotChanged(x, y);

    if (mCheckMarkDrawable != NULL) {
        mCheckMarkDrawable->SetHotspot(x, y);
    }
    return NOERROR;
}

ECode CheckedTextView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    TextView::OnInitializeAccessibilityEvent(event);
    String classNameStr("CheckedTextView");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(classNameStr, (ICharSequence**)&className));
    IAccessibilityRecord::Probe(event)->SetClassName(className);

    return NOERROR;
}

ECode CheckedTextView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    TextView::OnInitializeAccessibilityNodeInfo(info);
    String classNameStr("CheckedTextView");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(classNameStr, (ICharSequence**)&className));
    info->SetClassName(className);
    info->SetCheckable(TRUE);
    info->SetChecked(mChecked);

    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
