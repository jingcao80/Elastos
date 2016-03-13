
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/ShapeDrawable.h"
#include "elastos/droid/graphics/drawable/CShapeDrawable.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

ShapeDrawable::ShapeState::ShapeState(
    /* [in] */ ShapeState* orig)
    : mChangingConfigurations(0)
    , mTintMode(DEFAULT_TINT_MODE)
    , mIntrinsicWidth(0)
    , mIntrinsicHeight(0)
    , mAlpha(255)
{
    if (orig != NULL) {
        mThemeAttrs = orig->mThemeAttrs;
        mPaint = orig->mPaint;
        mShape = orig->mShape;
        mTint = orig->mTint;
        mTintMode = orig->mTintMode;
        mPadding = orig->mPadding;
        mIntrinsicWidth = orig->mIntrinsicWidth;
        mIntrinsicHeight = orig->mIntrinsicHeight;
        mAlpha = orig->mAlpha;
        mShaderFactory = orig->mShaderFactory;
    }
    else {
        CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mPaint);
    }
}

ECode ShapeDrawable::ShapeState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mThemeAttrs != NULL;
    return NOERROR;
}

ECode ShapeDrawable::ShapeState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CShapeDrawable::New(this, NULL, NULL, drawable);
}

ECode ShapeDrawable::ShapeState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CShapeDrawable::New(this, res, NULL, drawable);
}

ECode ShapeDrawable::ShapeState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return CShapeDrawable::New(this, res, theme, drawable);
}

ECode ShapeDrawable::ShapeState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ShapeDrawable, Drawable, IShapeDrawable);
ShapeDrawable::ShapeDrawable()
    : mMutated(FALSE)
{
}

ShapeDrawable::ShapeDrawable(
    /* [in] */ IShape* s)
    : mMutated(FALSE)
{
    constructor(s);
}

ShapeDrawable::ShapeDrawable(
    /* [in] */ ShapeState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
    : mMutated(FALSE)
{
    constructor(state, res, theme);
}

ECode ShapeDrawable::InitializeWithState(
    /* [in] */ ShapeState* state,
    /* [in] */ IResources* res)
{
    mTintFilter = UpdateTintFilter(mTintFilter, state->mTint, state->mTintMode);
    return NOERROR;
}

ECode ShapeDrawable::GetShape(
    /* [out] */ IShape** shape)
{
    VALIDATE_NOT_NULL(shape);
    *shape = mShapeState->mShape;
    REFCOUNT_ADD(*shape);
    return NOERROR;
}

ECode ShapeDrawable::SetShape(
    /* [in] */ IShape* s)
{
    mShapeState->mShape = s;
    UpdateShape();
    return NOERROR;
}

ECode ShapeDrawable::SetShaderFactory(
    /* [in] */ IShaderFactory* fact)
{
    mShapeState->mShaderFactory = fact;
    return NOERROR;
}

ECode ShapeDrawable::GetShaderFactory(
    /* [out] */ IShaderFactory** fact)
{
    VALIDATE_NOT_NULL(fact);
    *fact = mShapeState->mShaderFactory;
    REFCOUNT_ADD(*fact);
    return NOERROR;
}

ECode ShapeDrawable::GetPaint(
    /* [out] */ IPaint** paint)
{
    VALIDATE_NOT_NULL(paint);
    *paint = mShapeState->mPaint;
    REFCOUNT_ADD(*paint);
    return NOERROR;
}

ECode ShapeDrawable::SetPadding(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if ((left | top | right | bottom) == 0) {
        mShapeState->mPadding = NULL;
    }
    else {
        if (mShapeState->mPadding == NULL) {
             CRect::New((IRect**)&mShapeState->mPadding);
        }
        mShapeState->mPadding->Set(left, top, right, bottom);
    }
    return InvalidateSelf();
}

ECode ShapeDrawable::SetPadding(
    /* [in] */ IRect* padding)
{
    if (padding == NULL) {
        mShapeState->mPadding = NULL;
    }
    else {
        if (mShapeState->mPadding == NULL) {
            CRect::New((IRect**)&mShapeState->mPadding);
        }
        mShapeState->mPadding->Set(padding);
    }
    return InvalidateSelf();
}

ECode ShapeDrawable::SetIntrinsicWidth(
    /* [in] */ Int32 width)
{
    mShapeState->mIntrinsicWidth = width;
    return InvalidateSelf();
}

ECode ShapeDrawable::SetIntrinsicHeight(
    /* [in] */ Int32 height)
{
    mShapeState->mIntrinsicHeight = height;
    return InvalidateSelf();
}

ECode ShapeDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mShapeState->mIntrinsicWidth;
    return NOERROR;
}

