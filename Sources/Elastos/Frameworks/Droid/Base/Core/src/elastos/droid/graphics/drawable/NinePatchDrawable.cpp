
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/NinePatchDrawable.h"
#include "elastos/droid/graphics/drawable/CNinePatchDrawable.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CNinePatch.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/graphics/NinePatch.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ILayoutDirection;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

////////////////////////////////////////////////////////////////////////////////
// NinePatchDrawable::NinePatchState
NinePatchDrawable::NinePatchState::NinePatchState()
    : mTintMode(DEFAULT_TINT_MODE)
    , mOpticalInsets(Insets::NONE)
    , mBaseAlpha(1.0)
    , mDither(DEFAULT_DITHER)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mAutoMirrored(FALSE)
    , mChangingConfigurations(0)
{}

NinePatchDrawable::NinePatchState::NinePatchState(
    /* [in] *//*@NonNull*/ INinePatch* ninePatch,
    /* [in] *//*@Nullable*/ IRect* padding)
    : mTintMode(DEFAULT_TINT_MODE)
    , mOpticalInsets(Insets::NONE)
    , mBaseAlpha(1.0)
    , mDither(DEFAULT_DITHER)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mAutoMirrored(FALSE)
    , mChangingConfigurations(0)
{
    constructor(ninePatch, padding, NULL, DEFAULT_DITHER, FALSE);
}

NinePatchDrawable::NinePatchState::NinePatchState(
    /* [in] *//*@NonNull*/ INinePatch* ninePatch,
    /* [in] *//*@Nullable*/ IRect* padding,
    /* [in] *//*@Nullable*/ IRect* opticalInsets)
    : mTintMode(DEFAULT_TINT_MODE)
    , mOpticalInsets(Insets::NONE)
    , mBaseAlpha(1.0)
    , mDither(DEFAULT_DITHER)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mAutoMirrored(FALSE)
    , mChangingConfigurations(0)
{
    constructor(ninePatch, padding, opticalInsets, DEFAULT_DITHER, FALSE);
}

NinePatchDrawable::NinePatchState::NinePatchState(
    /* [in] *//*@NonNull*/ INinePatch* ninePatch,
    /* [in] *//*@Nullable*/ IRect* padding,
    /* [in] *//*@Nullable*/ IRect* opticalInsets,
    /* [in] */ Boolean dither,
    /* [in] */ Boolean autoMirror)
    : mTintMode(DEFAULT_TINT_MODE)
    , mBaseAlpha(1.0)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mChangingConfigurations(0)
{
    constructor(ninePatch, padding, opticalInsets, dither, autoMirror);
}

ECode NinePatchDrawable::NinePatchState::constructor(
    /* [in] *//*@NonNull*/ INinePatch* ninePatch,
    /* [in] *//*@Nullable*/ IRect* padding,
    /* [in] *//*@Nullable*/ IRect* opticalInsets,
    /* [in] */ Boolean dither,
    /* [in] */ Boolean autoMirror)
{
    mNinePatch = ninePatch;
    mPadding = padding;
    mOpticalInsets = Insets::Of(opticalInsets);
    mDither = dither;
    mAutoMirrored = autoMirror;
    return NOERROR;
}

NinePatchDrawable::NinePatchState::NinePatchState(
    /* [in] *//*@NonNull*/ NinePatchState* state)
{
    // We don't deep-copy any fields because they are all immutable.
    mNinePatch = state->mNinePatch;
    mTint = state->mTint;
    mTintMode = state->mTintMode;
    mThemeAttrs = state->mThemeAttrs;
    mPadding = state->mPadding;
    mOpticalInsets = state->mOpticalInsets;
    mBaseAlpha = state->mBaseAlpha;
    mDither = state->mDither;
    mChangingConfigurations = state->mChangingConfigurations;
    mTargetDensity = state->mTargetDensity;
    mAutoMirrored = state->mAutoMirrored;
}

ECode NinePatchDrawable::NinePatchState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mThemeAttrs != NULL;
    return NOERROR;
}

