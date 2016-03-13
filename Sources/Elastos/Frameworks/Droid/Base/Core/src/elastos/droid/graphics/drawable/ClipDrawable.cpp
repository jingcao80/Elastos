
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/ClipDrawable.h"
#include "elastos/droid/graphics/drawable/CClipDrawable.h"
#include "elastos/droid/view/CGravity.h"
#include "elastos/droid/R.h"

using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

ClipDrawable::ClipState::ClipState(
    /* [in] */ ClipState* orig,
    /* [in] */ ClipDrawable* owner,
    /* [in] */ IResources* res)
    : mChangingConfigurations(0)
    , mOrientation(0)
    , mGravity(0)
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
        mDrawable->SetCallback(
                (IDrawableCallback*)owner->Probe(EIID_IDrawableCallback));

        Int32 value = 0;
        orig->mDrawable->GetLayoutDirection(&value);
        mDrawable->SetLayoutDirection(value);
        AutoPtr<IRect> bounds;
        orig->mDrawable->GetBounds((IRect**)&bounds);
        mDrawable->SetBounds(bounds);
        orig->mDrawable->GetLevel(&value);
        Boolean res = FALSE;
        mDrawable->SetLevel(value, &res);
        mOrientation = orig->mOrientation;
        mGravity = orig->mGravity;
        mCheckedConstantState = mCanConstantState = TRUE;
    }
}

ECode ClipDrawable::ClipState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    return CClipDrawable::New(this, NULL, drawable);
}

ECode ClipDrawable::ClipState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    return CClipDrawable::New(this, res, drawable);
}

ECode ClipDrawable::ClipState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);

    *config = mChangingConfigurations;
    return NOERROR;
}

Boolean ClipDrawable::ClipState::CanConstantState()
{
    if (!mCheckedConstantState) {
        AutoPtr<IDrawableConstantState> state;
        mDrawable->GetConstantState((IDrawableConstantState**)&state);
        mCanConstantState = state != NULL;
        mCheckedConstantState = TRUE;
    }

    return mCanConstantState;
}

CAR_INTERFACE_IMPL_2(ClipDrawable, Drawable, IClipDrawable, IDrawableCallback)
ClipDrawable::ClipDrawable()
{
    CRect::NewByFriend((CRect**)&mTmpRect);
}

ClipDrawable::ClipDrawable(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 orientation)
{
    CRect::NewByFriend((CRect**)&mTmpRect);
    ASSERT_SUCCEEDED(constructor(drawable, gravity, orientation));
}

ECode ClipDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    FAIL_RETURN(Drawable::Inflate(r, parser, attrs, theme));

    Int32 type;

    Int32 size = ArraySize(R::styleable::ClipDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::ClipDrawable, size);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));

    Int32 orientation;
    a->GetInt32(
            R::styleable::ClipDrawable_clipOrientation,
            IClipDrawable::HORIZONTAL, &orientation);
    Int32 g;
    a->GetInt32(R::styleable::ClipDrawable_gravity, IGravity::LEFT, &g);
    AutoPtr<IDrawable> dr;
    a->GetDrawable(R::styleable::ClipDrawable_drawable, (IDrawable**)&dr);

    a->Recycle();

    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }
        dr = NULL;
        FAIL_RETURN(Drawable::CreateFromXmlInner(r, parser, attrs, theme, (IDrawable**)&dr));
    }

    if (dr == NULL) {
//        throw new IllegalArgumentException("No drawable specified for <clip>");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mClipState->mDrawable = dr;
    mClipState->mOrientation = orientation;
    mClipState->mGravity = g;

    dr->SetCallback((IDrawableCallback*)this->Probe(EIID_IDrawableCallback));
    return NOERROR;
}

ECode ClipDrawable::InvalidateDrawable(
    /* [in] */ IDrawable* who)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->InvalidateDrawable(
                (IDrawable*)this->Probe(EIID_IDrawable));
    }
    return NOERROR;
}

ECode ClipDrawable::ScheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what,
    /* [in] */ Int64 when)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->ScheduleDrawable(
                (IDrawable*)this->Probe(EIID_IDrawable), what, when);
    }
    return NOERROR;
}

ECode ClipDrawable::UnscheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->UnscheduleDrawable(
                (IDrawable*)this->Probe(EIID_IDrawable), what);
    }
    return NOERROR;
}

ECode ClipDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Int32 configs;
    mClipState->mDrawable->GetChangingConfigurations(&configs);
    Drawable::GetChangingConfigurations(configuration);
    *configuration = (*configuration)
            | mClipState->mChangingConfigurations
            | configs;
    return NOERROR;
}

