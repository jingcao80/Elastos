
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/drawable/InsetDrawable.h"
#include "elastos/droid/graphics/drawable/CInsetDrawable.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

InsetDrawable::InsetState::InsetState(
    /* [in] */ InsetState* orig,
    /* [in] */ InsetDrawable* owner,
    /* [in] */ IResources* res)
    : mChangingConfigurations(0)
    , mInsetLeft(0)
    , mInsetTop(0)
    , mInsetRight(0)
    , mInsetBottom(0)
    , mCheckedConstantState(FALSE)
    , mCanConstantState(FALSE)
{
    if (orig != NULL) {
        mThemeAttrs = orig->mThemeAttrs;
        mChangingConfigurations = orig->mChangingConfigurations;
        AutoPtr<IDrawableConstantState> state;
        orig->mDrawable->GetConstantState((IDrawableConstantState**)&state);
        if (res != NULL) {
            state->NewDrawable(res, (IDrawable**)&mDrawable);
        }
        else {
            state->NewDrawable((IDrawable**)&mDrawable);
        }
        mDrawable->SetCallback((IDrawableCallback*)owner->Probe(EIID_IDrawableCallback));

        Int32 dir = 0;
        orig->mDrawable->GetLayoutDirection(&dir);
        mDrawable->SetLayoutDirection(dir);
        AutoPtr<IRect> bounds;
        orig->mDrawable->GetBounds((IRect**)&bounds);
        mDrawable->SetBounds(bounds);
        Int32 level = 0;
        orig->mDrawable->GetLevel(&level);
        Boolean tmp = FALSE;
        mDrawable->SetLevel(level, &tmp);
        mInsetLeft = orig->mInsetLeft;
        mInsetTop = orig->mInsetTop;
        mInsetRight = orig->mInsetRight;
        mInsetBottom = orig->mInsetBottom;
        mCheckedConstantState = mCanConstantState = TRUE;
    }
}

ECode InsetDrawable::InsetState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CInsetDrawable::New(this, NULL, drawable);
}

ECode InsetDrawable::InsetState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CInsetDrawable::New(this, res, drawable);
}

ECode InsetDrawable::InsetState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}

Boolean InsetDrawable::InsetState::CanConstantState()
{
    if (!mCheckedConstantState) {
        AutoPtr<IDrawableConstantState> state;
        mDrawable->GetConstantState((IDrawableConstantState**)&state);
        mCanConstantState = state != NULL;
        mCheckedConstantState = TRUE;
    }

    return mCanConstantState;
}

CAR_INTERFACE_IMPL_2(InsetDrawable, Drawable, IInsetDrawable, IDrawableCallback);
InsetDrawable::InsetDrawable()
    : mMutated(FALSE)
{
    CRect::NewByFriend((CRect**)&mTmpRect);
}

InsetDrawable::InsetDrawable(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 inset)
    : mMutated(FALSE)
{
    CRect::NewByFriend((CRect**)&mTmpRect);
    ASSERT_SUCCEEDED(constructor(drawable, inset, inset, inset, inset));
}

InsetDrawable::InsetDrawable(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 insetLeft,
    /* [in] */ Int32 insetTop,
    /* [in] */ Int32 insetRight,
    /* [in] */ Int32 insetBottom)
    : mMutated(FALSE)
{
    CRect::NewByFriend((CRect**)&mTmpRect);
    ASSERT_SUCCEEDED(constructor(drawable, insetLeft, insetTop, insetRight, insetBottom));
}

InsetDrawable::InsetDrawable(
    /* [in] */ InsetState* state,
    /* [in] */ IResources* res)
    : mMutated(FALSE)
{
    CRect::NewByFriend((CRect**)&mTmpRect);
    mInsetState = new InsetState(state, this, res);
}

