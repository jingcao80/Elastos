
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/RippleDrawable.h"
#include "elastos/droid/graphics/drawable/CRippleDrawable.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/R.h"
#include <elastos/utility/Arrays.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_INTERFACE_IMPL(RippleDrawable::RippleState, LayerState, IRippleState);
RippleDrawable::RippleState::RippleState(
    /* [in] */ LayerState* orig,
    /* [in] */ IRippleDrawable* owner,
    /* [in] */ IResources* res)
    : LayerState(orig, (RippleDrawable*)owner, res)
    , mMaxRadius(RADIUS_AUTO)
{
    CColorStateList::ValueOf(IColor::MAGENTA, (IColorStateList**)&mColor);
    if (orig != NULL && orig->Probe(EIID_IRippleState)) {
        AutoPtr<RippleState> origs = (RippleState*) orig;
        mTouchThemeAttrs = origs->mTouchThemeAttrs;
        mColor = origs->mColor;
        mMaxRadius = origs->mMaxRadius;
    }
}

ECode RippleDrawable::RippleState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mTouchThemeAttrs != NULL || (LayerState::CanApplyTheme(can), can);
    return NOERROR;
}

ECode RippleDrawable::RippleState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CRippleDrawable::New(this, NULL, NULL, drawable);
}

ECode RippleDrawable::RippleState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CRippleDrawable::New(this, res, NULL, drawable);
}

ECode RippleDrawable::RippleState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return CRippleDrawable::New(this, res, theme, drawable);
}

/////////////////////// RippleDrawable //////////////
static AutoPtr<IPorterDuffXfermode> InitPorterDuffXfermode(
    /* [in] */ PorterDuffMode mode)
{
    AutoPtr<IPorterDuffXfermode> pdx;
    CPorterDuffXfermode::New(mode, (IPorterDuffXfermode**)&pdx);
    return pdx;
}

const Int32 RippleDrawable::RADIUS_AUTO = -1;
AutoPtr<IPorterDuffXfermode> RippleDrawable::DST_IN = InitPorterDuffXfermode(PorterDuffMode_DST_IN);
AutoPtr<IPorterDuffXfermode> RippleDrawable::SRC_ATOP = InitPorterDuffXfermode(PorterDuffMode_SRC_ATOP);
AutoPtr<IPorterDuffXfermode> RippleDrawable::SRC_OVER = InitPorterDuffXfermode(PorterDuffMode_SRC_OVER);
const Int32 RippleDrawable::MAX_RIPPLES = 10;

CAR_INTERFACE_IMPL(RippleDrawable, LayerDrawable, IRippleDrawable);

RippleDrawable::RippleDrawable()
    : mBackgroundActive(FALSE)
    , mRippleActive(FALSE)
    , mPendingX(0)
    , mPendingY(0)
    , mHasPending(FALSE)
    , mExitingRipplesCount(0)
    , mDensity(1.0f)
    , mOverrideBounds(FALSE)
    , mNeedsDraw(FALSE)
{
    AutoPtr<RippleState> rs = new RippleState(NULL, NULL, NULL);
    constructor(rs, NULL, NULL);
}

RippleDrawable::RippleDrawable(
    /* [in] */ /*@NonNull*/ IColorStateList* color,
    /* [in] */ /*@Nullable*/ IDrawable* content,
    /* [in] */ /*@Nullable*/ IDrawable* mask)
{
    constructor(color, content, mask);
}

ECode RippleDrawable::constructor(
    /* [in] */ /*@NonNull*/ IColorStateList* color,
    /* [in] */ /*@Nullable*/ IDrawable* content,
    /* [in] */ /*@Nullable*/ IDrawable* mask)
{
    assert(color != NULL);
    AutoPtr<RippleState> rs = new RippleState(NULL, NULL, NULL);
    constructor(rs, NULL, NULL);

    if (color == NULL) {
        // throw new IllegalArgumentException("RippleDrawable requires a non-NULL color");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (content != NULL) {
        AddLayer(content, NULL, 0, 0, 0, 0, 0);
    }

    if (mask != NULL) {
        AddLayer(mask, NULL, /*android.*/R::id::mask, 0, 0, 0, 0);
    }

    SetColor(color);
    EnsurePadding();
    InitializeFromState();
    return NOERROR;
}

