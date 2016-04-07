
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/GradientDrawable.h"
#include "elastos/droid/graphics/drawable/CGradientDrawable.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CDashPathEffect.h"
#include "elastos/droid/graphics/CLinearGradient.h"
#include "elastos/droid/graphics/CRadialGradient.h"
#include "elastos/droid/graphics/CSweepGradient.h"
#include "elastos/droid/R.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CResources.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::R;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

GradientDrawable::GradientState::GradientState(
    /* [in] */ GradientDrawableOrientation orientation,
    /* [in] */ ArrayOf<Int32>* colors)
    : mChangingConfigurations(0)
    , mShape(IGradientDrawable::RECTANGLE)
    , mGradient(IGradientDrawable::LINEAR_GRADIENT)
    , mAngle(0)
    , mOrientation(orientation)
    , mStrokeWidth(-1)
    , mStrokeDashWidth(0)
    , mStrokeDashGap(0)
    , mRadius(0)
    , mWidth(-1)
    , mHeight(-1)
    , mInnerRadiusRatio(DEFAULT_INNER_RADIUS_RATIO)
    , mThicknessRatio(DEFAULT_THICKNESS_RATIO)
    , mInnerRadius(-1)
    , mDither(FALSE)
    , mThickness(-1)
    , mCenterX(0.5f)
    , mCenterY(0.5f)
    , mGradientRadius(0.5f)
    , mGradientRadiusType(RADIUS_TYPE_PIXELS)
    , mUseLevel(FALSE)
    , mUseLevelForShape(FALSE)
    , mOpaqueOverBounds(FALSE)
    , mOpaqueOverShape(FALSE)
{
    SetColors(colors);
}

GradientDrawable::GradientState::GradientState(
    /* [in] */ GradientState* state)
    : mChangingConfigurations(state->mChangingConfigurations)
    , mShape(state->mShape)
    , mGradient(state->mGradient)
    , mAngle(state->mAngle)
    , mOrientation(state->mOrientation)
    , mColorStateList(state->mColorStateList)
    , mStrokeColorStateList(state->mStrokeColorStateList)
    , mStrokeWidth(state->mStrokeWidth)
    , mStrokeDashWidth(state->mStrokeDashWidth)
    , mStrokeDashGap(state->mStrokeDashGap)
    , mRadius(state->mRadius)
    , mWidth(state->mWidth)
    , mHeight(state->mHeight)
    , mInnerRadiusRatio(state->mInnerRadiusRatio)
    , mThicknessRatio(state->mThicknessRatio)
    , mInnerRadius(state->mInnerRadius)
    , mThickness(state->mThickness)
    , mThemeAttrs(state->mThemeAttrs)
    , mAttrSize(state->mAttrSize)
    , mAttrGradient(state->mAttrGradient)
    , mAttrSolid(state->mAttrSolid)
    , mAttrStroke(state->mAttrStroke)
    , mAttrCorners(state->mAttrCorners)
    , mAttrPadding(state->mAttrPadding)
    , mCenterX(state->mCenterX)
    , mCenterY(state->mCenterY)
    , mGradientRadius(state->mGradientRadius)
    , mGradientRadiusType(state->mGradientRadiusType)
    , mUseLevel(state->mUseLevel)
    , mUseLevelForShape(state->mUseLevelForShape)
    , mOpaqueOverBounds(state->mOpaqueOverBounds)
    , mOpaqueOverShape(state->mOpaqueOverShape)
{
    if (state->mColors != NULL) {
        mColors = state->mColors->Clone();
    }
    if (state->mPositions != NULL) {
        mPositions = state->mPositions->Clone();
    }
    if (state->mRadiusArray != NULL) {
        mRadiusArray = state->mRadiusArray->Clone();
    }
    if (state->mPadding != NULL) {
        CRect::New(state->mPadding, (IRect**)&mPadding);
    }
}

GradientDrawable::GradientState::~GradientState()
{
}

ECode GradientDrawable::GradientState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mThemeAttrs != NULL;
    return NOERROR;
}

ECode GradientDrawable::GradientState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CGradientDrawable::New(this, NULL, drawable);
}

ECode GradientDrawable::GradientState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CGradientDrawable::New(this, NULL, drawable);
}

ECode GradientDrawable::GradientState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return CGradientDrawable::New(this, theme, drawable);
}

ECode GradientDrawable::GradientState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);

    *config = mChangingConfigurations;
    return NOERROR;
}

void GradientDrawable::GradientState::SetShape(
    /* [in] */ Int32 shape)
{
    mShape = shape;
    ComputeOpacity();
}

void GradientDrawable::GradientState::SetGradientType(
    /* [in] */ Int32 gradient)
{
    mGradient = gradient;
}

void GradientDrawable::GradientState::SetGradientCenter(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mCenterX = x;
    mCenterY = y;
}

void GradientDrawable::GradientState::SetColors(
    /* [in] */ ArrayOf<Int32>* colors)
{
    mColors = colors;
    mColorStateList = NULL;
    ComputeOpacity();
}

void GradientDrawable::GradientState::SetColorStateList(
    /* [in] */ IColorStateList* colorStateList)
{
    mColors = NULL;
    mColorStateList = colorStateList;
    ComputeOpacity();
}

void GradientDrawable::GradientState::ComputeOpacity()
{
    mOpaqueOverBounds = FALSE;
    mOpaqueOverShape = FALSE;

    if (mColors != NULL) {
        for (Int32 i = 0; i < mColors->GetLength(); i++) {
            if (!IsOpaque((*mColors)[i])) {
                return;
            }
        }
    }

    // An unfilled shape is not opaque over bounds or shape
    if (mColors == NULL && mColorStateList == NULL) {
        return;
    }

    // Colors are opaque, so opaqueOverShape=true,
    mOpaqueOverShape = TRUE;
    // and opaqueOverBounds=true if shape fills bounds
    mOpaqueOverBounds = mShape == RECTANGLE
            && mRadius <= 0
            && mRadiusArray == NULL;
}

Boolean GradientDrawable::GradientState::IsOpaque(
    /* [in] */ Int32 color)
{
    return ((color >> 24) & 0xff) == 0xff;
}

void GradientDrawable::GradientState::SetStroke(
    /* [in] */ Int32 width,
    /* [in] */ IColorStateList* colorStateList,
    /* [in] */ Float dashWidth,
    /* [in] */ Float dashGap)
{
    mStrokeWidth = width;
    mStrokeColorStateList = colorStateList;
    mStrokeDashWidth = dashWidth;
    mStrokeDashGap = dashGap;
    ComputeOpacity();
}

void GradientDrawable::GradientState::SetCornerRadius(
    /* [in] */ Float radius)
{
    if (radius < 0) {
        radius = 0;
    }
    mRadius = radius;
    mRadiusArray = NULL;
}

void GradientDrawable::GradientState::SetCornerRadii(
    /* [in] */ ArrayOf<Float>* radii)
{
    mRadiusArray = radii;
    if (radii == NULL) {
        mRadius = 0;
    }
}

void GradientDrawable::GradientState::SetSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mWidth = width;
    mHeight = height;
}

void GradientDrawable::GradientState::SetGradientRadius(
    /* [in] */ Float gradientRadius,
    /* [in] */ Int32 type)
{
    mGradientRadius = gradientRadius;
    mGradientRadiusType = type;
}