ECode InsetDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/
{
    Int32 size = ArraySize(R::styleable::InsetDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::InsetDrawable, size);
    AutoPtr<ITypedArray> a;
    ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
    FAIL_GOTO(ec, error);
    ec = Drawable::InflateWithAttributes(r, parser, a, R::styleable::InsetDrawable_visible);
    FAIL_GOTO(ec, error);

    mInsetState->mDrawable = NULL;
    ec = UpdateStateFromTypedArray(a);
    FAIL_GOTO(ec, error);

    // Load inner XML elements.
    if (mInsetState->mDrawable == NULL) {
        Int32 type = 0;
        while ((parser->Next(&type), type) == IXmlPullParser::TEXT) {
        }
        if (type != IXmlPullParser::START_TAG) {
            // throw new XmlPullParserException(
            //         parser.getPositionDescription()
            //                 + ": <inset> tag requires a 'drawable' attribute or "
            //                 + "child tag defining a drawable");
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        AutoPtr<IDrawable> dr;
        ec = Drawable::CreateFromXmlInner(r, parser, attrs, theme, (IDrawable**)&dr);
        FAIL_GOTO(ec, error);
        mInsetState->mDrawable = dr;
        dr->SetCallback(this);
    }

    ec = VerifyRequiredAttributes(a);

error:
    a->Recycle();
    return ec;
}

ECode InsetDrawable::VerifyRequiredAttributes(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    // If we're not waiting on a theme, verify required attributes.
    if (mInsetState->mDrawable == NULL && (mInsetState->mThemeAttrs == NULL
            || (*mInsetState->mThemeAttrs)[R::styleable::InsetDrawable_drawable] == 0)) {
        // throw new XmlPullParserException(a.getPositionDescription() +
        //         ": <inset> tag requires a 'drawable' attribute or "
        //         + "child tag defining a drawable");
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

ECode InsetDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    AutoPtr<InsetState> state = mInsetState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    state->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mThemeAttrs);

    Int32 N = 0, inset = 0, attr = 0;;
    AutoPtr<IDrawable> dr;
    a->GetIndexCount(&N);
    for (Int32 i = 0; i < N; i++) {
        attr = 0;
        a->GetIndex(i, &attr);
        switch (attr) {
            case R::styleable::InsetDrawable_drawable:
                dr = NULL;
                a->GetDrawable(attr, (IDrawable**)&dr);
                if (dr != NULL) {
                    state->mDrawable = dr;
                    dr->SetCallback(this);
                }
                break;
            case R::styleable::InsetDrawable_inset:
                a->GetDimensionPixelOffset(attr, Elastos::Core::Math::INT32_MIN_VALUE, &inset);
                if (inset != Elastos::Core::Math::INT32_MIN_VALUE) {
                    state->mInsetLeft = inset;
                    state->mInsetTop = inset;
                    state->mInsetRight = inset;
                    state->mInsetBottom = inset;
                }
                break;
            case R::styleable::InsetDrawable_insetLeft:
                a->GetDimensionPixelOffset(attr, state->mInsetLeft, &state->mInsetLeft);
                break;
            case R::styleable::InsetDrawable_insetTop:
                a->GetDimensionPixelOffset(attr, state->mInsetTop, &state->mInsetTop);
                break;
            case R::styleable::InsetDrawable_insetRight:
                a->GetDimensionPixelOffset(attr, state->mInsetRight, &state->mInsetRight);
                break;
            case R::styleable::InsetDrawable_insetBottom:
                a->GetDimensionPixelOffset(attr, state->mInsetBottom, &state->mInsetBottom);
                break;
        }
    }
    return NOERROR;
}

ECode InsetDrawable::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    Drawable::ApplyTheme(t);

    AutoPtr<InsetState> state = mInsetState;
    if (state == NULL || state->mThemeAttrs == NULL) {
        return NOERROR;
    }

    AutoPtr<ITypedArray> a;
    Int32 size = ArraySize(R::styleable::InsetDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::InsetDrawable, size);
    ((CResources::Theme*)t)->ResolveAttribute(state->mThemeAttrs, layout, (ITypedArray**)&a);
    // try {
    if (FAILED(UpdateStateFromTypedArray(a))) {
        a->Recycle();
        return E_RUNTIME_EXCEPTION;
    }
    if (FAILED(VerifyRequiredAttributes(a))) {
        a->Recycle();
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (XmlPullParserException e) {
    //     throw new RuntimeException(e);
    // } finally {
    return a->Recycle();
    // }
}

ECode InsetDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mInsetState != NULL && mInsetState->mThemeAttrs != NULL;
    return NOERROR;
}

ECode InsetDrawable::InvalidateDrawable(
    /* [in] */ IDrawable* who)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->InvalidateDrawable(this);
    }
    return NOERROR;
}

ECode InsetDrawable::ScheduleDrawable(
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

ECode InsetDrawable::UnscheduleDrawable(
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

ECode InsetDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    return mInsetState->mDrawable->Draw(canvas);
}

ECode InsetDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Int32 config;
    mInsetState->mDrawable->GetChangingConfigurations(&config);
    Drawable::GetChangingConfigurations(configuration);
    *configuration = (*configuration)
            | mInsetState->mChangingConfigurations
            | config;
    return NOERROR;
}

ECode InsetDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    VALIDATE_NOT_NULL(isPadding);
    Boolean pad;
    mInsetState->mDrawable->GetPadding(padding, &pad);

    ((CRect*)padding)->mLeft += mInsetState->mInsetLeft;
    ((CRect*)padding)->mRight += mInsetState->mInsetRight;
    ((CRect*)padding)->mTop += mInsetState->mInsetTop;
    ((CRect*)padding)->mBottom += mInsetState->mInsetBottom;

    *isPadding = (pad || (mInsetState->mInsetLeft | mInsetState->mInsetRight |
                mInsetState->mInsetTop | mInsetState->mInsetBottom) != 0);
    return NOERROR;
}