ECode RippleDrawable::constructor(
    /* [in] */ RippleState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    CRect::New((IRect**)&mTempRect);
    CRect::New((IRect**)&mHotspotBounds);
    CRect::New((IRect**)&mDrawingBounds);
    CRect::New((IRect**)&mDirtyBounds);

    Boolean needsTheme = FALSE;

    AutoPtr<RippleState> ns;
    Boolean can = FALSE;
    if (theme != NULL && state != NULL && (state->CanApplyTheme(&can), can)) {
        ns = new RippleState(state, this, res);
        needsTheme = TRUE;
    } else if (state == NULL) {
        ns = new RippleState(NULL, this, res);
    } else {
        // We always need a new state since child drawables contain local
        // state but live within the parent's constant state.
        // TODO: Move child drawables into local state.
        ns = new RippleState(state, this, res);
    }

    if (res != NULL) {
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        dm->GetDensity(&mDensity);
    }

    mState = ns;
    mLayerState = ns;

    if (ns->mNum > 0) {
        EnsurePadding();
    }

    if (needsTheme) {
        ApplyTheme(theme);
    }

    InitializeFromState();
    return NOERROR;
}

ECode RippleDrawable::JumpToCurrentState()
{
    LayerDrawable::JumpToCurrentState();

    Boolean needsDraw = FALSE;

    if (mRipple != NULL) {
        needsDraw |= mRipple->IsHardwareAnimating();
        mRipple->Jump();
    }

    if (mBackground != NULL) {
        needsDraw |= mBackground->IsHardwareAnimating();
        mBackground->Jump();
    }

    needsDraw |= CancelExitingRipples();

    mNeedsDraw = needsDraw;
    return InvalidateSelf();
}

Boolean RippleDrawable::CancelExitingRipples()
{
    Boolean needsDraw = FALSE;

    Int32 count = mExitingRipplesCount;
    AutoPtr<ArrayOf<Ripple*> > ripples = mExitingRipples;
    for (Int32 i = 0; i < count; i++) {
        needsDraw |= (*ripples)[i]->IsHardwareAnimating();
        (*ripples)[i]->Cancel();
    }

    if (ripples != NULL) {
        Arrays::Fill(ripples, 0, count, (Ripple*)NULL);
    }
    mExitingRipplesCount = 0;

    return needsDraw;
}

ECode RippleDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return LayerDrawable::SetAlpha(alpha);

    // TODO: Should we support this?
}

ECode RippleDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return LayerDrawable::SetColorFilter(cf);

    // TODO: Should we support this?
}

ECode RippleDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    // Worst-case scenario.
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

Boolean RippleDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* stateSet)
{
    Boolean changed = LayerDrawable::OnStateChange(stateSet);

    Boolean enabled = FALSE;
    Boolean pressed = FALSE;
    Boolean focused = FALSE;

    for (Int32 i = 0; i < stateSet->GetLength(); i++ ) {
        Int32 state = (*stateSet)[i];
        if (state == R::attr::state_enabled) {
            enabled = TRUE;
        }
        if (state == R::attr::state_focused) {
            focused = TRUE;
        }
        if (state == R::attr::state_pressed) {
            pressed = TRUE;
        }
    }

    SetRippleActive(enabled && pressed);
    SetBackgroundActive(focused || (enabled && pressed));

    return changed;
}

void RippleDrawable::SetRippleActive(
    /* [in] */ Boolean active)
{
    if (mRippleActive != active) {
        mRippleActive = active;
        if (active) {
            TryRippleEnter();
        } else {
            TryRippleExit();
        }
    }
}

void RippleDrawable::SetBackgroundActive(
    /* [in] */ Boolean active)
{
    if (mBackgroundActive != active) {
        mBackgroundActive = active;
        if (active) {
            TryBackgroundEnter();
        } else {
            TryBackgroundExit();
        }
    }
}

void RippleDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    LayerDrawable::OnBoundsChange(bounds);

    if (!mOverrideBounds) {
        mHotspotBounds->Set(bounds);
        OnHotspotBoundsChanged();
    }

    InvalidateSelf();
}