const Int32 GradientDrawable::RADIUS_TYPE_PIXELS = 0;
const Int32 GradientDrawable::RADIUS_TYPE_FRACTION = 1;
const Int32 GradientDrawable::RADIUS_TYPE_FRACTION_PARENT = 2;
const Float GradientDrawable::DEFAULT_INNER_RADIUS_RATIO = 3.0f;
const Float GradientDrawable::DEFAULT_THICKNESS_RATIO = 9.0f;
CAR_INTERFACE_IMPL(GradientDrawable, Drawable, IGradientDrawable)
GradientDrawable::GradientDrawable()
    : mAlpha(0xFF)
    , mGradientIsDirty(FALSE)
    , mMutated(FALSE)
    , mPathIsDirty(TRUE)
    , mGradientRadius(0)
{
    CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mFillPaint);
    CPath::New((IPath**)&mPath);
    CRectF::New((IRectF**)&mRect);
}

GradientDrawable::GradientDrawable(
    /* [in] */ GradientDrawableOrientation orientation,
    /* [in] */ ArrayOf<Int32>* colors)
    : mAlpha(0xFF)
    , mGradientIsDirty(FALSE)
    , mMutated(FALSE)
    , mPathIsDirty(TRUE)
    , mGradientRadius(0)
{
    CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mFillPaint);
    CPath::New((IPath**)&mPath);
    CRectF::New((IRectF**)&mRect);
    AutoPtr<GradientState> state = new GradientState(orientation, colors);
    constructor(state, NULL);
}

ECode GradientDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    VALIDATE_NOT_NULL(isPadding);
    if (mPadding != NULL) {
        padding->Set(mPadding);
        *isPadding = TRUE;
        return NOERROR;
    }
    return  Drawable::GetPadding(padding, isPadding);
}

ECode GradientDrawable::SetCornerRadii(
    /* [in] */ ArrayOf<Float>* radii)
{
    mGradientState->SetCornerRadii(radii);
    mPathIsDirty = TRUE;
    return InvalidateSelf();
}

ECode GradientDrawable::SetCornerRadius(
    /* [in] */ Float radius)
{
    mGradientState->SetCornerRadius(radius);
    mPathIsDirty = TRUE;
    return InvalidateSelf();
}

ECode GradientDrawable::SetStroke(
    /* [in] */ Int32 width,
    /* [in] */ Int32 color)
{
    return SetStroke(width, color, 0, 0);
}

ECode GradientDrawable::SetStroke(
    /* [in] */ Int32 width,
    /* [in] */ Int32 color,
    /* [in] */ Float dashWidth,
    /* [in] */ Float dashGap)
{
    AutoPtr<IColorStateList> csl;
    CColorStateList::ValueOf(color, (IColorStateList**)&csl);
    mGradientState->SetStroke(width, csl, dashWidth, dashGap);
    return SetStrokeInternal(width, color, dashWidth, dashGap);
}

ECode GradientDrawable::SetStroke(
    /* [in] */ Int32 width,
    /* [in] */ IColorStateList* colorStateList,
    /* [in] */ Float dashWidth,
    /* [in] */ Float dashGap)
{
    mGradientState->SetStroke(width, colorStateList, dashWidth, dashGap);
    Int32 color;
    if (colorStateList == NULL) {
        color = IColor::TRANSPARENT;
    } else {
        AutoPtr<ArrayOf<Int32> > stateSet;
        GetState((ArrayOf<Int32>**)&stateSet);
        colorStateList->GetColorForState(stateSet, 0, &color);
    }
    return SetStrokeInternal(width, color, dashWidth, dashGap);
}

ECode GradientDrawable::SetStrokeInternal(
    /* [in] */ Int32 width,
    /* [in] */ Int32 color,
    /* [in] */ Float dashWidth,
    /* [in] */ Float dashGap)
{
    if (mStrokePaint == NULL)  {
        CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mStrokePaint);
        mStrokePaint->SetStyle(PaintStyle_STROKE);
    }
    mStrokePaint->SetStrokeWidth(width);
    mStrokePaint->SetColor(color);

    AutoPtr<IDashPathEffect> e;
    if (dashWidth > 0) {
        AutoPtr<ArrayOf<Float> > intervals = ArrayOf<Float>::Alloc(2);
        (*intervals)[0] = dashWidth;
        (*intervals)[1] = dashGap;
        CDashPathEffect::New(*intervals, 0, (IDashPathEffect**)&e);
    }
    mStrokePaint->SetPathEffect(IPathEffect::Probe(e));
    return InvalidateSelf();
}

ECode GradientDrawable::SetStroke(
    /* [in] */ Int32 width,
    /* [in] */ IColorStateList* colorStateList)
{
    return SetStroke(width, colorStateList, 0, 0);
}

ECode GradientDrawable::SetSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mGradientState->SetSize(width, height);
    mPathIsDirty = TRUE;
    return InvalidateSelf();
}

ECode GradientDrawable::SetShape(
    /* [in] */ Int32 shape)
{
    mRingPath = NULL;
    mPathIsDirty = TRUE;
    mGradientState->SetShape(shape);
    return InvalidateSelf();
}

ECode GradientDrawable::SetGradientType(
    /* [in] */ Int32 gradient)
{
    mGradientState->SetGradientType(gradient);
    mGradientIsDirty  = TRUE;
    return InvalidateSelf();
}

ECode GradientDrawable::SetGradientCenter(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mGradientState->SetGradientCenter(x, y);
    mGradientIsDirty  = TRUE;
    return InvalidateSelf();
}

ECode GradientDrawable::SetGradientRadius(
    /* [in] */ Float gradientRadius)
{
    mGradientState->SetGradientRadius(gradientRadius, ITypedValue::COMPLEX_UNIT_PX);
    mGradientIsDirty  = TRUE;
    return InvalidateSelf();
}

Boolean GradientDrawable::IsOpaque(
    /* [in] */ Int32 color)
{
    return ((color >> 24) & 0xff) == 0xff;
}

ECode GradientDrawable::GetGradientRadius(
    /* [out] */ Float* gradientRadius)
{
    VALIDATE_NOT_NULL(gradientRadius);
    if (mGradientState->mGradient != RADIAL_GRADIENT) {
        *gradientRadius = 0;
        return NOERROR;
    }

    EnsureValidRect();
    *gradientRadius = mGradientRadius;
    return NOERROR;
}

ECode GradientDrawable::SetUseLevel(
    /* [in] */ Boolean useLevel)
{
    mGradientState->mUseLevel = useLevel;
    mGradientIsDirty  = TRUE;
    return InvalidateSelf();
}

Int32 GradientDrawable::ModulateAlpha(
    /* [in] */ Int32 alpha)
{
    Int32 scale = mAlpha + (mAlpha >> 7);
    return alpha * scale >> 8;
}

ECode GradientDrawable::GetOrientation(
    /*[out] */ GradientDrawableOrientation* orientation)
{
    assert(orientation != NULL);
    *orientation = mGradientState->mOrientation;
    return NOERROR;
}

ECode GradientDrawable::SetOrientation(
    /* [in] */ GradientDrawableOrientation orientation)
{
    mGradientState->mOrientation = orientation;
    mGradientIsDirty  = TRUE;
    return InvalidateSelf();
}