ECode InsetDrawable::GetOpticalInsets(
    /* [out] */ IInsets** sets)
{
    VALIDATE_NOT_NULL(sets);
    AutoPtr<IInsets> contentInsets;
    Drawable::GetOpticalInsets((IInsets**)&contentInsets);
    *sets = Insets::Of(((Insets*)contentInsets.Get())->mLeft + mInsetState->mInsetLeft,
            ((Insets*)contentInsets.Get())->mTop + mInsetState->mInsetTop,
            ((Insets*)contentInsets.Get())->mRight + mInsetState->mInsetRight,
            ((Insets*)contentInsets.Get())->mBottom + mInsetState->mInsetBottom);
    REFCOUNT_ADD(*sets);
    return NOERROR;
}

ECode InsetDrawable::SetHotspot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return mInsetState->mDrawable->SetHotspot(x, y);
}

ECode InsetDrawable::SetHotspotBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return mInsetState->mDrawable->SetHotspotBounds(left, top, right, bottom);
}

ECode InsetDrawable::GetHotspotBounds(
    /* [in] */ IRect* outRect)
{
    return mInsetState->mDrawable->GetHotspotBounds(outRect);
}

ECode InsetDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* isDifferent)
{
    VALIDATE_NOT_NULL(isDifferent);
    mInsetState->mDrawable->SetVisible(visible, restart, isDifferent);
    return Drawable::SetVisible(visible, restart, isDifferent);
}

ECode InsetDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return mInsetState->mDrawable->SetAlpha(alpha);
}

ECode InsetDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    return mInsetState->mDrawable->GetAlpha(alpha);
}

ECode InsetDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return mInsetState->mDrawable->SetColorFilter(cf);
}

ECode InsetDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    return mInsetState->mDrawable->SetTintList(tint);
}

ECode InsetDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    return mInsetState->mDrawable->SetTintMode(tintMode);
}

ECode InsetDrawable::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    return mInsetState->mDrawable->SetLayoutDirection(layoutDirection);
}

ECode InsetDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    return mInsetState->mDrawable->GetOpacity(opacity);
}

ECode InsetDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    return mInsetState->mDrawable->IsStateful(isStateful);
}

Boolean InsetDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* state)
{
    Boolean changed;
    mInsetState->mDrawable->SetState(
        const_cast<ArrayOf<Int32>*>(state), &changed);
    AutoPtr<IRect> rect;
    GetBounds((IRect**)&rect);
    OnBoundsChange(rect);
    return changed;
}

Boolean InsetDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Boolean result = FALSE;
    mInsetState->mDrawable->SetLevel(level, &result);
    return result;
}

void InsetDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    CRect* r = (CRect*)mTmpRect.Get();
    r->Set(bounds);

    r->mLeft += mInsetState->mInsetLeft;
    r->mTop += mInsetState->mInsetTop;
    r->mRight -= mInsetState->mInsetRight;
    r->mBottom -= mInsetState->mInsetBottom;

    mInsetState->mDrawable->SetBounds(r->mLeft, r->mTop, r->mRight, r->mBottom);
}

ECode InsetDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    return mInsetState->mDrawable->GetIntrinsicWidth(width);
}

ECode InsetDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    return mInsetState->mDrawable->GetIntrinsicHeight(height);
}

ECode InsetDrawable::GetOutline(
    /* [in] */ /*@NonNull*/ IOutline* outline)
{
    return mInsetState->mDrawable->GetOutline(outline);
}

ECode InsetDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    if (mInsetState->CanConstantState()) {
        GetChangingConfigurations(&mInsetState->mChangingConfigurations);
        *state = mInsetState;
        REFCOUNT_ADD(*state);
        return NOERROR;
    }
    *state = NULL;
    return NOERROR;
}

ECode InsetDrawable::Mutate()
{
    if (!mMutated) {
        mInsetState->mDrawable->Mutate();
        mMutated = TRUE;
    }
    return NOERROR;
}

ECode InsetDrawable::GetDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mInsetState->mDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode InsetDrawable::constructor()
{
    return constructor((InsetState*)NULL, (IResources*)NULL);
}

ECode InsetDrawable::constructor(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 inset)
{
    return constructor(drawable, inset, inset, inset, inset);
}

ECode InsetDrawable::constructor(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 insetLeft,
    /* [in] */ Int32 insetTop,
    /* [in] */ Int32 insetRight,
    /* [in] */ Int32 insetBottom)
{
    FAIL_RETURN(constructor((InsetState*)NULL, (IResources*)NULL));

    mInsetState->mDrawable = drawable;
    mInsetState->mInsetLeft = insetLeft;
    mInsetState->mInsetTop = insetTop;
    mInsetState->mInsetRight = insetRight;
    mInsetState->mInsetBottom = insetBottom;

    if (drawable != NULL) {
        drawable->SetCallback(
                this);
    }
    return NOERROR;
}

ECode InsetDrawable::constructor(
    /* [in] */ InsetState* state,
    /* [in] */ IResources* res)
{
    mInsetState = new InsetState(state, this, res);
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