ECode ShapeDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mShapeState->mIntrinsicHeight;
    return NOERROR;
}

ECode ShapeDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    VALIDATE_NOT_NULL(isPadding);
    if (mShapeState->mPadding != NULL) {
        padding->Set(mShapeState->mPadding);
        *isPadding = TRUE;
        return NOERROR;
    }

    return Drawable::GetPadding(padding, isPadding);
}

Int32 ShapeDrawable::ModulateAlpha(
    /* [in] */ Int32 paintAlpha,
    /* [in] */ Int32 alpha)
{
    UInt32 u = alpha;
    Int32 scale = alpha + (u >> 7);  // convert to 0..256
    u = paintAlpha * scale;
    return (Int32)(u >> 8);
}

void ShapeDrawable::OnDraw(
    /* [in] */ IShape* shape,
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    shape->Draw(canvas, paint);
}

ECode ShapeDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IRect> r;
    GetBounds((IRect**)&r);
    AutoPtr<ShapeState> state = mShapeState;
    AutoPtr<IPaint> paint = state->mPaint;

    Int32 prevAlpha = 0;
    paint->GetAlpha(&prevAlpha);
    paint->SetAlpha(ModulateAlpha(prevAlpha, state->mAlpha));

    // only draw shape if it may affect output
    Int32 value = 0;
    AutoPtr<IXfermode> xf;
    Boolean has = FALSE;
    paint->GetXfermode((IXfermode**)&xf);
    if ((paint->GetAlpha(&value), value) != 0 || xf != NULL || (paint->HasShadowLayer(&has), has)) {
        Boolean clearColorFilter = FALSE;
        AutoPtr<IColorFilter> cf;
        if (mTintFilter != NULL && (paint->GetColorFilter((IColorFilter**)&cf), cf.Get()) == NULL) {
            paint->SetColorFilter(IColorFilter::Probe(mTintFilter));
            clearColorFilter = TRUE;
        } else {
            clearColorFilter = FALSE;
        }

        if (state->mShape != NULL) {
            // need the save both for the translate, and for the (unknown)
            // Shape
            Int32 count = 0;
            canvas->Save(&count);
            canvas->Translate(((CRect*)r.Get())->mLeft, ((CRect*)r.Get())->mTop);
            OnDraw(state->mShape, canvas, paint);
            canvas->RestoreToCount(count);
        } else {
            canvas->DrawRect(r, paint);
        }

        if (clearColorFilter) {
            paint->SetColorFilter(NULL);
        }
    }

    // restore
    paint->SetAlpha(prevAlpha);
    return NOERROR;
}

ECode ShapeDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Drawable::GetChangingConfigurations(configuration);
    *configuration = (*configuration)
            | mShapeState->mChangingConfigurations;
    return NOERROR;
}

ECode ShapeDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    mShapeState->mAlpha = alpha;
    return InvalidateSelf();
}

ECode ShapeDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mShapeState->mAlpha;
    return NOERROR;
}

ECode ShapeDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    mShapeState->mTint = tint;
    mTintFilter = UpdateTintFilter(mTintFilter, tint, mShapeState->mTintMode);
    return InvalidateSelf();
}

ECode ShapeDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    mShapeState->mTintMode = tintMode;
    mTintFilter = UpdateTintFilter(mTintFilter, mShapeState->mTint, tintMode);
    return InvalidateSelf();
}

ECode ShapeDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    mShapeState->mPaint->SetColorFilter(cf);
    return InvalidateSelf();
}