ECode RippleDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean changed = FALSE;
    LayerDrawable::SetVisible(visible, restart, &changed);

    if (!visible) {
        ClearHotspots();
    } else if (changed) {
        // If we just became visible, ensure the background and ripple
        // visibilities are consistent with their internal states.
        if (mRippleActive) {
            TryRippleEnter();
        }

        if (mBackgroundActive) {
            TryBackgroundEnter();
        }
    }

    *result = changed;
    return NOERROR;
}

ECode RippleDrawable::IsProjected(
    /* [out] */ Boolean* projected)
{
    VALIDATE_NOT_NULL(projected);
    Int32 num = 0;
    *projected = (GetNumberOfLayers(&num), num) == 0;
    return NOERROR;
}

ECode RippleDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    *isStateful = TRUE;
    return NOERROR;
}

ECode RippleDrawable::SetColor(
    /* [in] */ IColorStateList* color)
{
    mState->mColor = color;
    return InvalidateSelf();
}

ECode RippleDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/
{
    AutoPtr<ITypedArray> a;
    Int32 size = ArraySize(R::styleable::RippleDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::RippleDrawable, size);

    FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));
    ECode ec = UpdateStateFromTypedArray(a);
    if (FAILED(ec)) {
        a->Recycle();
        return ec;
    }
    a->Recycle();

    // Force padding default to STACK before inflating.
    SetPaddingMode(PADDING_MODE_STACK);

    FAIL_RETURN(LayerDrawable::Inflate(r, parser, attrs, theme));

    AutoPtr<IDisplayMetrics> dm;
    r->GetDisplayMetrics((IDisplayMetrics**)&dm);
    SetTargetDensity(dm);
    InitializeFromState();
    return NOERROR;
}

ECode RippleDrawable::SetDrawableByLayerId(
    /* [in] */ Int32 id,
    /* [in] */ IDrawable* drawable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if ((LayerDrawable::SetDrawableByLayerId(id, drawable, &tmp), tmp)) {
        if (id == R::id::mask) {
            mMask = drawable;
        }

        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode RippleDrawable::SetPaddingMode(
    /* [in] */ Int32 mode)
{
    return LayerDrawable::SetPaddingMode(mode);
}

ECode RippleDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    AutoPtr<RippleState> state = mState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    state->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mTouchThemeAttrs);

    AutoPtr<IColorStateList> color;
    a->GetColorStateList(R::styleable::RippleDrawable_color, (IColorStateList**)&color);
    if (color != NULL) {
        mState->mColor = color;
    }

    return VerifyRequiredAttributes(a);
}