ECode GradientDrawable::SetColors(
    /* [in] */ ArrayOf<Int32>* colors)
{
    mGradientState->SetColors(colors);
    mGradientIsDirty  = TRUE;
    return InvalidateSelf();
}

ECode GradientDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (!EnsureValidRect()) {
        // nothing to draw
        return NOERROR;
    }

    // remember the alpha values, in case we temporarily overwrite them
    // when we modulate them with mAlpha
    Int32 prevFillAlpha;
    mFillPaint->GetAlpha(&prevFillAlpha);
    Int32 prevStrokeAlpha = 0;
    if (mStrokePaint != NULL) mStrokePaint->GetAlpha(&prevStrokeAlpha);
    // compute the modulate alpha values
    Int32 currFillAlpha = ModulateAlpha(prevFillAlpha);
    Int32 currStrokeAlpha = ModulateAlpha(prevStrokeAlpha);

    Float width;
    Boolean haveStroke = currStrokeAlpha > 0 && mStrokePaint != NULL && (mStrokePaint->GetStrokeWidth(&width), width > 0);
    Boolean haveFill = currFillAlpha > 0;
    AutoPtr<GradientState> st = mGradientState;
    /*  we need a layer iff we're drawing both a fill and stroke, and the
        stroke is non-opaque, and our shapetype actually supports
        fill+stroke. Otherwise we can just draw the stroke (if any) on top
        of the fill (if any) without worrying about blending artifacts.
    */
    Boolean useLayer = haveStroke && haveFill && st->mShape != IGradientDrawable::LINE &&
            currStrokeAlpha < 255 && (mAlpha < 255 || mColorFilter != NULL);

    /*  Drawing with a layer is slower than direct drawing, but it
        allows us to apply paint effects like alpha and colorfilter to
        the result of multiple separate draws. In our case, if the user
        asks for a non-opaque alpha value (via setAlpha), and we're
        stroking, then we need to apply the alpha AFTER we've drawn
        both the fill and the stroke.
    */
    if (useLayer) {
        if (mLayerPaint == NULL) {
            CPaint::New((IPaint**)&mLayerPaint);
        }
        mLayerPaint->SetDither(st->mDither);
        mLayerPaint->SetAlpha(mAlpha);
        mLayerPaint->SetColorFilter(mColorFilter);

        Float rad, l, t, r, b;
        mStrokePaint->GetStrokeWidth(&rad);
        mRect->Get(&l, &t, &r, &b);
        Int32 count;
        canvas->SaveLayer(l - rad, t - rad, r + rad, b + rad,
            mLayerPaint, ICanvas::HAS_ALPHA_LAYER_SAVE_FLAG, &count);

        // don't perform the filter in our individual paints
        // since the layer will do it for us
        mFillPaint->SetColorFilter(NULL);
        mStrokePaint->SetColorFilter(NULL);
    }
    else {
        /*  if we're not using a layer, apply the dither/filter to our
            individual paints
        */
        mFillPaint->SetAlpha(currFillAlpha);
        mFillPaint->SetDither(st->mDither);
        mFillPaint->SetColorFilter(mColorFilter);
        if (mColorFilter != NULL && st->mColorStateList == NULL) {
            mFillPaint->SetColor(mAlpha << 24);
        }
        if (haveStroke) {
            mStrokePaint->SetAlpha(currStrokeAlpha);
            mStrokePaint->SetDither(st->mDither);
            mStrokePaint->SetColorFilter(mColorFilter);
        }
    }

    switch (st->mShape) {
        case IGradientDrawable::RECTANGLE:
            if (st->mRadiusArray != NULL) {
                BuildPathIfDirty();
                canvas->DrawPath(mPath, mFillPaint);
                if (haveStroke) {
                    canvas->DrawPath(mPath, mStrokePaint);
                }
            }
            else if (st->mRadius > 0.0f) {
                // since the caller is only giving us 1 value, we will force
                // it to be square if the rect is too small in one dimension
                // to show it. If we did nothing, Skia would clamp the rad
                // independently along each axis, giving us a thin ellipse
                // if the rect were very wide but not very tall
                Float width, height;
                mRect->GetWidth(&width);
                mRect->GetHeight(&height);
                Float rad = Elastos::Core::Math::Min(st->mRadius,
                        Elastos::Core::Math::Min(width, height) * 0.5f);

                canvas->DrawRoundRect(mRect, rad, rad, mFillPaint);
                if (haveStroke) {
                    canvas->DrawRoundRect(mRect, rad, rad, mStrokePaint);
                }
            }
            else {
                Int32 color = 0;
                AutoPtr<IShader> shader;
                if ((mFillPaint->GetColor(&color), color) != 0 || mColorFilter != NULL ||
                        (mFillPaint->GetShader((IShader**)&shader), shader) != NULL) {
                    canvas->DrawRect(mRect, mFillPaint);
                }
                if (haveStroke) {
                    canvas->DrawRect(mRect, mStrokePaint);
                }
            }
            break;
        case IGradientDrawable::OVAL:
            canvas->DrawOval(mRect, mFillPaint);
            if (haveStroke) {
                canvas->DrawOval(mRect, mStrokePaint);
            }
            break;
        case IGradientDrawable::LINE: {
            CRectF* r = (CRectF*)mRect.Get();
            Float y;
            r->GetCenterY(&y);
            if (haveStroke) {
                canvas->DrawLine(r->mLeft, y, r->mRight, y, mStrokePaint);
            }
            break;
        }
        case IGradientDrawable::RING:
            AutoPtr<IPath> path = BuildRing(st);
            canvas->DrawPath(path, mFillPaint);
            if (haveStroke) {
                canvas->DrawPath(path, mStrokePaint);
            }
            break;
    }

    if (useLayer) {
        canvas->Restore();
    }
    else {
        mFillPaint->SetAlpha(prevFillAlpha);
        if (haveStroke) {
            mStrokePaint->SetAlpha(prevStrokeAlpha);
        }
    }
    return NOERROR;
}

void GradientDrawable::BuildPathIfDirty()
{
    AutoPtr<GradientState> st = mGradientState;
    if (mPathIsDirty) {
        EnsureValidRect();
        mPath->Reset();
        mPath->AddRoundRect(mRect, st->mRadiusArray, PathDirection_CW);
        mPathIsDirty = FALSE;
    }
}