ECode ShapeDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    if (mShapeState->mShape == NULL) {
        AutoPtr<IPaint> p = mShapeState->mPaint;
        AutoPtr<IXfermode> mode;
        if (p->GetXfermode((IXfermode**)&mode), mode == NULL) {
            Int32 alpha;
            p->GetAlpha(&alpha);
            if (alpha == 0) {
                *opacity = IPixelFormat::TRANSPARENT;
                return NOERROR;
            }
            if (alpha == 255) {
                *opacity = IPixelFormat::OPAQUE;
                return NOERROR;
            }
        }
    }
    // not sure, so be safe
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode ShapeDrawable::SetDither(
    /* [in] */ Boolean dither)
{
    mShapeState->mPaint->SetDither(dither);
    return InvalidateSelf();
}

void ShapeDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    Drawable::OnBoundsChange(bounds);
    UpdateShape();
}

Boolean ShapeDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>& stateSet)
{
    AutoPtr<ShapeState> state = mShapeState;
    if (state->mTint.Get() != NULL && state->mTintMode != -1) {
        mTintFilter = UpdateTintFilter(mTintFilter, state->mTint, state->mTintMode);
        return TRUE;
    }
    return FALSE;
}

ECode ShapeDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    AutoPtr<ShapeState> s = mShapeState;
    Boolean value = FALSE;
    *isStateful = (Drawable::IsStateful(isStateful), *isStateful) || (s->mTint != NULL && (s->mTint->IsStateful(&value), value));
    return NOERROR;
}

Boolean ShapeDrawable::InflateTag(
    /* [in] */ const String& name,
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs)
{
    if (name.Equals("padding")) {
        Int32 size = ArraySize(R::styleable::ShapeDrawablePadding);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::ShapeDrawablePadding, size);

        AutoPtr<ITypedArray> a;
        r->ObtainAttributes(attrs, layout, (ITypedArray**)&a);
        Int32 left, top, right, bottom;
        a->GetDimensionPixelOffset(
                R::styleable::ShapeDrawablePadding_left, 0, &left);
        a->GetDimensionPixelOffset(
                R::styleable::ShapeDrawablePadding_top, 0, &top);
        a->GetDimensionPixelOffset(
                R::styleable::ShapeDrawablePadding_right, 0, &right);
        a->GetDimensionPixelOffset(
                R::styleable::ShapeDrawablePadding_bottom, 0, &bottom);
        SetPadding(left, top, right, bottom);
        a->Recycle();
        return TRUE;
    }

    return FALSE;
}

ECode ShapeDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    FAIL_RETURN(Drawable::Inflate(r, parser, attrs, theme));

    Int32 size = ArraySize(R::styleable::ShapeDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::ShapeDrawable, size);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));
    FAIL_RETURN(UpdateStateFromTypedArray(a));

    a->Recycle();

    Int32 type, outerDepth, depth;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
           && (type != IXmlPullParser::END_TAG ||
              (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        parser->GetName(&name);
        // call our subclass
        if (!InflateTag(name, r, parser, attrs)) {
//            android.util.Log.w("drawable", "Unknown element: " + name +
//                    " for ShapeDrawable " + this);
        }
    }

    // Update local properties.
    return InitializeWithState(mShapeState, r);
}

ECode ShapeDrawable::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    Drawable::ApplyTheme(t);

    AutoPtr<ShapeState> state = mShapeState;
    if (state == NULL || state->mThemeAttrs == NULL) {
        return NOERROR;
    }

    Int32 size = ArraySize(R::styleable::ShapeDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::ShapeDrawable, size);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(((CResources::Theme*)t)->ResolveAttribute(state->mThemeAttrs, layout, (ITypedArray**)&a));
    ECode ec = UpdateStateFromTypedArray(a);
    if (FAILED(ec)) {
        a->Recycle();
        return ec;
    }
    a->Recycle();

    // Update local properties.
    AutoPtr<IResources> res;
    ((CResources::Theme*)t)->GetResources((IResources**)&res);
    return InitializeWithState(state, res);
}