ECode RippleDrawable::VerifyRequiredAttributes(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    if (mState->mColor == NULL && (mState->mTouchThemeAttrs == NULL
            || mState->mTouchThemeAttrs[R::styleable::RippleDrawable_color] == 0)) {
        // throw new XmlPullParserException(a.getPositionDescription() +
        //         ": <ripple> requires a valid color attribute");
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

void RippleDrawable::SetTargetDensity(
    /* [in] */ IDisplayMetrics* metrics)
{
    Float density = 0;
    metrics->GetDensity(&density);
    if (mDensity != density) {
        mDensity = density;
        InvalidateSelf();
    }
}

ECode RippleDrawable::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    LayerDrawable::ApplyTheme(t);

    AutoPtr<RippleState> state = mState;
    if (state == NULL || state->mTouchThemeAttrs == NULL) {
        return NOERROR;
    }

    AutoPtr<ITypedArray> a;
    Int32 size = ArraySize(R::styleable::RippleDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::RippleDrawable, size);
    ((CResources::Theme*)t)->ResolveAttribute(state->mTouchThemeAttrs, layout, (ITypedArray**)&a);

    // try {
    ECode ec = UpdateStateFromTypedArray(a);
    if (FAILED(ec)) {
        a->Recycle();
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (XmlPullParserException e) {
    //     throw new RuntimeException(e);
    // } finally {
    a->Recycle();
    // }

    InitializeFromState();
    return NOERROR;
}

ECode RippleDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = (LayerDrawable::CanApplyTheme(can), *can) || (mState != NULL && mState->mTouchThemeAttrs != NULL);
    return NOERROR;
}

ECode RippleDrawable::SetHotspot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (mRipple == NULL || mBackground == NULL) {
        mPendingX = x;
        mPendingY = y;
        mHasPending = TRUE;
    }

    if (mRipple != NULL) {
        mRipple->Move(x, y);
    }
    return NOERROR;
}

void RippleDrawable::TryBackgroundEnter()
{
    if (mBackground == NULL) {
        mBackground = new RippleBackground(this, mHotspotBounds);
    }

    AutoPtr<ArrayOf<Int32> > states;
    GetState((ArrayOf<Int32>**)&states);
    Int32 color = 0;
    mState->mColor->GetColorForState(states, IColor::TRANSPARENT, &color);
    mBackground->Setup(mState->mMaxRadius, color, mDensity);
    mBackground->Enter();
}

void RippleDrawable::TryBackgroundExit()
{
    if (mBackground != NULL) {
        // Don't NULL out the background, we need it to draw!
        mBackground->Exit();
    }
}

void RippleDrawable::TryRippleEnter()
{
    if (mExitingRipplesCount >= MAX_RIPPLES) {
        // This should never happen unless the user is tapping like a maniac
        // or there is a bug that's preventing ripples from being removed.
        return;
    }

    if (mRipple == NULL) {
        Float x = 0;
        Float y = 0;
        if (mHasPending) {
            mHasPending = FALSE;
            x = mPendingX;
            y = mPendingY;
        } else {
            mHotspotBounds->GetExactCenterX(&x);
            mHotspotBounds->GetExactCenterY(&y);
        }
        mRipple = new Ripple(this, mHotspotBounds, x, y);
    }

    Int32 color = 0;
    AutoPtr<ArrayOf<Int32> > states;
    GetState((ArrayOf<Int32>**)&states);
    mState->mColor->GetColorForState(states, IColor::TRANSPARENT, &color);
    mRipple->Setup(mState->mMaxRadius, color, mDensity);
    mRipple->Enter();
}

void RippleDrawable::TryRippleExit()
{
    if (mRipple != NULL) {
        if (mExitingRipples == NULL) {
            mExitingRipples = ArrayOf<Ripple*>::Alloc(MAX_RIPPLES);
        }
        mExitingRipples->Set(mExitingRipplesCount++, mRipple);
        mRipple->Exit();
        mRipple = NULL;
    }
}

void RippleDrawable::ClearHotspots()
{
    Boolean needsDraw = FALSE;

    if (mRipple != NULL) {
        needsDraw |= mRipple->IsHardwareAnimating();
        mRipple->Cancel();
        mRipple = NULL;
    }

    if (mBackground != NULL) {
        needsDraw |= mBackground->IsHardwareAnimating();
        mBackground->Cancel();
        mBackground = NULL;
    }

    needsDraw |= CancelExitingRipples();

    mNeedsDraw = needsDraw;
    InvalidateSelf();
}

ECode RippleDrawable::SetHotspotBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    mOverrideBounds = TRUE;
    mHotspotBounds->Set(left, top, right, bottom);

    OnHotspotBoundsChanged();
    return NOERROR;
}

ECode RippleDrawable::GetHotspotBounds(
    /* [in] */ IRect* outRect)
{
    return outRect->Set(mHotspotBounds);
}

void RippleDrawable::OnHotspotBoundsChanged()
{
    Int32 count = mExitingRipplesCount;
    AutoPtr<ArrayOf<Ripple*> > ripples = mExitingRipples;
    for (Int32 i = 0; i < count; i++) {
        (*ripples)[i]->OnHotspotBoundsChanged();
    }

    if (mRipple != NULL) {
        mRipple->OnHotspotBoundsChanged();
    }

    if (mBackground != NULL) {
        mBackground->OnHotspotBoundsChanged();
    }
}

ECode RippleDrawable::GetOutline(
    /* [in] */ /*@NonNull*/ IOutline* outline)
{
    assert(outline != NULL);
    AutoPtr<LayerState> state = mLayerState;
    AutoPtr<ArrayOf<ChildDrawable*> > children = state->mChildren;
    const Int32 N = state->mNum;
    for (Int32 i = 0; i < N; i++) {
        if ((*children)[i]->mId != R::id::mask) {
            (*children)[i]->mDrawable->GetOutline(outline);
            Boolean empty = FALSE;
            if (!(outline->IsEmpty(&empty), empty)) return NOERROR;
        }
    }
    return NOERROR;
}