ECode NinePatchDrawable::NinePatchState::GetBitmap(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return mNinePatch->GetBitmap(bitmap);
}

ECode NinePatchDrawable::NinePatchState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CNinePatchDrawable::New(this, NULL, NULL, drawable);
}

ECode NinePatchDrawable::NinePatchState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CNinePatchDrawable::New(this, res, NULL, drawable);
}

ECode NinePatchDrawable::NinePatchState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return CNinePatchDrawable::New(this, res, theme, drawable);
}

ECode NinePatchDrawable::NinePatchState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}

////////////////////////////////////////////////////////////////////////////////
// NinePatchDrawable
CAR_INTERFACE_IMPL(NinePatchDrawable, Drawable, INinePatchDrawable)

const Boolean NinePatchDrawable::DEFAULT_DITHER = FALSE;
NinePatchDrawable::NinePatchDrawable()
    : mMutated(FALSE)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mBitmapWidth(-1)
    , mBitmapHeight(-1)
{
    mOpticalInsets = Insets::NONE;
    mNinePatchState = new NinePatchState();
}

NinePatchDrawable::NinePatchDrawable(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ const String& srcName)
    : mMutated(FALSE)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mBitmapWidth(-1)
    , mBitmapHeight(-1)
{
    mOpticalInsets = Insets::NONE;
    constructor(bitmap, chunk, padding, srcName);
}

NinePatchDrawable::NinePatchDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ const String& srcName)
    : mMutated(FALSE)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mBitmapWidth(-1)
    , mBitmapHeight(-1)
{
    mOpticalInsets = Insets::NONE;
    constructor(res, bitmap, chunk, padding, srcName);
}

NinePatchDrawable::NinePatchDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ IRect* layoutInsets,
    /* [in] */ const String& srcName)
    : mMutated(FALSE)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mBitmapWidth(-1)
    , mBitmapHeight(-1)
{
    mOpticalInsets = Insets::NONE;
    constructor(res, bitmap, chunk, padding, layoutInsets, srcName);
}

NinePatchDrawable::NinePatchDrawable(
    /* [in] */ INinePatch* patch)
    : mMutated(FALSE)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mBitmapWidth(-1)
    , mBitmapHeight(-1)
{
    mOpticalInsets = Insets::NONE;
    constructor(patch);
}

NinePatchDrawable::NinePatchDrawable(
    /* [in] */ IResources* res,
    /* [in] */ INinePatch* patch)
    : mMutated(FALSE)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mBitmapWidth(-1)
    , mBitmapHeight(-1)
{
    mOpticalInsets = Insets::NONE;
    constructor(res, patch);
}