AutoPtr<IPath> GradientDrawable::BuildRing(
    /* [in] */ GradientState* st)
{
    if (mRingPath != NULL && (!st->mUseLevelForShape || !mPathIsDirty)) return mRingPath;
    mPathIsDirty = FALSE;

    Int32 level = 0;
    GetLevel(&level);
    Float sweep = st->mUseLevelForShape ? (360.0f * level / 10000.0f) : 360.0f;

    AutoPtr<IRectF> bounds;
    CRectF::New(mRect, (IRectF**)&bounds);

    Float w, h, x, y;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);
    x = w /2.0f;
    y = h /2.0f;

    Float thickness = st->mThickness != -1 ?
            st->mThickness : w / st->mThicknessRatio;
    // inner radius
    Float radius = st->mInnerRadius != -1 ?
            st->mInnerRadius : w / st->mInnerRadiusRatio;

    AutoPtr<IRectF> innerBounds;
    CRectF::New(bounds, (IRectF**)&innerBounds);
    innerBounds->Inset(x - radius, y - radius);

    bounds = NULL;
    CRectF::New(innerBounds, (IRectF**)&bounds);
    bounds->Inset(-thickness, -thickness);

    if (mRingPath == NULL) {
        CPath::New((IPath**)&mRingPath);
    }
    else {
        mRingPath->Reset();
    }

    AutoPtr<IPath> ringPath = mRingPath;
    // arcTo treats the sweep angle mod 360, so check for that, since we
    // think 360 means draw the entire oval
    if (sweep < 360 && sweep > -360) {
        ringPath->SetFillType(PathFillType_EVEN_ODD);
        // inner top
        ringPath->MoveTo(x + radius, y);
        // outer top
        ringPath->LineTo(x + radius + thickness, y);
        // outer arc
        ringPath->ArcTo(bounds, 0.0f, sweep, FALSE);
        // inner arc
        ringPath->ArcTo(innerBounds, sweep, -sweep, FALSE);
        ringPath->Close();
    }
    else {
        // add the entire ovals
        ringPath->AddOval(bounds, PathDirection_CW);
        ringPath->AddOval(innerBounds, PathDirection_CCW);
    }

    return ringPath;
}

ECode GradientDrawable::SetColor(
    /* [in] */ Int32 argb)
{
    AutoPtr<IColorStateList> csl;
    CColorStateList::ValueOf(argb, (IColorStateList**)&csl);
    mGradientState->SetColorStateList(csl);
    mFillPaint->SetColor(argb);
    return InvalidateSelf();
}

ECode GradientDrawable::SetColor(
    /* [in] */ IColorStateList* colorStateList)
{
    mGradientState->SetColorStateList(colorStateList);
    Int32 color;
    if (colorStateList == NULL) {
        color = IColor::TRANSPARENT;
    } else {
        AutoPtr<ArrayOf<Int32> > stateSet;
        GetState((ArrayOf<Int32>**)&stateSet);
        colorStateList->GetColorForState(stateSet, 0, &color);
    }
    mFillPaint->SetColor(color);
    return InvalidateSelf();
}

Boolean GradientDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* stateSet)
{
    Boolean invalidateSelf = FALSE;

    AutoPtr<GradientState> s = mGradientState;
    AutoPtr<IColorStateList> stateList = s->mColorStateList;
    if (stateList != NULL) {
        Int32 newColor = 0;
        stateList->GetColorForState((ArrayOf<Int32>*)stateSet, 0, &newColor);
        Int32 oldColor = 0;
        mFillPaint->GetColor(&oldColor);
        if (oldColor != newColor) {
            mFillPaint->SetColor(newColor);
            invalidateSelf = TRUE;
        }
    }

    AutoPtr<IPaint> strokePaint = mStrokePaint;
    if (strokePaint != NULL) {
        AutoPtr<IColorStateList> strokeStateList = s->mStrokeColorStateList;
        if (strokeStateList != NULL) {
            Int32 newStrokeColor = 0;
            strokeStateList->GetColorForState((ArrayOf<Int32>*)stateSet, 0, &newStrokeColor);
            Int32 oldStrokeColor = 0;
            strokePaint->GetColor(&oldStrokeColor);
            if (oldStrokeColor != newStrokeColor) {
                strokePaint->SetColor(newStrokeColor);
                invalidateSelf = TRUE;
            }
        }
    }

    if (invalidateSelf) {
        InvalidateSelf();
        return TRUE;
    }

    return FALSE;
}

ECode GradientDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    AutoPtr<GradientState> s = mGradientState;
    Boolean sf1 = FALSE, sf2 = FALSE;
    Drawable::IsStateful(isStateful);
    *isStateful = (*isStateful)
            || (s->mColorStateList != NULL && (s->mColorStateList->IsStateful(&sf1), sf1))
            || (s->mStrokeColorStateList != NULL && (s->mStrokeColorStateList->IsStateful(&sf2), sf2));
    return NOERROR;
}

ECode GradientDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Drawable::GetChangingConfigurations(configuration);
    *configuration = (*configuration)
            | mGradientState->mChangingConfigurations;
    return NOERROR;
}