ECode RippleDrawable::Draw(
    /* [in] */ /*@NonNull*/ ICanvas* canvas)
{
    assert(canvas != NULL);
    Boolean hasMask = mMask != NULL;
    Boolean drawNonMaskContent = mLayerState->mNum > (hasMask ? 1 : 0);
    Int32 iv = 0;
    Boolean drawMask = hasMask && (mMask->GetOpacity(&iv), iv) != IPixelFormat::OPAQUE;
    AutoPtr<IRect> bounds;
    GetDirtyBounds((IRect**)&bounds);
    Int32 saveCount = 0;
    canvas->Save(ICanvas::CLIP_SAVE_FLAG, &saveCount);
    Boolean isNonEmpty = FALSE;
    canvas->ClipRect(bounds, &isNonEmpty);

    // If we have content, draw it into a layer first.
    Int32 contentLayer = 0;
    if (drawNonMaskContent) {
        contentLayer = DrawContentLayer(canvas, bounds, SRC_OVER);
    } else {
        contentLayer = -1;
    }

    // Next, try to draw the ripples (into a layer if necessary). If we need
    // to mask against the underlying content, set the xfermode to SRC_ATOP.
    AutoPtr<IPorterDuffXfermode> xfermode = (hasMask || !drawNonMaskContent) ? SRC_OVER : SRC_ATOP;

    // If we have a background and a non-opaque mask, draw the masking layer.
    Int32 backgroundLayer = DrawBackgroundLayer(canvas, bounds, xfermode, drawMask);
    if (backgroundLayer >= 0) {
        if (drawMask) {
            DrawMaskingLayer(canvas, bounds, DST_IN);
        }
        canvas->RestoreToCount(backgroundLayer);
    }

    // If we have ripples and a non-opaque mask, draw the masking layer.
    Int32 rippleLayer = DrawRippleLayer(canvas, bounds, xfermode);
    if (rippleLayer >= 0) {
        if (drawMask) {
            DrawMaskingLayer(canvas, bounds, DST_IN);
        }
        canvas->RestoreToCount(rippleLayer);
    }

    // If we failed to draw anything and we just canceled animations, at
    // least draw a color so that hardware invalidation works correctly.
    if (contentLayer < 0 && backgroundLayer < 0 && rippleLayer < 0 && mNeedsDraw) {
        canvas->DrawColor(IColor::TRANSPARENT);

        // Request another draw so we can avoid adding a transparent layer
        // during the next display list refresh.
        InvalidateSelf();
    }
    mNeedsDraw = FALSE;

    return canvas->RestoreToCount(saveCount);
}

void RippleDrawable::RemoveRipple(
    /* [in] */ Ripple* ripple)
{
    // Ripple ripple ripple ripple. Ripple ripple.
    AutoPtr<ArrayOf<Ripple*> > ripples = mExitingRipples;
    Int32 count = mExitingRipplesCount;
    Int32 index = GetRippleIndex(ripple);
    if (index >= 0) {
        // System.arraycopy(ripples, index + 1, ripples, index, count - (index + 1));
        ripples->Copy(index, ripples, index + 1, count - (index + 1));
        (*ripples)[count - 1] = NULL;
        mExitingRipplesCount--;

        InvalidateSelf();
    }
}

Int32 RippleDrawable::GetRippleIndex(
    /* [in] */ Ripple* ripple)
{
    AutoPtr<ArrayOf<Ripple*> > ripples = mExitingRipples;
    const Int32 count = mExitingRipplesCount;
    for (Int32 i = 0; i < count; i++) {
        if ((*ripples)[i] == ripple) {
            return i;
        }
    }
    return -1;
}