NinePatchDrawable::NinePatchDrawable(
    /* [in] */ NinePatchState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
    : mMutated(FALSE)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mBitmapWidth(-1)
    , mBitmapHeight(-1)
{
    mOpticalInsets = Insets::NONE;
    constructor(state, res, theme);
}

ECode NinePatchDrawable::constructor(
    /* [in] */ NinePatchState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    Boolean can = FALSE;
    if (theme != NULL && (state->CanApplyTheme(&can), can)) {
        // If we need to apply a theme, implicitly mutate.
        mNinePatchState = new NinePatchState(state);
        ApplyTheme(theme);
    } else {
        mNinePatchState = state;
    }

    return InitializeWithState(state, res);
}

ECode NinePatchDrawable::InitializeWithState(
    /* [in] */ NinePatchState* state,
    /* [in] */ IResources* res)
{
    if (res != NULL) {
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        dm->GetDensityDpi(&mTargetDensity);
    } else {
        mTargetDensity = state->mTargetDensity;
    }

    // If we can, avoid calling any methods that initialize Paint.
    if (state->mDither != DEFAULT_DITHER) {
        SetDither(state->mDither);
    }

    // Make a local copy of the padding.
    if (state->mPadding != NULL) {
        CRect::New(state->mPadding, (IRect**)&mPadding);
    }

    mTintFilter = UpdateTintFilter(mTintFilter, state->mTint, state->mTintMode);
    SetNinePatch(state->mNinePatch);
    return NOERROR;
}

ECode NinePatchDrawable::SetTargetDensity(
    /* [in] */ ICanvas* canvas)
{
    Int32 density;
    canvas->GetDensity(&density);
    return SetTargetDensity(density);
}

ECode NinePatchDrawable::SetTargetDensity(
    /* [in] */ IDisplayMetrics* metrics)
{
    Int32 dpi;
    metrics->GetDensityDpi(&dpi);
    return SetTargetDensity(dpi);
}

ECode NinePatchDrawable::SetTargetDensity(
    /* [in] */ Int32 density)
{
    if (density != mTargetDensity) {
        mTargetDensity = density == 0 ? IDisplayMetrics::DENSITY_DEFAULT : density;
        if (mNinePatch != NULL) {
            ComputeBitmapSize();
        }
        InvalidateSelf();
    }
    return NOERROR;
}

AutoPtr<Insets> NinePatchDrawable::ScaleFromDensity(
    /* [in] */ Insets* insets,
    /* [in] */ Int32 sdensity,
    /* [in] */ Int32 tdensity)
{
    Int32 left = CBitmap::ScaleFromDensity(insets->mLeft, sdensity, tdensity);
    Int32 top = CBitmap::ScaleFromDensity(insets->mTop, sdensity, tdensity);
    Int32 right = CBitmap::ScaleFromDensity(insets->mRight, sdensity, tdensity);
    Int32 bottom = CBitmap::ScaleFromDensity(insets->mBottom, sdensity, tdensity);
    return Insets::Of(left, top, right, bottom);
}

void NinePatchDrawable::ComputeBitmapSize()
{
    Int32 sdensity = 0;
    mNinePatch->GetDensity(&sdensity);
    const Int32 tdensity = mTargetDensity;
    if (sdensity == tdensity) {
        mNinePatch->GetWidth(&mBitmapWidth);
        mNinePatch->GetHeight(&mBitmapHeight);
        mOpticalInsets = mNinePatchState->mOpticalInsets;
    }
    else {
        Int32 width = 0, height = 0;
        mNinePatch->GetWidth(&width);
        mNinePatch->GetHeight(&height);
        mBitmapWidth = CBitmap::ScaleFromDensity(width, sdensity, tdensity);
        mBitmapHeight = CBitmap::ScaleFromDensity(height, sdensity, tdensity);
        if (mNinePatchState->mPadding != NULL && mPadding != NULL) {
            AutoPtr<IRect> dest = mPadding;
            AutoPtr<IRect> src = mNinePatchState->mPadding;
            if (dest == src) {
                mPadding = NULL;
                CRect::New(src, (IRect**)&mPadding);
                dest = mPadding;
            }
            CRect* csrc = (CRect*)src.Get();
            CRect* cdest = (CRect*)dest.Get();
            cdest->mLeft = CBitmap::ScaleFromDensity(csrc->mLeft, sdensity, tdensity);
            cdest->mTop = CBitmap::ScaleFromDensity(csrc->mTop, sdensity, tdensity);
            cdest->mRight = CBitmap::ScaleFromDensity(csrc->mRight, sdensity, tdensity);
            cdest->mBottom = CBitmap::ScaleFromDensity(csrc->mBottom, sdensity, tdensity);
        }
        mOpticalInsets = ScaleFromDensity((Insets*)mNinePatchState->mOpticalInsets.Get(), sdensity, tdensity);
    }
}

void NinePatchDrawable::SetNinePatch(
    /* [in] */ INinePatch* ninePatch)
{
    if (mNinePatch.Get() != ninePatch) {
        mNinePatch = ninePatch;
        if (ninePatch != NULL) {
            ComputeBitmapSize();
        } else {
            mBitmapWidth = mBitmapHeight = -1;
            mOpticalInsets = Insets::NONE;
        }
        InvalidateSelf();
    }
}

ECode NinePatchDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);

    Boolean clearColorFilter = FALSE;
    AutoPtr<IPaint> paint;
    GetPaint((IPaint**)&paint);
    AutoPtr<IColorFilter> cf;
    paint->GetColorFilter((IColorFilter**)&cf);
    if (mTintFilter != NULL && cf == NULL) {
        mPaint->SetColorFilter(IColorFilter::Probe(mTintFilter));
        clearColorFilter = TRUE;
    } else {
        clearColorFilter = FALSE;
    }

    Boolean needsMirroring = NeedsMirroring();
    if (needsMirroring) {
        // Mirror the 9patch
        canvas->Translate(((CRect*)bounds.Get())->mRight - ((CRect*)bounds.Get())->mLeft, 0);
        canvas->Scale(-1.0f, 1.0f);
    }

    Int32 restoreAlpha = 0;
    if (mNinePatchState->mBaseAlpha != 1.0f) {
        mPaint->GetAlpha(&restoreAlpha);
        mPaint->SetAlpha((Int32) (restoreAlpha * mNinePatchState->mBaseAlpha + 0.5f));
    } else {
        restoreAlpha = -1;
    }

    mNinePatch->Draw(canvas, bounds, mPaint);

    if (clearColorFilter) {
        mPaint->SetColorFilter(NULL);
    }

    if (restoreAlpha >= 0) {
        mPaint->SetAlpha(restoreAlpha);
    }
    return NOERROR;
}