ECode GradientDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    if (alpha != mAlpha) {
        mAlpha = alpha;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode GradientDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode GradientDrawable::SetDither(
    /* [in] */ Boolean dither)
{
    if (dither != mGradientState->mDither) {
        mGradientState->mDither = dither;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode GradientDrawable::GetColorFilter(
    /* [out] */ IColorFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    *filter = mColorFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode GradientDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    if (cf != mColorFilter) {
        mColorFilter = cf;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode GradientDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = (mAlpha == 255 && mGradientState->mOpaqueOverBounds && IsOpaqueForState()) ?
            IPixelFormat::OPAQUE : IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

void GradientDrawable::OnBoundsChange(
    /* [in] */ IRect* r)
{
    Drawable::OnBoundsChange(r);
    mRingPath = NULL;
    mPathIsDirty = TRUE;
    mGradientIsDirty  = TRUE;
}

Boolean GradientDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Drawable::OnLevelChange(level);
    mGradientIsDirty  = TRUE;
    mPathIsDirty = TRUE;
    InvalidateSelf();
    return TRUE;
}

Boolean GradientDrawable::EnsureValidRect()
{
    if (mGradientIsDirty ) {
        mGradientIsDirty  = FALSE;

        AutoPtr<IRect> rect;
        GetBounds((IRect**)&rect);
        AutoPtr<CRect> bounds = (CRect*)rect.Get();
        Float inset = 0;

        if (mStrokePaint != NULL) {
            mStrokePaint->GetStrokeWidth(&inset);
            inset = inset * 0.5f;
        }

        GradientState* st = mGradientState;

        mRect->Set(bounds->mLeft + inset, bounds->mTop + inset,
                   bounds->mRight - inset, bounds->mBottom - inset);

        ArrayOf<Int32>* colors = st->mColors;
        if (colors != NULL) {
            CRectF* r = (CRectF*)mRect.Get();
            Float x0, x1, y0, y1;

            if (st->mGradient == IGradientDrawable::LINEAR_GRADIENT) {
                Int32 tmp = 0;
                GetLevel(&tmp);
                Float level = st->mUseLevel ? tmp / 10000.0f : 1.0f;
                switch (st->mOrientation) {
                case GradientDrawableOrientation_TOP_BOTTOM:
                    x0 = r->mLeft;          y0 = r->mTop;
                    x1 = x0;                y1 = level * r->mBottom;
                    break;
                case GradientDrawableOrientation_TR_BL:
                    x0 = r->mRight;         y0 = r->mTop;
                    x1 = level * r->mLeft;  y1 = level * r->mBottom;
                    break;
                case GradientDrawableOrientation_RIGHT_LEFT:
                    x0 = r->mRight;         y0 = r->mTop;
                    x1 = level * r->mLeft;  y1 = y0;
                    break;
                case GradientDrawableOrientation_BR_TL:
                    x0 = r->mRight;         y0 = r->mBottom;
                    x1 = level * r->mLeft;  y1 = level * r->mTop;
                    break;
                case GradientDrawableOrientation_BOTTOM_TOP:
                    x0 = r->mLeft;          y0 = r->mBottom;
                    x1 = x0;                y1 = level * r->mTop;
                    break;
                case GradientDrawableOrientation_BL_TR:
                    x0 = r->mLeft;          y0 = r->mBottom;
                    x1 = level * r->mRight; y1 = level * r->mTop;
                    break;
                case GradientDrawableOrientation_LEFT_RIGHT:
                    x0 = r->mLeft;          y0 = r->mTop;
                    x1 = level * r->mRight; y1 = y0;
                    break;
                default:/* TL_BR */
                    x0 = r->mLeft;          y0 = r->mTop;
                    x1 = level * r->mRight; y1 = level * r->mBottom;
                    break;
                }

                AutoPtr<IShader> shader;
                CLinearGradient::New(x0, y0, x1, y1, *colors,
                        st->mPositions, ShaderTileMode_CLAMP, (IShader**)&shader);
                mFillPaint->SetShader(shader);
            }
            else if (st->mGradient == IGradientDrawable::RADIAL_GRADIENT) {
                x0 = r->mLeft + (r->mRight - r->mLeft) * st->mCenterX;
                y0 = r->mTop + (r->mBottom - r->mTop) * st->mCenterY;

                Float radius = st->mGradientRadius;
                if (st->mGradientRadiusType == RADIUS_TYPE_FRACTION) {
                    radius *= Elastos::Core::Math::Min(st->mWidth, st->mHeight);
                } else if (st->mGradientRadiusType == RADIUS_TYPE_FRACTION_PARENT) {
                    Float width = 0, height = 0;
                    r->GetWidth(&width);
                    r->GetHeight(&height);
                    radius *= Elastos::Core::Math::Min(width, height);
                }

                if (st->mUseLevel) {
                    Int32 level = 0;
                    GetLevel(&level);
                    radius *= level / 10000.0f;
                }

                mGradientRadius = radius;

                if (radius == 0) {
                    // We can't have a shader with zero radius, so let's
                    // have a very, very small radius.
                    radius = 0.001f;
                }

                AutoPtr<IRadialGradient> shader;
                CRadialGradient::New(
                        x0, y0, radius, *colors, NULL, ShaderTileMode_CLAMP, (IRadialGradient**)&shader);
                mFillPaint->SetShader(IShader::Probe(shader));
            }
            else if (st->mGradient == IGradientDrawable::SWEEP_GRADIENT) {
                x0 = r->mLeft + (r->mRight - r->mLeft) * st->mCenterX;
                y0 = r->mTop + (r->mBottom - r->mTop) * st->mCenterY;

                AutoPtr<ArrayOf<Int32> > tempColors = colors;
                AutoPtr<ArrayOf<Float> > tempPositions;

                if (st->mUseLevel) {
                    tempColors = st->mTempColors;
                    Int32 length = colors->GetLength();
                    if (tempColors == NULL || tempColors->GetLength() != length + 1) {
                        tempColors = st->mTempColors = ArrayOf<Int32>::Alloc(length + 1);
                    }
                    tempColors->Copy(colors, length);
                    (*tempColors)[length] = (*colors)[length - 1];

                    tempPositions = st->mTempPositions;
                    Float fraction = 1.0f / (length - 1);
                    if (tempPositions == NULL || tempPositions->GetLength() != length + 1) {
                        tempPositions = st->mTempPositions = ArrayOf<Float>::Alloc(length + 1);
                    }

                    Int32 tmp = 0;
                    GetLevel(&tmp);
                    Float level = tmp / 10000.0f;
                    for (Int32 i = 0; i < length; i++) {
                        (*tempPositions)[i] = i * fraction * level;
                    }
                    (*tempPositions)[length] = 1.0f;

                }
                AutoPtr<IShader> shader;
                CSweepGradient::New(x0, y0, *tempColors, tempPositions, (IShader**)&shader);
                mFillPaint->SetShader(shader);
            }

            // If we don't have a solid color, the alpha channel must be
            // maxed out so that alpha modulation works correctly.
            if (st->mColorStateList == NULL) {
                mFillPaint->SetColor(IColor::BLACK);
            }
        }
    }
    Boolean empty;
    mRect->IsEmpty(&empty);
    return !empty;
}

ECode GradientDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    Int32 size = ArraySize(R::styleable::GradientDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::GradientDrawable, size);

    AutoPtr<ITypedArray> a;
    ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
    FAIL_GOTO(ec, error);
    ec = Drawable::InflateWithAttributes(r, parser, a, R::styleable::GradientDrawable_visible);
    FAIL_GOTO(ec, error);
    ec = UpdateStateFromTypedArray(a);

error:
    a->Recycle();
    if (ec != NOERROR) {
        return ec;
    }

    FAIL_RETURN(InflateChildElements(r, parser, attrs, theme));
    mGradientState->ComputeOpacity();
    return NOERROR;
}

ECode GradientDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<GradientState> state = mGradientState;

    // Account for any configuration changes.
    Int32 configuration = 0;
    a->GetChangingConfigurations(&configuration);
    state->mChangingConfigurations |= configuration;

    // Extract the theme attributes, if any.
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mThemeAttrs);

    a->GetInt32(R::styleable::GradientDrawable_shape, state->mShape, &state->mShape);
    a->GetBoolean(R::styleable::GradientDrawable_dither, state->mDither, &state->mDither);

    if (state->mShape == RING) {
        a->GetDimensionPixelSize(R::styleable::GradientDrawable_innerRadius, state->mInnerRadius, &state->mInnerRadius);

        if (state->mInnerRadius == -1) {
            a->GetFloat(R::styleable::GradientDrawable_innerRadiusRatio, state->mInnerRadiusRatio, &state->mInnerRadiusRatio);
        }

        a->GetDimensionPixelSize(R::styleable::GradientDrawable_thickness, state->mThickness, &state->mThickness);

        if (state->mThickness == -1) {
            a->GetFloat(R::styleable::GradientDrawable_thicknessRatio, state->mThicknessRatio, &state->mThicknessRatio);
        }

        a->GetBoolean(R::styleable::GradientDrawable_useLevel, state->mUseLevelForShape, &state->mUseLevelForShape);
    }
    return NOERROR;
}

ECode GradientDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    AutoPtr<GradientState> st = mGradientState;
    *can = st != NULL && (st->mThemeAttrs != NULL || st->mAttrSize != NULL
            || st->mAttrGradient != NULL || st->mAttrSolid != NULL
            || st->mAttrStroke != NULL || st->mAttrCorners != NULL
            || st->mAttrPadding != NULL);

    return NOERROR;
}

