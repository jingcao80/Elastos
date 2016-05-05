
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/ScaleDrawable.h"
#include "elastos/droid/graphics/drawable/CScaleDrawable.h"
#include "elastos/droid/view/CGravity.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

ScaleDrawable::ScaleState::ScaleState(
    /* [in] */ ScaleState* orig,
    /* [in] */ ScaleDrawable* owner,
    /* [in] */ IResources* res)
    : mChangingConfigurations(0)
    , mScaleWidth(0)
    , mScaleHeight(0)
    , mGravity(0)
    , mUseIntrinsicSizeAsMin(FALSE)
    , mCheckedConstantState(FALSE)
    , mCanConstantState(FALSE)
{
    if (orig != NULL) {
        AutoPtr<IDrawableConstantState> state;
        orig->mDrawable->GetConstantState((IDrawableConstantState**)&state);
        if (res != NULL) {
            state->NewDrawable(res, (IDrawable**)&mDrawable);
        }
        else {
            state->NewDrawable((IDrawable**)&mDrawable);
        }
        mDrawable->SetCallback((IDrawableCallback*)owner);
        mScaleWidth = orig->mScaleWidth;
        mScaleHeight = orig->mScaleHeight;
        mGravity = orig->mGravity;
        mUseIntrinsicSizeAsMin = orig->mUseIntrinsicSizeAsMin;
        mCheckedConstantState = mCanConstantState = TRUE;
    }
}

ECode ScaleDrawable::ScaleState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CScaleDrawable::New(this, NULL, drawable);
}

ECode ScaleDrawable::ScaleState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CScaleDrawable::New(this, res, drawable);
}

ECode ScaleDrawable::ScaleState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}

Boolean ScaleDrawable::ScaleState::CanConstantState()
{
    if (!mCheckedConstantState) {
        AutoPtr<IDrawableConstantState> state;
        mDrawable->GetConstantState((IDrawableConstantState**)&state);
        mCanConstantState = state != NULL;
        mCheckedConstantState = TRUE;
    }

    return mCanConstantState;
}

CAR_INTERFACE_IMPL_2(ScaleDrawable, Drawable, IScaleDrawable, IDrawableCallback);

ScaleDrawable::ScaleDrawable()
    : mMutated(FALSE)
{
    CRect::New((IRect**)&mTmpRect);
}

ECode ScaleDrawable::constructor()
{
    return constructor(NULL, NULL);
}

ECode ScaleDrawable::constructor(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 gravity,
    /* [in] */ Float scaleWidth,
    /* [in] */ Float scaleHeight)
{
    FAIL_RETURN(constructor(NULL, NULL));

    mScaleState->mDrawable = drawable;
    mScaleState->mGravity = gravity;
    mScaleState->mScaleWidth = scaleWidth;
    mScaleState->mScaleHeight = scaleHeight;

    if (drawable != NULL) {
        drawable->SetCallback(this);
    }
    return NOERROR;
}

ECode ScaleDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res)
{
    mScaleState = new ScaleState((ScaleState*)state, this, res);
    return NOERROR;
}

ECode ScaleDrawable::GetDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mScaleState->mDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

Float ScaleDrawable::GetPercent(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 name)
{
    String s;
    a->GetString(name, &s);
    if (!s.IsNull()) {
        if (s.EndWith("%")) {
            String f = s.Substring(0, s.GetLength() - 1);
            return StringUtils::ParseFloat(f) / 100.0f;
        }
    }
    return -1.0f;
}

ECode ScaleDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    FAIL_RETURN(Drawable::Inflate(r, parser, attrs, theme));

    Int32 type;

    Int32 size = ArraySize(R::styleable::ScaleDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::ScaleDrawable, size);

    AutoPtr<ITypedArray> a;
    ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);

    Float sw = GetPercent(a, R::styleable::ScaleDrawable_scaleWidth);
    Float sh = GetPercent(a, R::styleable::ScaleDrawable_scaleHeight);
    Int32 g;
    a->GetInt32(
            R::styleable::ScaleDrawable_scaleGravity, IGravity::LEFT, &g);
    Boolean min = FALSE;
    a->GetBoolean(R::styleable::ScaleDrawable_useIntrinsicSizeAsMinimum, FALSE, &min);
    AutoPtr<IDrawable> dr;
    a->GetDrawable(R::styleable::ScaleDrawable_drawable, (IDrawable**)&dr);

    a->Recycle();

    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }
        dr = NULL;
        Drawable::CreateFromXmlInner(r, parser, attrs, theme, (IDrawable**)&dr);
    }

    if (dr == NULL) {
//        throw new IllegalArgumentException("No drawable specified for <scale>");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mScaleState->mDrawable = dr;
    mScaleState->mScaleWidth = sw;
    mScaleState->mScaleHeight = sh;
    mScaleState->mGravity = g;
    mScaleState->mUseIntrinsicSizeAsMin = min;
    if (dr != NULL) {
        dr->SetCallback(this);
    }
    return NOERROR;
}