ECode NinePatchDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Drawable::GetChangingConfigurations(configuration);
    *configuration = (*configuration)
        | mNinePatchState->mChangingConfigurations;

    return NOERROR;
}

ECode NinePatchDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    VALIDATE_NOT_NULL(isPadding);
    CRect* scaledPadding = (CRect*)mPadding.Get();
    if (scaledPadding != NULL) {
        if (NeedsMirroring()) {
            padding->Set(scaledPadding->mRight, scaledPadding->mTop,
                    scaledPadding->mLeft, scaledPadding->mBottom);
        } else {
            padding->Set(mPadding);
        }
        *isPadding = (((CRect*)padding)->mLeft | ((CRect*)padding)->mTop | ((CRect*)padding)->mRight | ((CRect*)padding)->mBottom) != 0;
        return NOERROR;
    }
    *isPadding = FALSE;
    return NOERROR;
}

ECode NinePatchDrawable::GetOutline(
    /* [in] */ /*@NonNull*/ IOutline* outline)
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Boolean isEmpty = FALSE;
    if (bounds->IsEmpty(&isEmpty), isEmpty) return NOERROR;

    if (mNinePatchState != NULL) {
        AutoPtr<IBitmap> bitmap;
        mNinePatchState->GetBitmap((IBitmap**)&bitmap);
        AutoPtr<INinePatchInsetStruct> insets;
        bitmap->GetNinePatchInsets((INinePatchInsetStruct**)&insets);
        if (insets != NULL) {
            NinePatch::InsetStruct* _insets = (NinePatch::InsetStruct*)insets.Get();
            CRect* outlineInsets = (CRect*)_insets->mOutlineRect.Get();
            CRect* _bounds = (CRect*)bounds.Get();
            outline->SetRoundRect(_bounds->mLeft + outlineInsets->mLeft,
                    _bounds->mTop + outlineInsets->mTop,
                    _bounds->mRight - outlineInsets->mRight,
                    _bounds->mBottom - outlineInsets->mBottom,
                    _insets->mOutlineRadius);
            Int32 alpha = 0;
            GetAlpha(&alpha);
            return outline->SetAlpha(_insets->mOutlineAlpha * (alpha / 255.0f));
        }
    }
    return Drawable::GetOutline(outline);
}

ECode NinePatchDrawable::GetOpticalInsets(
    /* [out] */ IInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    if (NeedsMirroring()) {
        AutoPtr<IInsets> temp = Insets::Of(mOpticalInsets->mRight, mOpticalInsets->mTop,
                mOpticalInsets->mLeft, mOpticalInsets->mBottom);
        *insets = temp;
        REFCOUNT_ADD(*insets);
        return NOERROR;
    }
    *insets = (IInsets*)mOpticalInsets.Get();
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode NinePatchDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    if (mPaint == NULL && alpha == 0xFF) {
        // Fast common case -- leave at normal alpha.
        return NOERROR;
    }
    AutoPtr<IPaint> paint;
    GetPaint((IPaint**)&paint);
    paint->SetAlpha(alpha);
    return InvalidateSelf();
}