Int32 RippleDrawable::DrawContentLayer(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* bounds,
    /* [in] */ IPorterDuffXfermode* mode)
{
    AutoPtr<ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 count = mLayerState->mNum;

    // We don't need a layer if we don't expect to draw any ripples, we have
    // an explicit mask, or if the non-mask content is all opaque.
    Boolean needsLayer = FALSE;
    if ((mExitingRipplesCount > 0 || mBackground != NULL) && mMask == NULL) {
        Int32 iv = 0;
        for (Int32 i = 0; i < count; i++) {
            if ((*array)[i]->mId != R::id::mask
                    && ((*array)[i]->mDrawable->GetOpacity(&iv), iv) != IPixelFormat::OPAQUE) {
                needsLayer = TRUE;
                break;
            }
        }
    }

    AutoPtr<IPaint> maskingPaint = GetMaskingPaint(mode);
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    bounds->Get(&left, &top, &right, &bottom);
    Int32 restoreToCount = -1;
    if (needsLayer) {
        canvas->SaveLayer(left, top, right, bottom, maskingPaint, &restoreToCount);
    }

    // Draw everything except the mask.
    for (Int32 i = 0; i < count; i++) {
        if ((*array)[i]->mId != R::id::mask) {
            (*array)[i]->mDrawable->Draw(canvas);
        }
    }

    return restoreToCount;
}

Int32 RippleDrawable::DrawBackgroundLayer(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* bounds,
    /* [in] */ IPorterDuffXfermode* mode,
    /* [in] */ Boolean drawMask)
{
    Int32 saveCount = -1;

    if (mBackground != NULL && mBackground->ShouldDraw()) {
        // TODO: We can avoid saveLayer here if we push the xfermode into
        // the background's render thread animator at exit() time.
        if (drawMask || mode != SRC_OVER) {
            Int32 left = 0, top = 0, right = 0, bottom = 0;
            bounds->Get(&left, &top, &right, &bottom);
            canvas->SaveLayer(left, top, right, bottom, GetMaskingPaint(mode), &saveCount);
        }

        Float x = 0;
        mHotspotBounds->GetExactCenterX(&x);
        Float y = 0;
        mHotspotBounds->GetExactCenterY(&y);
        canvas->Translate(x, y);
        mBackground->Draw(canvas, GetRipplePaint());
        canvas->Translate(-x, -y);
    }

    return saveCount;
}

Int32 RippleDrawable::DrawRippleLayer(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* bounds,
    /* [in] */ IPorterDuffXfermode* mode)
{
    Boolean drewRipples = FALSE;
    Int32 restoreToCount = -1;
    Int32 restoreTranslate = -1;

    // Draw ripples and update the animating ripples array.
    const Int32 count = mExitingRipplesCount;
    AutoPtr<ArrayOf<Ripple*> > ripples = mExitingRipples;
    for (Int32 i = 0; i <= count; i++) {
        AutoPtr<Ripple> ripple;
        if (i < count) {
            ripple = (*ripples)[i];
        } else if (mRipple != NULL) {
            ripple = mRipple;
        } else {
            continue;
        }

        // If we're masking the ripple layer, make sure we have a layer
        // first. This will merge SRC_OVER (directly) onto the canvas.
        if (restoreToCount < 0) {
            AutoPtr<IPaint> maskingPaint = GetMaskingPaint(mode);
            Int32 color = 0;
            AutoPtr<ArrayOf<Int32> > states;
            GetState((ArrayOf<Int32>**)&states);
            mState->mColor->GetColorForState(states, IColor::TRANSPARENT, &color);
            maskingPaint->SetAlpha(Color::Alpha(color) / 2);

            // TODO: We can avoid saveLayer here if we're only drawing one
            // ripple and we don't have content or a translucent mask.
            Int32 left = 0, top = 0, right = 0, bottom = 0;
            bounds->Get(&left, &top, &right, &bottom);
            canvas->SaveLayer(left, top, right, bottom, maskingPaint, &restoreToCount);

            // Translate the canvas to the current hotspot bounds.
            canvas->Save(&restoreTranslate);
            Float x = 0, y = 0;
            mHotspotBounds->GetExactCenterX(&x);
            mHotspotBounds->GetExactCenterY(&y);
            canvas->Translate(x, y);
        }

        drewRipples |= ripple->Draw(canvas, GetRipplePaint());
    }

    // Always restore the translation.
    if (restoreTranslate >= 0) {
        canvas->RestoreToCount(restoreTranslate);
    }

    // If we created a layer with no content, merge it immediately.
    if (restoreToCount >= 0 && !drewRipples) {
        canvas->RestoreToCount(restoreToCount);
        restoreToCount = -1;
    }

    return restoreToCount;
}