ECode ScaleDrawable::InvalidateDrawable(
    /* [in] */ IDrawable* who)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->InvalidateDrawable(this);
    }
    return NOERROR;
}

ECode ScaleDrawable::ScheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what,
    /* [in] */ Int64 when)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->ScheduleDrawable(this, what, when);
    }
    return NOERROR;
}

ECode ScaleDrawable::UnscheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->UnscheduleDrawable(this, what);
    }
    return NOERROR;
}

ECode ScaleDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 level;
    mScaleState->mDrawable->GetLevel(&level);
    if (level != 0) {
        mScaleState->mDrawable->Draw(canvas);
    }
    return NOERROR;
}

ECode ScaleDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Int32 config;
    mScaleState->mDrawable->GetChangingConfigurations(&config);
    Drawable::GetChangingConfigurations(configuration);
    *configuration = (*configuration)
            | mScaleState->mChangingConfigurations
            | config;
    return NOERROR;
}

ECode ScaleDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    // XXX need to adjust padding!
    return mScaleState->mDrawable->GetPadding(padding, isPadding);
}

ECode ScaleDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* isDifferent)
{
    VALIDATE_NOT_NULL(isDifferent);
    mScaleState->mDrawable->SetVisible(visible, restart, isDifferent);
    return Drawable::SetVisible(visible, restart, isDifferent);
}

ECode ScaleDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return mScaleState->mDrawable->SetAlpha(alpha);
}

ECode ScaleDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    return mScaleState->mDrawable->GetAlpha(alpha);
}

ECode ScaleDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    return mScaleState->mDrawable->SetTintList(tint);
}

ECode ScaleDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    return mScaleState->mDrawable->SetTintMode(tintMode);
}

ECode ScaleDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return mScaleState->mDrawable->SetColorFilter(cf);
}

ECode ScaleDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    return mScaleState->mDrawable->GetOpacity(opacity);
}

ECode ScaleDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    return mScaleState->mDrawable->IsStateful(isStateful);
}

Boolean ScaleDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* state)
{
    Boolean changed;
    mScaleState->mDrawable->SetState(
            const_cast<ArrayOf<Int32>*>(state), &changed);
    AutoPtr<IRect> rect;
    GetBounds((IRect**)&rect);
    OnBoundsChange(rect);
    return changed;
}

Boolean ScaleDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Boolean changed;
    mScaleState->mDrawable->SetLevel(level, &changed);
    AutoPtr<IRect> rect;
    GetBounds((IRect**)&rect);
    OnBoundsChange(rect);
    InvalidateSelf();
    return TRUE;
}

void ScaleDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    CRect* r = (CRect*)mTmpRect.Get();
    Boolean min = mScaleState->mUseIntrinsicSizeAsMin;
    Int32 level = 0;
    GetLevel(&level);
    Int32 w;
    bounds->GetWidth(&w);
    if (mScaleState->mScaleWidth > 0) {
        Int32 iw = 0;
        if (min) {
            mScaleState->mDrawable->GetIntrinsicWidth(&iw);
        }
        w -= (Int32)((w - iw) * (10000 - level) * mScaleState->mScaleWidth / 10000);
    }
    Int32 h;
    bounds->GetHeight(&h);
    if (mScaleState->mScaleHeight > 0) {
        Int32 ih = 0;
        if (min) {
            mScaleState->mDrawable->GetIntrinsicHeight(&ih);
        }
        h -= (Int32)((h - ih) * (10000 - level) * mScaleState->mScaleHeight / 10000);
    }
    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    AutoPtr<IGravity> gravity;
    CGravity::AcquireSingleton((IGravity**)&gravity);
    gravity->Apply(mScaleState->mGravity, w, h, bounds, r, layoutDirection);

    if (w > 0 && h > 0) {
        mScaleState->mDrawable->SetBounds(r->mLeft, r->mTop, r->mRight, r->mBottom);
    }
}

ECode ScaleDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    return mScaleState->mDrawable->GetIntrinsicWidth(width);
}

ECode ScaleDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    return mScaleState->mDrawable->GetIntrinsicHeight(height);
}

ECode ScaleDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    if (mScaleState->CanConstantState()) {
        GetChangingConfigurations(&mScaleState->mChangingConfigurations);
        *state = mScaleState;
        REFCOUNT_ADD(*state);
        return NOERROR;
    }
    *state = NULL;
    return NOERROR;
}

ECode ScaleDrawable::Mutate()
{
    if (!mMutated) {
        mScaleState->mDrawable->Mutate();
        mMutated = TRUE;
    }
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