ECode NinePatchDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    if (mPaint == NULL) {
        // Fast common case -- normal alpha.
        *alpha = 0xFF;
        return NOERROR;
    }
    AutoPtr<IPaint> paint;
    GetPaint((IPaint**)&paint);
    return paint->GetAlpha(alpha);
}

ECode NinePatchDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    if (mPaint == NULL && cf == NULL) {
        // Fast common case -- leave at no color filter.
        return NOERROR;
    }
    AutoPtr<IPaint> paint;
    GetPaint((IPaint**)&paint);
    return paint->SetColorFilter(cf);
}

ECode NinePatchDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    mNinePatchState->mTint = tint;
    mTintFilter = UpdateTintFilter(mTintFilter, tint, mNinePatchState->mTintMode);
    return InvalidateSelf();
}

ECode NinePatchDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    mNinePatchState->mTintMode = tintMode;
    mTintFilter = UpdateTintFilter(mTintFilter, mNinePatchState->mTint, tintMode);
    return InvalidateSelf();
}

ECode NinePatchDrawable::SetDither(
    /* [in] */ Boolean dither)
{
    //noinspection PointlessBooleanExpression
    if (mPaint == NULL && dither == DEFAULT_DITHER) {
        // Fast common case -- leave at default dither.
        return NOERROR;
    }
    AutoPtr<IPaint> paint;
    GetPaint((IPaint**)&paint);
    paint->SetDither(dither);
    return InvalidateSelf();
}

ECode NinePatchDrawable::SetAutoMirrored(
    /* [in] */ Boolean mirrored)
{
    mNinePatchState->mAutoMirrored = mirrored;
    return NOERROR;
}

Boolean NinePatchDrawable::NeedsMirroring()
{
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    Boolean mirrored = FALSE;
    return (IsAutoMirrored(&mirrored), mirrored) && (GetLayoutDirection(&dir), dir) == ILayoutDirection::RTL;
}

ECode NinePatchDrawable::IsAutoMirrored(
    /* [out] */ Boolean* mirrored)
{
    VALIDATE_NOT_NULL(mirrored);
    *mirrored = mNinePatchState->mAutoMirrored;
    return NOERROR;
}

ECode NinePatchDrawable::SetFilterBitmap(
    /* [in] */ Boolean filter)
{
    AutoPtr<IPaint> paint;
    GetPaint((IPaint**)&paint);
    paint->SetFilterBitmap(filter);
    return InvalidateSelf();
}

ECode NinePatchDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    Drawable::Inflate(r, parser, attrs, theme);

    Int32 size = ArraySize(R::styleable::NinePatchDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::NinePatchDrawable, size);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));
    ECode ec = UpdateStateFromTypedArray(a);
    if (FAILED(ec)) {
    a->Recycle();
        return ec;
    }
    a->Recycle();
    return NOERROR;
}