ECode ShapeDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<ShapeState> state = mShapeState;
    AutoPtr<IPaint> paint = state->mPaint;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    state->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    FAIL_RETURN(((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mThemeAttrs));

    Int32 color = 0;
    paint->GetColor(&color);
    a->GetColor(R::styleable::ShapeDrawable_color, color, &color);
    paint->SetColor(color);

    Boolean dither = FALSE;
    paint->IsDither(&dither);
    a->GetBoolean(R::styleable::ShapeDrawable_dither, dither, &dither);
    paint->SetDither(dither);

    Float value = 0;
    a->GetDimension(R::styleable::ShapeDrawable_width, state->mIntrinsicWidth, &value);
    SetIntrinsicWidth((Int32) value);
    a->GetDimension(R::styleable::ShapeDrawable_height, state->mIntrinsicHeight, &value);
    SetIntrinsicHeight((Int32) value);

    Int32 tintMode = 0;
    a->GetInt32(R::styleable::ShapeDrawable_tintMode, -1, &tintMode);
    if (tintMode != -1) {
        Drawable::ParseTintMode(tintMode, PorterDuffMode_SRC_IN, &state->mTintMode);
    }

    AutoPtr<IColorStateList> tint;
    a->GetColorStateList(R::styleable::ShapeDrawable_tint, (IColorStateList**)&tint);
    if (tint != NULL) {
        state->mTint = tint;
    }
    return NOERROR;
}

void ShapeDrawable::UpdateShape()
{
    if (mShapeState->mShape != NULL) {
        AutoPtr<IRect> r;
        GetBounds((IRect**)&r);
        Int32 w, h;
        r->GetWidth(&w);
        r->GetHeight(&h);

        mShapeState->mShape->Resize(w, h);
        if (mShapeState->mShaderFactory != NULL) {
            AutoPtr<IShader> shader;
            mShapeState->mShaderFactory->Resize(w, h, (IShader**)&shader);
            mShapeState->mPaint->SetShader(shader);
        }
    }
    InvalidateSelf();
}

ECode ShapeDrawable::GetOutline(
    /* [in] */ IOutline* outline)
{
    if (mShapeState->mShape != NULL) {
        mShapeState->mShape->GetOutline(outline);
        Int32 alpha = 0;
        GetAlpha(&alpha);
        outline->SetAlpha(alpha / 255.0f);
    }
    return NOERROR;
}

ECode ShapeDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    Drawable::GetChangingConfigurations(&mShapeState->mChangingConfigurations);
    *state = (IDrawableConstantState*)mShapeState->Probe(EIID_IDrawableConstantState);
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode ShapeDrawable::Mutate()
{
    if (!mMutated) {
        AutoPtr<IPaint> paint;
        if (mShapeState->mPaint != NULL) {
            CPaint::New(mShapeState->mPaint, (IPaint**)&paint);
        }
        else {
            CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&paint);
        }
        mShapeState->mPaint = paint;

        AutoPtr<IRect> padding;
        if (mShapeState->mPadding != NULL) {
            CRect::New(mShapeState->mPadding, (IRect**)&padding);
        }
        else {
            CRect::New((IRect**)&padding);
        }
        mShapeState->mPadding = padding;

//        try {
        AutoPtr<IInterface> shape;
        ICloneable::Probe(mShapeState->mShape)->Clone((IInterface**)&shape);
        mShapeState->mShape = IShape::Probe(shape);
//        } catch (CloneNotSupportedException e) {
//            return null;
//        }
        mMutated = TRUE;
    }

    return NOERROR;
}

ECode ShapeDrawable::constructor()
{
    return constructor(new ShapeState(NULL), NULL, NULL);
}

ECode ShapeDrawable::constructor(
    /* [in] */ IShape* s)
{
    constructor(new ShapeState(NULL), NULL, NULL);
    mShapeState->mShape = s;
    return NOERROR;
}

ECode ShapeDrawable::constructor(
    /* [in] */ ShapeState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    Boolean can = FALSE;
    if (theme != NULL && (state->CanApplyTheme(&can), can)) {
        mShapeState = new ShapeState(state);
        ApplyTheme(theme);
    } else {
        mShapeState = state;
    }

    return InitializeWithState(state, res);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