ECode GradientDrawable::ApplyThemeChildElements(
    /* [in] */ IResourcesTheme* t)
{
    AutoPtr<GradientState> st = mGradientState;

    if (st->mAttrSize != NULL) {
        AutoPtr<ITypedArray> a;

        Int32 size = ArraySize(R::styleable::GradientDrawableSize);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::GradientDrawableSize, size);
        ((CResources::Theme*)t)->ResolveAttribute(st->mAttrSize, layout, (ITypedArray**)&a);
        ECode ec = UpdateGradientDrawableSize(a);
        a->Recycle();
        if (FAILED(ec)) {
            return ec;
        }
    }

    if (st->mAttrGradient != NULL) {
        Int32 size = ArraySize(R::styleable::GradientDrawableGradient);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::GradientDrawableGradient, size);
        AutoPtr<ITypedArray> a;
        ((CResources::Theme*)t)->ResolveAttribute(st->mAttrGradient, layout, (ITypedArray**)&a);
        // try {
        AutoPtr<IResources> res;
        ((CResources::Theme*)t)->GetResources((IResources**)&res);
        ECode ec = UpdateGradientDrawableGradient(res, a);
        // } catch (XmlPullParserException e) {
        //     throw new RuntimeException(e);
        // } finally {
        a->Recycle();
        // }
        if (FAILED(ec)) {
            return E_RUNTIME_EXCEPTION;
        }
    }

    if (st->mAttrSolid != NULL) {
        Int32 size = ArraySize(R::styleable::GradientDrawableSolid);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::GradientDrawableSolid, size);

        AutoPtr<ITypedArray> a;
        ((CResources::Theme*)t)->ResolveAttribute(st->mAttrSolid, layout, (ITypedArray**)&a);
        ECode ec = UpdateGradientDrawableSolid(a);
        a->Recycle();
        if (FAILED(ec)) {
            return ec;
        }
    }

    if (st->mAttrStroke != NULL) {
        Int32 size = ArraySize(R::styleable::GradientDrawableStroke);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::GradientDrawableStroke, size);

        AutoPtr<ITypedArray> a;
        ((CResources::Theme*)t)->ResolveAttribute(st->mAttrStroke, layout, (ITypedArray**)&a);
        ECode ec = UpdateGradientDrawableStroke(a);
        a->Recycle();
        if (FAILED(ec)) {
            return ec;
        }
    }

    if (st->mAttrCorners != NULL) {
        Int32 size = ArraySize(R::styleable::DrawableCorners);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::DrawableCorners, size);

        AutoPtr<ITypedArray> a;
        ((CResources::Theme*)t)->ResolveAttribute(st->mAttrCorners, layout, (ITypedArray**)&a);
        ECode ec = UpdateDrawableCorners(a);
        a->Recycle();
        if (FAILED(ec)) {
            return ec;
        }
    }

    if (st->mAttrPadding != NULL) {
        Int32 size = ArraySize(R::styleable::GradientDrawablePadding);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::GradientDrawablePadding, size);

        AutoPtr<ITypedArray> a;
        ((CResources::Theme*)t)->ResolveAttribute(st->mAttrPadding, layout, (ITypedArray**)&a);
        ECode ec = UpdateGradientDrawablePadding(a);
        a->Recycle();
        if (FAILED(ec)) {
            return ec;
        }
    }
    return NOERROR;
}

ECode GradientDrawable::InflateChildElements(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/
{
    AutoPtr<ITypedArray> a;
    Int32 type;

    Int32 innerDepth = 0;
    parser->GetDepth(&innerDepth);
    innerDepth += 1;
    Int32 depth = 0;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
           && ((parser->GetDepth(&depth), depth) >= innerDepth
                   || type != IXmlPullParser::END_TAG)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        if (depth > innerDepth) {
            continue;
        }

        String name;
        parser->GetName(&name);

        if (name.Equals("size")) {
            Int32 size = ArraySize(R::styleable::GradientDrawableSize);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::GradientDrawableSize, size);
            ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
            if (FAILED(ec)) {
                a->Recycle();
                return ec;
            }
            ec = UpdateGradientDrawableSize(a);
            a->Recycle();
            if (FAILED(ec)) {
                return ec;
            }
        } else if (name.Equals("gradient")) {
            Int32 size = ArraySize(R::styleable::GradientDrawableGradient);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::GradientDrawableGradient, size);
            ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
            if (FAILED(ec)) {
                a->Recycle();
                return ec;
            }
            ec = UpdateGradientDrawableGradient(r, a);
            a->Recycle();
            if (FAILED(ec)) {
                return ec;
            }
        } else if (name.Equals("solid")) {
            Int32 size = ArraySize(R::styleable::GradientDrawableSolid);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::GradientDrawableSolid, size);
            ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
            if (FAILED(ec)) {
                a->Recycle();
                return ec;
            }
            ec = UpdateGradientDrawableSolid(a);
            a->Recycle();
            if (FAILED(ec)) {
                return ec;
            }
        } else if (name.Equals("stroke")) {
            Int32 size = ArraySize(R::styleable::GradientDrawableStroke);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::GradientDrawableStroke, size);
            ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
            if (FAILED(ec)) {
                a->Recycle();
                return ec;
            }
            ec = UpdateGradientDrawableStroke(a);
            a->Recycle();
            if (FAILED(ec)) {
                return ec;
            }
        } else if (name.Equals("corners")) {
            Int32 size = ArraySize(R::styleable::DrawableCorners);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::DrawableCorners, size);
            ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
            if (FAILED(ec)) {
                a->Recycle();
                return ec;
            }
            ec = UpdateDrawableCorners(a);
            a->Recycle();
            if (FAILED(ec)) {
                return ec;
            }
        } else if (name.Equals("padding")) {
            Int32 size = ArraySize(R::styleable::GradientDrawablePadding);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::GradientDrawablePadding, size);
            ECode ec = ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
            if (FAILED(ec)) {
                a->Recycle();
                return ec;
            }
            ec = UpdateGradientDrawablePadding(a);
            a->Recycle();
            if (FAILED(ec)) {
                return ec;
            }
        } else {
            Logger::W(String("drawable"), String("Bad element under <shape>: ") + name);
        }
    }
    return NOERROR;
}

ECode GradientDrawable::UpdateGradientDrawablePadding(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<GradientState> st = mGradientState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    st->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&st->mAttrPadding);

    if (st->mPadding == NULL) {
        CRect::New((IRect**)&st->mPadding);
    }

    AutoPtr<IRect> pad = st->mPadding;
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    pad->Get(&left, &top, &right, &bottom);
    a->GetDimensionPixelOffset(R::styleable::GradientDrawablePadding_left, left, &left);
    a->GetDimensionPixelOffset(R::styleable::GradientDrawablePadding_top, top, &top);
    a->GetDimensionPixelOffset(R::styleable::GradientDrawablePadding_right, right, &right);
    a->GetDimensionPixelOffset(R::styleable::GradientDrawablePadding_bottom, bottom, &bottom);
    pad->Set(left, top , right, bottom);
    mPadding = pad;
    return NOERROR;
}

ECode GradientDrawable::UpdateDrawableCorners(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<GradientState> st = mGradientState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    st->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&st->mAttrCorners);

    Int32 radius = 0;
    a->GetDimensionPixelSize(R::styleable::DrawableCorners_radius, (Int32) st->mRadius, &radius);
    SetCornerRadius(radius);

    // TODO: Update these to be themeable.
    Int32 topLeftRadius = 0;
    a->GetDimensionPixelSize(R::styleable::DrawableCorners_topLeftRadius, radius, &topLeftRadius);
    Int32 topRightRadius = 0;
    a->GetDimensionPixelSize(R::styleable::DrawableCorners_topRightRadius, radius, &topRightRadius);
    Int32 bottomLeftRadius = 0;
    a->GetDimensionPixelSize(R::styleable::DrawableCorners_bottomLeftRadius, radius, &bottomLeftRadius);
    Int32 bottomRightRadius = 0;
    a->GetDimensionPixelSize(R::styleable::DrawableCorners_bottomRightRadius, radius, &bottomRightRadius);
    if (topLeftRadius != radius || topRightRadius != radius ||
            bottomLeftRadius != radius || bottomRightRadius != radius) {
        // The corner radii are specified in clockwise order (see Path.addRoundRect())
        AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(8);
        (*values)[0] = topLeftRadius;
        (*values)[1] = topLeftRadius;
        (*values)[2] = topRightRadius;
        (*values)[3] = topRightRadius;
        (*values)[4] = bottomRightRadius;
        (*values)[5] = bottomRightRadius;
        (*values)[6] = bottomLeftRadius;
        (*values)[7] = bottomLeftRadius;
        SetCornerRadii(values);
    }
    return NOERROR;
}