ECode NinePatchDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    AutoPtr<IResources> r;
    a->GetResources((IResources**)&r);
    AutoPtr<NinePatchState> state = mNinePatchState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    state->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    FAIL_RETURN(((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mThemeAttrs));

    a->GetBoolean(R::styleable::NinePatchDrawable_dither, state->mDither, &state->mDither);

    Int32 srcResId = 0;
    a->GetResourceId(R::styleable::NinePatchDrawable_src, 0, &srcResId);
    if (srcResId != 0) {
        AutoPtr<IBitmapFactoryOptions> options;
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
        ((CBitmapFactoryOptions*)options.Get())->mInDither = !state->mDither;
        AutoPtr<IDisplayMetrics> dm;
        r->GetDisplayMetrics((IDisplayMetrics**)&dm);
        dm->GetNoncompatDensityDpi(&((CBitmapFactoryOptions*)options.Get())->mInScreenDensity);

        AutoPtr<IRect> padding;
        CRect::New((IRect**)&padding);
        AutoPtr<IRect> opticalInsets;
        CRect::New((IRect**)&opticalInsets);
        AutoPtr<IBitmap> bitmap;

        // try {
        AutoPtr<ITypedValue> value;
        CTypedValue::New((ITypedValue**)&value);
        AutoPtr<IInputStream> is;
        r->OpenRawResource(srcResId, value, (IInputStream**)&is);

        BitmapFactory::DecodeResourceStream(r, value, is, padding, options, (IBitmap**)&bitmap);

        is->Close();
        // } catch (IOException e) {
        //     // Ignore
        // }

        AutoPtr<ArrayOf<Byte> > chunk;
        if (bitmap == NULL) {
            // throw new XmlPullParserException(a.getPositionDescription() +
            //         ": <nine-patch> requires a valid src attribute");
            return E_XML_PULL_PARSER_EXCEPTION;
        } else if ((bitmap->GetNinePatchChunk((ArrayOf<Byte>**)&chunk), chunk.Get()) == NULL) {
            // throw new XmlPullParserException(a.getPositionDescription() +
            //         ": <nine-patch> requires a valid 9-patch source image");
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        bitmap->GetOpticalInsets(opticalInsets);

        CNinePatch::New(bitmap, chunk, (INinePatch**)&state->mNinePatch);
        state->mPadding = padding;
        state->mOpticalInsets = Insets::Of(opticalInsets);
    }

    a->GetBoolean(R::styleable::NinePatchDrawable_autoMirrored, state->mAutoMirrored, &state->mAutoMirrored);
    a->GetFloat(R::styleable::NinePatchDrawable_alpha, state->mBaseAlpha, &state->mBaseAlpha);

    Int32 tintMode = 0;
    a->GetInt32(R::styleable::NinePatchDrawable_tintMode, -1, &tintMode);
    if (tintMode != -1) {
        Drawable::ParseTintMode(tintMode, PorterDuffMode_SRC_IN, &state->mTintMode);
    }

    AutoPtr<IColorStateList> tint;
    a->GetColorStateList(R::styleable::NinePatchDrawable_tint, (IColorStateList**)&tint);
    if (tint != NULL) {
        state->mTint = tint;
    }

    // Update local properties.
    InitializeWithState(state, r);

    // Push density applied by setNinePatchState into state.
    state->mTargetDensity = mTargetDensity;
    return NOERROR;
}

ECode NinePatchDrawable::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    Drawable::ApplyTheme(t);

    AutoPtr<NinePatchState> state = mNinePatchState;
    if (state == NULL || state->mThemeAttrs == NULL) {
        return NOERROR;
    }

    AutoPtr<ITypedArray> a;
    Int32 size = ArraySize(R::styleable::NinePatchDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::NinePatchDrawable, size);
    ((CResources::Theme*)t)->ResolveAttribute(state->mThemeAttrs, layout, (ITypedArray**)&a);
    // try {
    if (FAILED(UpdateStateFromTypedArray(a))) {
        a->Recycle();
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (XmlPullParserException e) {
    //     throw new RuntimeException(e);
    // } finally {
    return a->Recycle();
    // }
}

ECode NinePatchDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mNinePatchState != NULL && mNinePatchState->mThemeAttrs != NULL;
    return NOERROR;
}

ECode NinePatchDrawable::GetPaint(
    /* [out] */ IPaint** paint)
{
    VALIDATE_NOT_NULL(paint);
    VALIDATE_NOT_NULL(paint);
    if (mPaint == NULL) {
        ASSERT_SUCCEEDED(CPaint::New((IPaint**)&mPaint));
        mPaint->SetDither(DEFAULT_DITHER);
    }
    *paint = mPaint;
    REFCOUNT_ADD(*paint);
    return NOERROR;
}

Int32 NinePatchDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mBitmapWidth;
    return NOERROR;;
}