ECode ClipDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    VALIDATE_NOT_NULL(isPadding);
    // XXX need to adjust padding!
    return mClipState->mDrawable->GetPadding(padding, isPadding);
}

ECode ClipDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* isDifferent)
{
    VALIDATE_NOT_NULL(isDifferent);
    Boolean different;
    mClipState->mDrawable->SetVisible(visible, restart, &different);
    return Drawable::SetVisible(visible, restart, isDifferent);
}

ECode ClipDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return mClipState->mDrawable->SetAlpha(alpha);
}

ECode ClipDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    return mClipState->mDrawable->GetAlpha(alpha);
}

ECode ClipDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    return mClipState->mDrawable->SetTintList(tint);
}

ECode ClipDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    return mClipState->mDrawable->SetTintMode(tintMode);
}

ECode ClipDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return mClipState->mDrawable->SetColorFilter(cf);
}

ECode ClipDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    return mClipState->mDrawable->GetOpacity(opacity);
}

ECode ClipDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    return mClipState->mDrawable->IsStateful(isStateful);
}

Boolean ClipDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* state)
{
    Boolean changed;
    mClipState->mDrawable->SetState(const_cast<ArrayOf<Int32>*>(state), &changed);
    return changed;
}

Boolean ClipDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Boolean changed;
    mClipState->mDrawable->SetLevel(level, &changed);
    InvalidateSelf();
    return TRUE;
}

void ClipDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    mClipState->mDrawable->SetBounds(bounds);
}

ECode ClipDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 l;
    mClipState->mDrawable->GetLevel(&l);
    if (l == 0) {
        return NOERROR;
    }

    IRect* r = (IRect*)mTmpRect.Get();
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 level = 0;
    GetLevel(&level);
    Int32 w;
    bounds->GetWidth(&w);
    Int32 iw = 0; //mClipState.mDrawable.getIntrinsicWidth();
    if ((mClipState->mOrientation & IClipDrawable::HORIZONTAL) != 0) {
        w -= (w - iw) * (10000 - level) / 10000;
    }
    Int32 h;
    bounds->GetHeight(&h);
    Int32 ih = 0; //mClipState.mDrawable.getIntrinsicHeight();
    if ((mClipState->mOrientation & IClipDrawable::VERTICAL) != 0) {
        h -= (h - ih) * (10000 - level) / 10000;
    }
    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    AutoPtr<IGravity> gravity;
    CGravity::AcquireSingleton((IGravity**)&gravity);
    gravity->Apply(mClipState->mGravity, w, h, bounds, r, layoutDirection);

    if (w > 0 && h > 0) {
        Int32 count;
        canvas->Save(&count);
        Boolean nonEmpty;
        canvas->ClipRect(r, &nonEmpty);
        mClipState->mDrawable->Draw(canvas);
        canvas->Restore();
    }
    return NOERROR;
}

ECode ClipDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    return mClipState->mDrawable->GetIntrinsicWidth(width);
}

ECode ClipDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    return mClipState->mDrawable->GetIntrinsicHeight(height);
}

ECode ClipDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    if (mClipState->CanConstantState()) {
        Drawable::GetChangingConfigurations(&mClipState->mChangingConfigurations);
        *state = mClipState;
        REFCOUNT_ADD(*state);
        return NOERROR;
    }
    *state = NULL;
    return NOERROR;
}

ECode ClipDrawable::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    mClipState->mDrawable->SetLayoutDirection(layoutDirection);
    Drawable::SetLayoutDirection(layoutDirection);
    return NOERROR;
}

ClipDrawable::ClipDrawable(
    /* [in] */ ClipState* state,
    /* [in] */ IResources* res)
{
    CRect::NewByFriend((CRect**)&mTmpRect);
    ASSERT_SUCCEEDED(constructor(state, res));
}

ECode ClipDrawable::constructor(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 orientation)
{
    FAIL_RETURN(constructor(NULL, NULL));

    mClipState->mDrawable = drawable;
    mClipState->mGravity = gravity;
    mClipState->mOrientation = orientation;

    if (drawable != NULL) {
        drawable->SetCallback(
                (IDrawableCallback*)this->Probe(EIID_IDrawableCallback));
    }
    return NOERROR;
}

ECode ClipDrawable::constructor(
    /* [in] */ ClipState* state,
    /* [in] */ IResources* res)
{
    mClipState = new ClipState(state, this, res);
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