ECode GradientDrawable::UpdateGradientDrawableStroke(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<GradientState> st = mGradientState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    st->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    FAIL_RETURN(((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&st->mAttrStroke));

    // We have an explicit stroke defined, so the default stroke width
    // must be at least 0 or the current stroke width.
    Int32 defaultStrokeWidth = Elastos::Core::Math::Max(0, st->mStrokeWidth);
    Int32 width = 0;
    a->GetDimensionPixelSize(R::styleable::GradientDrawableStroke_width, defaultStrokeWidth, &width);
    Float dashWidth = 0;
    a->GetDimension(R::styleable::GradientDrawableStroke_dashWidth, st->mStrokeDashWidth, &dashWidth);

    AutoPtr<IColorStateList> colorStateList;
    a->GetColorStateList(R::styleable::GradientDrawableStroke_color, (IColorStateList**)&colorStateList);
    if (colorStateList == NULL) {
        colorStateList = st->mStrokeColorStateList;
    }

    if (dashWidth != 0.0f) {
        Float dashGap = 0;
        a->GetDimension(R::styleable::GradientDrawableStroke_dashGap, st->mStrokeDashGap, &dashGap);
        FAIL_RETURN(SetStroke(width, colorStateList, dashWidth, dashGap));
    } else {
        FAIL_RETURN(SetStroke(width, colorStateList));
    }
    return NOERROR;
}

ECode GradientDrawable::UpdateGradientDrawableSolid(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<GradientState> st = mGradientState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    st->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    FAIL_RETURN(((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&st->mAttrSolid));

    AutoPtr<IColorStateList> colorStateList;
    a->GetColorStateList(R::styleable::GradientDrawableSolid_color, (IColorStateList**)&colorStateList);
    if (colorStateList != NULL) {
        FAIL_RETURN(SetColor(colorStateList));
    }
    return NOERROR;
}

ECode GradientDrawable::UpdateGradientDrawableGradient(
    /* [in] */ IResources* r,
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    AutoPtr<GradientState> st = mGradientState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    st->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    FAIL_RETURN(((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&st->mAttrGradient));

    st->mCenterX = GetFloatOrFraction(a, R::styleable::GradientDrawableGradient_centerX, st->mCenterX);
    st->mCenterY = GetFloatOrFraction(a, R::styleable::GradientDrawableGradient_centerY, st->mCenterY);
    a->GetBoolean(R::styleable::GradientDrawableGradient_useLevel, st->mUseLevel, &st->mUseLevel);
    a->GetInt32(R::styleable::GradientDrawableGradient_type, st->mGradient, &st->mGradient);

    // TODO: Update these to be themeable.
    Int32 startColor = 0;
    a->GetColor(R::styleable::GradientDrawableGradient_startColor, 0, &startColor);
    Boolean hasCenterColor = FALSE;
    a->HasValue(R::styleable::GradientDrawableGradient_centerColor, &hasCenterColor);
    Int32 centerColor = 0;
    a->GetColor(R::styleable::GradientDrawableGradient_centerColor, 0, &centerColor);
    Int32 endColor = 0;
    a->GetColor(R::styleable::GradientDrawableGradient_endColor, 0, &endColor);

    if (hasCenterColor) {
        st->mColors = ArrayOf<Int32>::Alloc(3);
        (*st->mColors)[0] = startColor;
        (*st->mColors)[1] = centerColor;
        (*st->mColors)[2] = endColor;

        st->mPositions = ArrayOf<Float>::Alloc(3);
        (*st->mPositions)[0] = 0.0f;
        // Since 0.5f is default value, try to take the one that isn't 0.5f
        (*st->mPositions)[1] = st->mCenterX != 0.5f ? st->mCenterX : st->mCenterY;
        (*st->mPositions)[2] = 1.f;
    } else {
        st->mColors = ArrayOf<Int32>::Alloc(2);
        (*st->mColors)[0] = startColor;
        (*st->mColors)[1] = endColor;
    }

    if (st->mGradient == LINEAR_GRADIENT) {
        Float value = 0;
        a->GetFloat(R::styleable::GradientDrawableGradient_angle, st->mAngle, &value);
        Int32 angle = (Int32) value;
        angle %= 360;

        if (angle % 45 != 0) {
            // throw new XmlPullParserException(a.getPositionDescription()
            //         + "<gradient> tag requires 'angle' attribute to "
            //         + "be a multiple of 45");
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        st->mAngle = angle;

        switch (angle) {
            case 0:
                st->mOrientation = GradientDrawableOrientation_LEFT_RIGHT;
                break;
            case 45:
                st->mOrientation = GradientDrawableOrientation_BL_TR;
                break;
            case 90:
                st->mOrientation = GradientDrawableOrientation_BOTTOM_TOP;
                break;
            case 135:
                st->mOrientation = GradientDrawableOrientation_BR_TL;
                break;
            case 180:
                st->mOrientation = GradientDrawableOrientation_RIGHT_LEFT;
                break;
            case 225:
                st->mOrientation = GradientDrawableOrientation_TR_BL;
                break;
            case 270:
                st->mOrientation = GradientDrawableOrientation_TOP_BOTTOM;
                break;
            case 315:
                st->mOrientation = GradientDrawableOrientation_TL_BR;
                break;
        }
    } else {
        AutoPtr<ITypedValue> tv;
        FAIL_RETURN(a->PeekValue(R::styleable::GradientDrawableGradient_gradientRadius, (ITypedValue**)&tv));
        if (tv != NULL) {
            Float radius = 0;
            Int32 radiusType = 0;
            Int32 value = 0;
            if ((tv->GetType(&value), value) == ITypedValue::TYPE_FRACTION) {
                tv->GetFraction(1.0f, 1.0f, &radius);


                Int32 unit = ((tv->GetData(&value), value) >> ITypedValue::COMPLEX_UNIT_SHIFT)
                        & ITypedValue::COMPLEX_UNIT_MASK;
                if (unit == ITypedValue::COMPLEX_UNIT_FRACTION_PARENT) {
                    radiusType = RADIUS_TYPE_FRACTION_PARENT;
                } else {
                    radiusType = RADIUS_TYPE_FRACTION;
                }
            } else {
                AutoPtr<IDisplayMetrics> dm;
                tv->GetDimension((r->GetDisplayMetrics((IDisplayMetrics**)&dm), dm), &radius);
                radiusType = RADIUS_TYPE_PIXELS;
            }

            st->mGradientRadius = radius;
            st->mGradientRadiusType = radiusType;
        } else if (st->mGradient == RADIAL_GRADIENT) {
            // throw new XmlPullParserException(
            //         a.getPositionDescription()
            //         + "<gradient> tag requires 'gradientRadius' "
            //         + "attribute with radial type");
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode GradientDrawable::UpdateGradientDrawableSize(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<GradientState> st = mGradientState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    st->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    FAIL_RETURN(((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&st->mAttrSize));

    a->GetDimensionPixelSize(R::styleable::GradientDrawableSize_width, st->mWidth, &st->mWidth);
    a->GetDimensionPixelSize(R::styleable::GradientDrawableSize_height, st->mHeight, &st->mHeight);
    return NOERROR;
}

Float GradientDrawable::GetFloatOrFraction(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [in] */ Float defaultValue)
{
    AutoPtr<ITypedValue> tv;
    a->PeekValue(index, (ITypedValue**)&tv);
    Float v = defaultValue;
    if (tv != NULL) {
        Int32 type;
        tv->GetType(&type);
        Boolean vIsFraction = type == ITypedValue::TYPE_FRACTION;
        vIsFraction ? tv->GetFraction(1.0f, 1.0f, &v) : tv->GetFloat(&v);
    }
    return v;
}

ECode GradientDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mGradientState->mWidth;
    return NOERROR;
}

ECode GradientDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mGradientState->mHeight;
    return NOERROR;
}

ECode GradientDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    GetChangingConfigurations(&mGradientState->mChangingConfigurations);
    *state = (IDrawableConstantState*)mGradientState.Get();
    REFCOUNT_ADD(*state);
    return NOERROR;
}

Boolean GradientDrawable::IsOpaqueForState()
{
    Int32 color = 0;
    if (mGradientState->mStrokeWidth >= 0 && mStrokePaint != NULL
            && !IsOpaque((mStrokePaint->GetColor(&color), color))) {
        return FALSE;
    }

    if (!IsOpaque((mFillPaint->GetColor(&color), color))) {
        return FALSE;
    }

    return TRUE;
}

ECode GradientDrawable::GetOutline(
    /* [out] */ IOutline* outline)
{
    AutoPtr<GradientState> st = mGradientState;
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    // only report non-zero alpha if shape being drawn is opaque
    outline->SetAlpha(st->mOpaqueOverShape && IsOpaqueForState() ? (mAlpha / 255.0f) : 0.0f);

    Float rad = 0, centerY = 0, halfStrokeWidth = 0;;
    Float value = 0;
    Int32 tmp = 0;
    Int32 left = 0, right = 0, top = 0, bottom = 0;
    switch (st->mShape) {
        case RECTANGLE:
            if (st->mRadiusArray != NULL) {
                BuildPathIfDirty();
                outline->SetConvexPath(mPath);
                return NOERROR;
            }

            if (st->mRadius > 0.0f) {
                // clamp the radius based on width & height, matching behavior in draw()
                Int32 width = 0, height = 0;
                bounds->GetWidth(&width);
                bounds->GetHeight(&height);
                rad = Elastos::Core::Math::Min(st->mRadius,
                        Elastos::Core::Math::Min(width, height) * 0.5f);
            }
            outline->SetRoundRect(bounds, rad);
            return NOERROR;
        case OVAL:
            outline->SetOval(bounds);
            return NOERROR;
        case LINE:
            // Hairlines (0-width stroke) must have a non-empty outline for
            // shadows to draw correctly, so we'll use a very small width.
            halfStrokeWidth = mStrokePaint == NULL ?
                    0.0001f : (mStrokePaint->GetStrokeWidth(&value), value) * 0.5f;
            bounds->GetCenterY(&tmp);
            centerY = tmp;
            top = (Int32) Elastos::Core::Math::Floor(centerY - halfStrokeWidth);
            bottom = (Int32) Elastos::Core::Math::Ceil(centerY + halfStrokeWidth);

            bounds->GetLeft(&left);
            bounds->GetRight(&right);
            outline->SetRect(left, top, right, bottom);
            return NOERROR;
        default:
            ;// TODO: support more complex shapes
    }
    return NOERROR;
}

ECode GradientDrawable::Mutate()
{
    if (!mMutated) {
        mGradientState = new GradientState(mGradientState);
        InitializeWithState(mGradientState);
        mMutated = TRUE;
    }
    return NOERROR;
}

GradientDrawable::GradientDrawable(
    /* [in] */ GradientState* state,
    /* [in] */ IResourcesTheme* theme)
    : mAlpha(0xFF)
    , mGradientIsDirty(FALSE)
    , mMutated(FALSE)
    , mPathIsDirty(TRUE)
    , mGradientRadius(0)
{
    constructor(state, theme);
}

void GradientDrawable::InitializeWithState(
    /* [in] */ GradientState* state)
{
    if (state->mColorStateList != NULL) {
        AutoPtr<ArrayOf<Int32> > currentState;
        GetState((ArrayOf<Int32>**)&currentState);
        Int32 stateColor = 0;
        state->mColorStateList->GetColorForState(currentState, 0, &stateColor);
        mFillPaint->SetColor(stateColor);
    }
    else if (state->mColors == NULL) {
        // If we don't have a solid color and we don't have a gradient,
        // the app is stroking the shape, set the color to the default
        // value of state.mSolidColor
        mFillPaint->SetColor(0);
    }
    else {
        // Otherwise, make sure the fill alpha is maxed out.
        mFillPaint->SetColor(IColor::BLACK);
    }

    mPadding = state->mPadding;
    if (state->mStrokeWidth >= 0) {
        mStrokePaint = NULL;
        CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mStrokePaint);
        mStrokePaint->SetStyle(PaintStyle_STROKE);
        mStrokePaint->SetStrokeWidth(state->mStrokeWidth);
        if (state->mStrokeColorStateList != NULL) {
            AutoPtr<ArrayOf<Int32> > currentState;
            GetState((ArrayOf<Int32>**)&currentState);
            Int32 strokeStateColor = 0;
            state->mStrokeColorStateList->GetColorForState(currentState, 0, &strokeStateColor);
            mStrokePaint->SetColor(strokeStateColor);
        }

        if (state->mStrokeDashWidth != 0.0f) {
            AutoPtr<IDashPathEffect> e;
            AutoPtr<ArrayOf<Float> > intervals = ArrayOf<Float>::Alloc(2);
            (*intervals)[0] = state->mStrokeDashWidth;
            (*intervals)[1] = state->mStrokeDashGap;
            CDashPathEffect::New(*intervals, 0, (IDashPathEffect**)&e);
            mStrokePaint->SetPathEffect((IPathEffect*)e.Get());
        }
    }
}

ECode GradientDrawable::constructor()
{
    AutoPtr<GradientState> state = new GradientState(GradientDrawableOrientation_TOP_BOTTOM, NULL);
    return constructor(state, NULL);
}

ECode GradientDrawable::constructor(
    /* [in] */ GradientDrawableOrientation orientation,
    /* [in] */ ArrayOf<Int32>* colors)
{
    AutoPtr<GradientState> state = new GradientState(orientation, colors);
    return constructor(state, NULL);
}

ECode GradientDrawable::constructor(
    /* [in] */ GradientState* state,
    /* [in] */ IResourcesTheme* theme)
{
    Boolean can = FALSE;
    if (theme != NULL && (state->CanApplyTheme(&can), can)) {
        // If we need to apply a theme, implicitly mutate.
        mGradientState = new GradientState(state);
        ApplyTheme(theme);
    } else {
        mGradientState = state;
    }

    InitializeWithState(state);

    mGradientIsDirty = TRUE;
    mMutated = FALSE;
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