Int32 RippleDrawable::DrawMaskingLayer(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* bounds,
    /* [in] */ IPorterDuffXfermode* mode)
{
    Int32 restoreToCount = 0;
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    bounds->Get(&left, &top, &right, &bottom);
    canvas->SaveLayer(left, top, right, bottom, GetMaskingPaint(mode), &restoreToCount);

    // Ensure that DST_IN blends using the entire layer.
    canvas->DrawColor(IColor::TRANSPARENT);

    mMask->Draw(canvas);

    return restoreToCount;
}

AutoPtr<IPaint> RippleDrawable::GetRipplePaint()
{
    if (mRipplePaint == NULL) {
        CPaint::New((IPaint**)&mRipplePaint);
        mRipplePaint->SetAntiAlias(TRUE);
    }
    return mRipplePaint;
}

AutoPtr<IPaint> RippleDrawable::GetMaskingPaint(
    /* [in] */ IPorterDuffXfermode* xfermode)
{
    if (mMaskingPaint == NULL) {
        CPaint::New((IPaint**)&mMaskingPaint);
    }
    mMaskingPaint->SetXfermode(IXfermode::Probe(xfermode));
    mMaskingPaint->SetAlpha(0xFF);
    return mMaskingPaint;
}

ECode RippleDrawable::GetDirtyBounds(
    /* [out] */ IRect** dirty)
{
    VALIDATE_NOT_NULL(dirty);
    Boolean bv = FALSE;
    if ((IsProjected(&bv), bv)) {
        AutoPtr<IRect> drawingBounds = mDrawingBounds;
        AutoPtr<IRect> dirtyBounds = mDirtyBounds;
        dirtyBounds->Set(drawingBounds);
        drawingBounds->SetEmpty();

        Float fv = 0;
        mHotspotBounds->GetExactCenterX(&fv);
        Int32 cX = (Int32) fv;
        mHotspotBounds->GetExactCenterY(&fv);
        Int32 cY = (Int32) fv;
        AutoPtr<IRect> rippleBounds = mTempRect;

        AutoPtr<ArrayOf<Ripple*> > activeRipples = mExitingRipples;
        const Int32 N = mExitingRipplesCount;
        for (Int32 i = 0; i < N; i++) {
            (*activeRipples)[i]->GetBounds(rippleBounds);
            rippleBounds->Offset(cX, cY);
            drawingBounds->Union(rippleBounds);
        }

        AutoPtr<RippleBackground> background = mBackground;
        if (background != NULL) {
            background->GetBounds(rippleBounds);
            rippleBounds->Offset(cX, cY);
            drawingBounds->Union(rippleBounds);
        }

        dirtyBounds->Union(drawingBounds);
        AutoPtr<IRect> r;
        LayerDrawable::GetDirtyBounds((IRect**)&r);
        dirtyBounds->Union(r);
        *dirty = dirtyBounds;
        REFCOUNT_ADD(*dirty);
        return NOERROR;
    }

    return GetBounds(dirty);
}

ECode RippleDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode RippleDrawable::Mutate()
{
    LayerDrawable::Mutate();

    // LayerDrawable creates a new state using createConstantState, so
    // this should always be a safe cast.
    mState = (RippleState*) mLayerState.Get();
    return NOERROR;
}

AutoPtr<LayerDrawable::LayerState> RippleDrawable::CreateConstantState(
    /* [in] */ LayerState* state,
    /* [in] */ IResources* res)
{
    AutoPtr<LayerState> ls = new RippleState(state, this, res);
    return ls;
}

ECode RippleDrawable::SetMaxRadius(
    /* [in] */ Int32 maxRadius)
{
    if (maxRadius != RADIUS_AUTO && maxRadius < 0) {
        // throw new IllegalArgumentException("maxRadius must be RADIUS_AUTO or >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mState->mMaxRadius = maxRadius;
    return NOERROR;
}

ECode RippleDrawable::GetMaxRadius(
    /* [out] */ Int32* radius)
{
    VALIDATE_NOT_NULL(radius);
    *radius = mState->mMaxRadius;
    return NOERROR;
}

RippleDrawable::RippleDrawable(
    /* [in] */ RippleState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    constructor(state, res, theme);
}

void RippleDrawable::InitializeFromState()
{
    // Initialize from constant state.
    FindDrawableByLayerId(R::id::mask, (IDrawable**)&mMask);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