Int32 NinePatchDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mBitmapHeight;
    return NOERROR;;
}

ECode NinePatchDrawable::GetMinimumWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mBitmapWidth;
    return NOERROR;
}

ECode NinePatchDrawable::GetMinimumHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mBitmapHeight;
    return NOERROR;
}

ECode NinePatchDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    Boolean hasAlpha;
    Int32 alpha;
    mNinePatch->HasAlpha(&hasAlpha);
    *opacity = (hasAlpha || (mPaint != NULL && (mPaint->GetAlpha(&alpha), alpha < 255))) ?
           IPixelFormat::TRANSLUCENT : IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode NinePatchDrawable::GetTransparentRegion(
    /* [out] */ IRegion** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    AutoPtr<IRect> tmp;
    GetBounds((IRect**)&tmp);
    return mNinePatch->GetTransparentRegion(tmp, bounds);
}

ECode NinePatchDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    GetChangingConfigurations(&mNinePatchState->mChangingConfigurations);
    *state = (IDrawableConstantState*)mNinePatchState.Get();
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode NinePatchDrawable::Mutate()
{
    if (!mMutated) {
        mNinePatchState = new NinePatchState(mNinePatchState);
        mNinePatch = mNinePatchState->mNinePatch;
        mMutated = TRUE;
    }

    return NOERROR;
}

Boolean NinePatchDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>& stateSet)
{
    AutoPtr<NinePatchState> state = mNinePatchState;
    if (state->mTint != NULL && state->mTintMode != -1) {
        mTintFilter = UpdateTintFilter(mTintFilter, state->mTint, state->mTintMode);
        return TRUE;
    }

    return FALSE;
}

ECode NinePatchDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    AutoPtr<NinePatchState> s = mNinePatchState;
    Boolean tmp = FALSE;
    *isStateful = (Drawable::IsStateful(isStateful), *isStateful) || (s->mTint != NULL && (s->mTint->IsStateful(&tmp), tmp));
    return NOERROR;
}

ECode NinePatchDrawable::constructor(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ const String& srcName)
{
    AutoPtr<INinePatch> patch;
    FAIL_RETURN(CNinePatch::New(bitmap, chunk, srcName, (INinePatch**)&patch));
    AutoPtr<NinePatchState> state = new NinePatchState(patch, padding);
    return constructor(state, NULL, NULL);
}

ECode NinePatchDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ const String& srcName)
{
    AutoPtr<INinePatch> patch;
    FAIL_RETURN(CNinePatch::New(bitmap, chunk, srcName, (INinePatch**)&patch));
    AutoPtr<NinePatchState> state = new NinePatchState(patch, padding);
    FAIL_RETURN(constructor(state, res, NULL));
    mNinePatchState->mTargetDensity = mTargetDensity;
    return NOERROR;
}

ECode NinePatchDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ IRect* opticalInsets,
    /* [in] */ const String& srcName)
{
    AutoPtr<INinePatch> ninePatch;
    FAIL_RETURN(CNinePatch::New(bitmap, chunk, srcName, (INinePatch**)&ninePatch));
    AutoPtr<NinePatchState> state = new NinePatchState(ninePatch, padding, opticalInsets);
    FAIL_RETURN(constructor(state, res, NULL));
    mNinePatchState->mTargetDensity = mTargetDensity;
    return NOERROR;
}

ECode NinePatchDrawable::constructor(
    /* [in] */ INinePatch* patch)
{
    AutoPtr<IRect> rect;
    FAIL_RETURN(CRect::New((IRect**)&rect));
    AutoPtr<NinePatchState> state = new NinePatchState(patch, rect);
    return constructor(state, NULL, NULL);
}

ECode NinePatchDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ INinePatch* patch)
{
    AutoPtr<IRect> rect;
    FAIL_RETURN(CRect::New((IRect**)&rect));
    AutoPtr<NinePatchState> state = new NinePatchState(patch, rect);
    FAIL_RETURN(constructor(state, res, NULL));
    mNinePatchState->mTargetDensity = mTargetDensity;
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
