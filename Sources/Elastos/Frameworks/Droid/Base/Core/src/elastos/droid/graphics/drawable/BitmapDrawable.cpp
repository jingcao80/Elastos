
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/graphics/drawable/BitmapDrawable.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmapShader.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/view/CGravity.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::Utility::ILayoutDirection;
using Elastos::Droid::R;
using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

BitmapDrawable::BitmapState::BitmapState(
    /* [in] */ IBitmap* bitmap)
    : mBitmap(bitmap)
    , mTintMode(Drawable::DEFAULT_TINT_MODE)
    , mGravity(IGravity::FILL)
    , mBaseAlpha(1.0)
    , mTileModeX(-1)
    , mTileModeY(-1)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mAutoMirrored(FALSE)
    , mChangingConfigurations(0)
    , mRebuildShader(FALSE)
{
    ASSERT_SUCCEEDED(CPaint::New(DEFAULT_PAINT_FLAGS, (IPaint**)&mPaint));
}

BitmapDrawable::BitmapState::BitmapState(
    /* [in] */ BitmapState* state)
    : mTintMode(Drawable::DEFAULT_TINT_MODE)
    , mGravity(IGravity::FILL)
    , mBaseAlpha(1.0)
    , mTileModeX(-1)
    , mTileModeY(-1)
    , mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mAutoMirrored(FALSE)
    , mChangingConfigurations(0)
    , mRebuildShader(FALSE)
{
    mBitmap = state->mBitmap;
    mTint = state->mTint;
    mTintMode = state->mTintMode;
    mThemeAttrs = state->mThemeAttrs;
    mChangingConfigurations = state->mChangingConfigurations;
    mGravity = state->mGravity;
    mTileModeX = state->mTileModeX;
    mTileModeY = state->mTileModeY;
    mTargetDensity = state->mTargetDensity;
    mBaseAlpha = state->mBaseAlpha;
    ASSERT_SUCCEEDED(CPaint::New(state->mPaint, (IPaint**)&mPaint));
    mRebuildShader = state->mRebuildShader;
    mAutoMirrored = state->mAutoMirrored;
}

ECode BitmapDrawable::BitmapState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mThemeAttrs != NULL;
    return NOERROR;
}

ECode BitmapDrawable::BitmapState::GetBitmap(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = mBitmap;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode BitmapDrawable::BitmapState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CBitmapDrawable::New(this, NULL, NULL, drawable);
}

ECode BitmapDrawable::BitmapState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CBitmapDrawable::New(this, res, NULL, drawable);
}

ECode BitmapDrawable::BitmapState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CBitmapDrawable::New(this, res, theme, drawable);
}

ECode BitmapDrawable::BitmapState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}

//////////////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(BitmapDrawable, Drawable, IBitmapDrawable);

const Int32 BitmapDrawable::DEFAULT_PAINT_FLAGS =
        IPaint::FILTER_BITMAP_FLAG | IPaint::DITHER_FLAG;
const Int32 BitmapDrawable::TILE_MODE_UNDEFINED = -2;
const Int32 BitmapDrawable::TILE_MODE_DISABLED = -1;
const Int32 BitmapDrawable::TILE_MODE_CLAMP = 0;
const Int32 BitmapDrawable::TILE_MODE_REPEAT = 1;
const Int32 BitmapDrawable::TILE_MODE_MIRROR = 2;

BitmapDrawable::BitmapDrawable()
    : mTargetDensity(IDisplayMetrics::DENSITY_DEFAULT)
    , mDstRectAndInsetsDirty(TRUE)
    , mMutated(FALSE)
    , mBitmapWidth(0)
    , mBitmapHeight(0)
    , mOpticalInsets(Insets::NONE)
{
    CRect::New((IRect**)&mDstRect);
}

ECode BitmapDrawable::constructor()
{
    mBitmapState = new BitmapState((IBitmap*)NULL);
    return NOERROR;
}

ECode BitmapDrawable::constructor(
    /* [in] */ IResources* res)
{
    mBitmapState = new BitmapState((IBitmap*)NULL);
    mBitmapState->mTargetDensity = mTargetDensity;
    return NOERROR;
}

ECode BitmapDrawable::constructor(
    /* [in] */ IBitmap* bitmap)
{
    AutoPtr<BitmapState> state = new BitmapState(bitmap);
    return constructor((IDrawableConstantState*)state, NULL, NULL);
}

ECode BitmapDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap)
{
    AutoPtr<BitmapState> state = new BitmapState(bitmap);
    constructor(state, res, NULL);
    mBitmapState->mTargetDensity = mTargetDensity;
    return NOERROR;
}

ECode BitmapDrawable::constructor(
    /* [in] */ const String& filepath)
{
    AutoPtr<IBitmap> bitmap;
    FAIL_RETURN(BitmapFactory::DecodeFile(filepath, (IBitmap**)&bitmap));

    AutoPtr<BitmapState> state = new BitmapState(bitmap);
    constructor(state, NULL, NULL);
    if (mBitmapState->mBitmap == NULL) {
        Logger::W("BitmapDrawable", "BitmapDrawable cannot decode %s", filepath.string());
    }
    return NOERROR;
}

ECode BitmapDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ const String& filepath)
{
    AutoPtr<IBitmap> bitmap;
    FAIL_RETURN(BitmapFactory::DecodeFile(filepath, (IBitmap**)&bitmap));

    AutoPtr<BitmapState> state = new BitmapState(bitmap);
    constructor(state, NULL, NULL);
    mBitmapState->mTargetDensity = mTargetDensity;
    if (mBitmapState->mBitmap == NULL) {
        Logger::W("BitmapDrawable", "BitmapDrawable cannot decode %s", filepath.string());
    }
    return NOERROR;
}

ECode BitmapDrawable::constructor(
    /* [in] */ IInputStream* is)
{
    AutoPtr<IBitmap> bitmap;
    FAIL_RETURN(BitmapFactory::DecodeStream(is, (IBitmap**)&bitmap));

    AutoPtr<BitmapState> state = new BitmapState(bitmap);
    constructor(state, NULL, NULL);
    if (mBitmapState->mBitmap == NULL) {
        Logger::W("BitmapDrawable", "BitmapDrawable cannot decode"/*is*/);
    }
    return NOERROR;
}

ECode BitmapDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IInputStream* is)
{
    AutoPtr<IBitmap> bitmap;
    FAIL_RETURN(BitmapFactory::DecodeStream(is, (IBitmap**)&bitmap));

    AutoPtr<BitmapState> state = new BitmapState(bitmap);
    constructor((IDrawableConstantState*)state, NULL, NULL);
    mBitmapState->mTargetDensity = mTargetDensity;
    if (mBitmapState->mBitmap == NULL) {
        Logger::W("BitmapDrawable", "BitmapDrawable cannot decode"/*is*/);
    }
    return NOERROR;
}

ECode BitmapDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    Boolean can = FALSE;
    if (theme != NULL && (state->CanApplyTheme(&can), can)) {
        // If we need to apply a theme, implicitly mutate.
        mBitmapState = new BitmapState((BitmapState*)state);
        ApplyTheme(theme);
    }
    else {
        mBitmapState = (BitmapState*)state;
    }

    return InitializeWithState((BitmapState*)state, res);
}

ECode BitmapDrawable::GetPaint(
    /* [out] */ IPaint** paint)
{
    VALIDATE_NOT_NULL(paint);
    *paint = mBitmapState->mPaint;
    REFCOUNT_ADD(*paint);
    return NOERROR;
}

ECode BitmapDrawable::GetBitmap(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = mBitmapState->mBitmap;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

void BitmapDrawable::ComputeBitmapSize()
{
    AutoPtr<IBitmap> bitmap = mBitmapState->mBitmap;
    if (bitmap != NULL) {
        bitmap->GetScaledWidth(mTargetDensity, &mBitmapWidth);
        bitmap->GetScaledHeight(mTargetDensity, &mBitmapHeight);
    }
    else {
        mBitmapWidth = mBitmapHeight = -1;
    }
}

void BitmapDrawable::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    if (mBitmapState->mBitmap.Get() != bitmap) {
        mBitmapState->mBitmap = bitmap;
        ComputeBitmapSize();
        InvalidateSelf();
    }
}

ECode BitmapDrawable::SetTargetDensity(
    /* [in] */ ICanvas* canvas)
{
    Int32 density;
    canvas->GetDensity(&density);
    return SetTargetDensity(density);
}

ECode BitmapDrawable::SetTargetDensity(
    /* [in] */ IDisplayMetrics* metrics)
{
    Int32 dpi;
    metrics->GetDensityDpi(&dpi);
    return SetTargetDensity(dpi);
}

ECode BitmapDrawable::SetTargetDensity(
    /* [in] */ Int32 density)
{
    if (mTargetDensity != density) {
        mTargetDensity = density == 0 ? IDisplayMetrics::DENSITY_DEFAULT : density;
        if (mBitmapState->mBitmap != NULL) {
            ComputeBitmapSize();
        }
        InvalidateSelf();
    }
    return NOERROR;
}

ECode BitmapDrawable::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mBitmapState->mGravity;
    return NOERROR;
}

ECode BitmapDrawable::SetGravity(
    /* [in] */ Int32 gravity)
{
    if (mBitmapState->mGravity != gravity) {
        mBitmapState->mGravity = gravity;
        mDstRectAndInsetsDirty = TRUE;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode BitmapDrawable::SetMipMap(
    /* [in] */ Boolean mipMap)
{
    if (mBitmapState->mBitmap != NULL) {
        mBitmapState->mBitmap->SetHasMipMap(mipMap);
        InvalidateSelf();
    }
    return NOERROR;
}

ECode BitmapDrawable::HasMipMap(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    mBitmapState->mBitmap->HasMipMap(has);
    *has = mBitmapState->mBitmap != NULL && (*has);
    return NOERROR;
}

ECode BitmapDrawable::SetAntiAlias(
    /* [in] */ Boolean aa)
{
    mBitmapState->mPaint->SetAntiAlias(aa);
    return InvalidateSelf();
}

ECode BitmapDrawable::HasAntiAlias(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    return mBitmapState->mPaint->IsAntiAlias(has);
}

ECode BitmapDrawable::SetFilterBitmap(
    /* [in] */ Boolean filter)
{
    mBitmapState->mPaint->SetFilterBitmap(filter);
    return InvalidateSelf();
}

ECode BitmapDrawable::SetDither(
    /* [in] */ Boolean dither)
{
    mBitmapState->mPaint->SetDither(dither);
    return InvalidateSelf();
}

ECode BitmapDrawable::GetTileModeX(
    /* [out] */ ShaderTileMode* tileModeX)
{
    VALIDATE_NOT_NULL(tileModeX)
    *tileModeX = mBitmapState->mTileModeX;
    return NOERROR;
}

ECode BitmapDrawable::GetTileModeY(
    /* [out] */ ShaderTileMode* tileModeY)
{
    VALIDATE_NOT_NULL(tileModeY);
    *tileModeY = mBitmapState->mTileModeY;
    return NOERROR;
}

ECode BitmapDrawable::SetTileModeX(
    /* [in] */ ShaderTileMode mode)
{
    return SetTileModeXY(mode, mBitmapState->mTileModeY);
}

ECode BitmapDrawable::SetTileModeY(
    /* [in] */ ShaderTileMode mode)
{
    return SetTileModeXY(mBitmapState->mTileModeX, mode);
}

ECode BitmapDrawable::SetTileModeXY(
    /* [in] */ ShaderTileMode xmode,
    /* [in] */ ShaderTileMode ymode)
{
    BitmapState* state = mBitmapState;
    if (state->mTileModeX != xmode || state->mTileModeY != ymode) {
        state->mTileModeX = xmode;
        state->mTileModeY = ymode;
        state->mRebuildShader = TRUE;
        mDstRectAndInsetsDirty = TRUE;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode BitmapDrawable::SetAutoMirrored(
    /* [in] */ Boolean mirrored)
{
    if (mBitmapState->mAutoMirrored != mirrored) {
        mBitmapState->mAutoMirrored = mirrored;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode BitmapDrawable::IsAutoMirrored(
    /* [out] */ Boolean* mirrored)
{
    VALIDATE_NOT_NULL(mirrored);
    *mirrored = mBitmapState->mAutoMirrored;
    return NOERROR;
}

ECode BitmapDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Drawable::GetChangingConfigurations(configuration);
    *configuration = (*configuration)
        | mBitmapState->mChangingConfigurations;
    return NOERROR;
}

Boolean BitmapDrawable::NeedMirroring()
{
    Int32 dir = 0;
    Boolean mirrored = FALSE;
    return (IsAutoMirrored(&mirrored), mirrored) && (GetLayoutDirection(&dir), dir) == ILayoutDirection::RTL;
}

void BitmapDrawable::UpdateMirrorMatrix(
    /* [in] */ Float dx)
{
    if (mMirrorMatrix == NULL) {
        CMatrix::New((IMatrix**)&mMirrorMatrix);
    }
    mMirrorMatrix->SetTranslate(dx, 0);
    Boolean res = FALSE;
    mMirrorMatrix->PreScale(-1.0, 1.0, &res);
}

void BitmapDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    mDstRectAndInsetsDirty = TRUE;

    AutoPtr<IShader> shader;
    mBitmapState->mPaint->GetShader((IShader**)&shader);
    if (shader != NULL) {
        if (NeedMirroring()) {
            Int32 left = 0, right = 0;
            bounds->GetLeft(&left);
            bounds->GetRight(&right);
            UpdateMirrorMatrix(right - left);
            shader->SetLocalMatrix(mMirrorMatrix);
            mBitmapState->mPaint->SetShader(shader);
        }
        else {
            if (mMirrorMatrix != NULL) {
                mMirrorMatrix = NULL;
                shader->SetLocalMatrix(CMatrix::IDENTITY_MATRIX);
                mBitmapState->mPaint->SetShader(shader);
            }
        }
    }
}

ECode BitmapDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IBitmap> bitmap = mBitmapState->mBitmap;
    if (bitmap == NULL) {
        return NOERROR;
    }

    AutoPtr<BitmapState> state = mBitmapState;
    AutoPtr<IPaint> paint = state->mPaint;
    if (state->mRebuildShader) {
        ShaderTileMode tmx = state->mTileModeX;
        ShaderTileMode tmy = state->mTileModeY;
        if (tmx == -1 && tmy == -1) {
            paint->SetShader(NULL);
        }
        else {
            AutoPtr<IShader> s;
            CBitmapShader::New(bitmap,
                tmx == -1 ? ShaderTileMode_CLAMP : tmx,
                tmy == -1 ? ShaderTileMode_CLAMP : tmy, (IShader**)&s);
            paint->SetShader(s);
        }

        state->mRebuildShader = FALSE;
    }

    Int32 restoreAlpha = 0;
    if (state->mBaseAlpha != 1.0f) {
        AutoPtr<IPaint> p;
        GetPaint((IPaint**)&p);
        p->GetAlpha(&restoreAlpha);
        Int32 newAlpha = (Int32) (restoreAlpha * state->mBaseAlpha + 0.5f);
        p->SetAlpha(newAlpha);
    }
    else {
        restoreAlpha = -1;
    }

    Boolean clearColorFilter = FALSE;
    AutoPtr<IColorFilter> cf;
    if (mTintFilter != NULL && (paint->GetColorFilter((IColorFilter**)&cf), cf) == NULL) {
        paint->SetColorFilter(IColorFilter::Probe(mTintFilter));
        clearColorFilter = TRUE;
    }
    else {
        clearColorFilter = FALSE;
    }

    UpdateDstRectAndInsetsIfDirty();
    AutoPtr<IShader> shader;
    paint->GetShader((IShader**)&shader);
    Boolean needMirroring = NeedMirroring();
    if (shader == NULL) {
        if (needMirroring) {
            Int32 value = 0;
            canvas->Save(&value);
            // Mirror the bitmap
            Int32 left = 0, right = 0;
            mDstRect->GetLeft(&left);
            mDstRect->GetRight(&right);
            canvas->Translate(right - left, 0);
            canvas->Scale(-1.0f, 1.0f);
        }

        canvas->DrawBitmap(bitmap, NULL, mDstRect, paint);

        if (needMirroring) {
            canvas->Restore();
        }
    }
    else {
        if (needMirroring) {
            // Mirror the bitmap
            Int32 left = 0, right = 0;
            mDstRect->GetLeft(&left);
            mDstRect->GetRight(&right);
            UpdateMirrorMatrix(right - left);
            shader->SetLocalMatrix(mMirrorMatrix);
            paint->SetShader(shader);
        }
        else {
            if (mMirrorMatrix != NULL) {
                mMirrorMatrix = NULL;
                shader->SetLocalMatrix(CMatrix::IDENTITY_MATRIX);
                paint->SetShader(shader);
            }
        }

        canvas->DrawRect(mDstRect, paint);
    }

    if (clearColorFilter) {
        paint->SetColorFilter(NULL);
    }

    if (restoreAlpha >= 0) {
        paint->SetAlpha(restoreAlpha);
    }
    return NOERROR;
}

void BitmapDrawable::UpdateDstRectAndInsetsIfDirty()
{
    if (mDstRectAndInsetsDirty) {
        if (mBitmapState->mTileModeX == -1 && mBitmapState->mTileModeY == -1) {
            AutoPtr<IRect> bounds;
            GetBounds((IRect**)&bounds);
            Int32 layoutDirection = 0;
            GetLayoutDirection(&layoutDirection);

            AutoPtr<IGravity> gravity;
            CGravity::AcquireSingleton((IGravity**)&gravity);
            gravity->Apply(mBitmapState->mGravity, mBitmapWidth, mBitmapHeight,
                    bounds, mDstRect, layoutDirection);

            CRect* dr = (CRect*)mDstRect.Get();
            CRect* bd = (CRect*)bounds.Get();
            Int32 left = dr->mLeft - bd->mLeft;
            Int32 top = dr->mTop - bd->mTop;
            Int32 right = bd->mRight - dr->mRight;
            Int32 bottom = bd->mBottom - dr->mBottom;
            mOpticalInsets = Insets::Of(left, top, right, bottom);
        }
        else {
            CopyBounds(mDstRect);
            mOpticalInsets = Insets::NONE;
        }
    }
    mDstRectAndInsetsDirty = FALSE;
}

ECode BitmapDrawable::GetOpticalInsets(
    /* [out] */ IInsets** sets)
{
    VALIDATE_NOT_NULL(sets);
    UpdateDstRectAndInsetsIfDirty();
    *sets = mOpticalInsets;
    REFCOUNT_ADD(*sets);
    return NOERROR;
}

ECode BitmapDrawable::GetOutline(
    /* [in] */ /*@NonNull*/ IOutline* outline)
{
    UpdateDstRectAndInsetsIfDirty();
    outline->SetRect(mDstRect);

    // Only opaque Bitmaps can report a non-0 alpha,
    // since only they are guaranteed to fill their bounds
    Boolean has = FALSE;
    Boolean opaqueOverShape = mBitmapState->mBitmap != NULL
            && !(mBitmapState->mBitmap->HasAlpha(&has), has);

    Int32 alpha = 0;
    GetAlpha(&alpha);
    return outline->SetAlpha(opaqueOverShape ? alpha / 255.0f : 0.0f);
}

ECode BitmapDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    Int32 oldAlpha = 0;
    mBitmapState->mPaint->GetAlpha(&oldAlpha);
    if (alpha != oldAlpha) {
        mBitmapState->mPaint->SetAlpha(alpha);
        InvalidateSelf();
    }
    return NOERROR;
}

ECode BitmapDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    return mBitmapState->mPaint->GetAlpha(alpha);
}

ECode BitmapDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    mBitmapState->mPaint->SetColorFilter(cf);
    return InvalidateSelf();
}

ECode BitmapDrawable::GetColorFilter(
    /* [out] */ IColorFilter** filter)
{
    return mBitmapState->mPaint->GetColorFilter(filter);
}

ECode BitmapDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    mBitmapState->mTint = tint;
    mTintFilter = UpdateTintFilter(mTintFilter, tint, mBitmapState->mTintMode);
    return InvalidateSelf();
}

ECode BitmapDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    mBitmapState->mTintMode = tintMode;
    mTintFilter = UpdateTintFilter(mTintFilter, mBitmapState->mTint, tintMode);
    return InvalidateSelf();
}

ECode BitmapDrawable::GetTint(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mBitmapState->mTint;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode BitmapDrawable::GetTintMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mBitmapState->mTintMode;
    return NOERROR;
}

ECode BitmapDrawable::SetXfermode(
    /* [in] */ IXfermode* xfermode)
{
    mBitmapState->mPaint->SetXfermode(xfermode);
    return InvalidateSelf();
}

ECode BitmapDrawable::Mutate()
{
    if (!mMutated ) {
        mBitmapState = new BitmapState(mBitmapState);
        mMutated = TRUE;
    }
    return NOERROR;
}

Boolean BitmapDrawable::OnStateChange(
    /* [in] */ ArrayOf<Int32>* stateSet)
{
    AutoPtr<BitmapState> state = mBitmapState;
    if (state->mTint != NULL && state->mTintMode != -1) {
        mTintFilter = UpdateTintFilter(mTintFilter, state->mTint, state->mTintMode);
        return TRUE;
    }
    return FALSE;
}

ECode BitmapDrawable::IsStateful(
    /* [out] */ Boolean* is)
{
    AutoPtr<BitmapState> s = mBitmapState;
    Boolean value = FALSE;
    return (Drawable::IsStateful(&value), value) || (s->mTint != NULL && (s->mTint->IsStateful(&value), value));
}

ECode BitmapDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    FAIL_RETURN(Drawable::Inflate(r, parser, attrs, theme));

    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::BitmapDrawable);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));

    FAIL_RETURN(UpdateStateFromTypedArray(a));
    FAIL_RETURN(VerifyState(a));
    return a->Recycle();
}

ECode BitmapDrawable::VerifyState(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    AutoPtr<BitmapState> state = mBitmapState;
    if (state->mBitmap == NULL) {
        return E_XML_PULL_PARSER_EXCEPTION;
        // throw new XmlPullParserException(a.getPositionDescription() +
        //         ": <bitmap> requires a valid src attribute");
    }
    return NOERROR;
}

ECode BitmapDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    AutoPtr<IResources> r;
    a->GetResources((IResources**)&r);
    AutoPtr<BitmapState> state = mBitmapState;

    // Account for any configuration changes.
    Int32 config = 0;
    state->mChangingConfigurations |= (a->GetChangingConfigurations(&config), config);

    // Extract the theme attributes, if any.
    FAIL_RETURN(((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mThemeAttrs));

    Int32 srcResId = 0;
    a->GetResourceId(R::styleable::BitmapDrawable_src, 0, &srcResId);
    if (srcResId != 0) {
        AutoPtr<IBitmap> bitmap;
        BitmapFactory::DecodeResource(r, srcResId, (IBitmap**)&bitmap);
        if (bitmap == NULL) {
            // throw new XmlPullParserException(a.getPositionDescription() +
            //         ": <bitmap> requires a valid src attribute");
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        state->mBitmap = bitmap;
    }

    AutoPtr<IDisplayMetrics> metrics;
    r->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    metrics->GetDensityDpi(&state->mTargetDensity);

    Boolean value = FALSE;
    Boolean defMipMap = state->mBitmap != NULL ? (state->mBitmap->HasMipMap(&value), value) : FALSE;
    a->GetBoolean(R::styleable::BitmapDrawable_mipMap, defMipMap, &value);
    SetMipMap(value);

    a->GetBoolean(R::styleable::BitmapDrawable_autoMirrored, state->mAutoMirrored, &state->mAutoMirrored);
    a->GetFloat(R::styleable::BitmapDrawable_alpha, state->mBaseAlpha, &state->mBaseAlpha);

    Int32 tintMode = 0;
    a->GetInt32(R::styleable::BitmapDrawable_tintMode, -1, &tintMode);
    if (tintMode != -1) {
        Drawable::ParseTintMode(tintMode, PorterDuffMode_SRC_IN, &state->mTintMode);
    }

    AutoPtr<IColorStateList> tint;
    a->GetColorStateList(R::styleable::BitmapDrawable_tint, (IColorStateList**)&tint);
    if (tint != NULL) {
        state->mTint = tint;
    }

    AutoPtr<IPaint> paint = mBitmapState->mPaint;
    Boolean res = FALSE;
    paint->IsAntiAlias(&res);
    a->GetBoolean(R::styleable::BitmapDrawable_antialias, res, &value);
    paint->SetAntiAlias(value);
    paint->IsFilterBitmap(&res);
    a->GetBoolean(R::styleable::BitmapDrawable_filter, res, &value);
    paint->SetFilterBitmap(value);
    paint->IsDither(&res);
    a->GetBoolean(R::styleable::BitmapDrawable_dither, res, &value);
    paint->SetDither(value);

    Int32 gravity = 0;
    a->GetInt32(R::styleable::BitmapDrawable_gravity, state->mGravity, &gravity);
    SetGravity(gravity);

    Int32 tileMode = 0;
    a->GetInt32(R::styleable::BitmapDrawable_tileMode, TILE_MODE_UNDEFINED, &tileMode);
    if (tileMode != TILE_MODE_UNDEFINED) {
        ShaderTileMode mode = ParseTileMode(tileMode);
        SetTileModeXY(mode, mode);
    }

    Int32 tileModeX = 0;
    a->GetInt32(R::styleable::BitmapDrawable_tileModeX, TILE_MODE_UNDEFINED, &tileModeX);
    if (tileModeX != TILE_MODE_UNDEFINED) {
        SetTileModeX(ParseTileMode(tileModeX));
    }

    Int32 tileModeY = 0;
    a->GetInt32(R::styleable::BitmapDrawable_tileModeY, TILE_MODE_UNDEFINED, &tileModeY);
    if (tileModeY != TILE_MODE_UNDEFINED) {
        SetTileModeY(ParseTileMode(tileModeY));
    }

    // Update local properties.
    return InitializeWithState(state, r);
}

ECode BitmapDrawable::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    Drawable::ApplyTheme(t);

    AutoPtr<BitmapState> state = mBitmapState;
    if (state == NULL || state->mThemeAttrs == NULL) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::BitmapDrawable);

    AutoPtr<ITypedArray> a;
    ((CResources::Theme*)t)->ResolveAttribute(state->mThemeAttrs, layout, (ITypedArray**)&a);
    // try {
    if (UpdateStateFromTypedArray(a) == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        a->Recycle();
        return E_ANDROID_RUNTIME_EXCEPTION;
    }
    // } catch (XmlPullParserException e) {
    //     throw new RuntimeException(e);
    // } finally {
        // a.recycle();
    // }
    a->Recycle();
    return NOERROR;
}

ShaderTileMode BitmapDrawable::ParseTileMode(
    /* [in] */ Int32 tileMode)
{
    switch (tileMode) {
        case TILE_MODE_CLAMP:
            return ShaderTileMode_CLAMP;
        case TILE_MODE_REPEAT:
            return ShaderTileMode_REPEAT;
        case TILE_MODE_MIRROR:
            return ShaderTileMode_MIRROR;
        default:
            return -1;
    }
    return -1;
}

ECode BitmapDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mBitmapState != NULL && mBitmapState->mThemeAttrs != NULL;
    return NOERROR;
}

ECode BitmapDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mBitmapWidth;
    return NOERROR;
}

ECode BitmapDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mBitmapHeight;
    return NOERROR;
}

ECode BitmapDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    if (mBitmapState->mGravity != IGravity::FILL) {
        *opacity = IPixelFormat::TRANSLUCENT;
        return NOERROR;
    }

    Boolean bmHasAlpha = FALSE;
    if (mBitmapState->mBitmap != NULL) {
        mBitmapState->mBitmap->HasAlpha(&bmHasAlpha);
    }

    Int32 alpha;
    mBitmapState->mPaint->GetAlpha(&alpha);

    *opacity = (bmHasAlpha || alpha < 255) ?
            IPixelFormat::TRANSLUCENT : IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode BitmapDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    GetChangingConfigurations(&mBitmapState->mChangingConfigurations);
    *state = mBitmapState;
    REFCOUNT_ADD(*state);
    return NOERROR;
}


ECode BitmapDrawable::InitializeWithState(
    /* [in] */ BitmapState* state,
    /* [in] */ IResources* res)
{
    if (res != NULL) {
        AutoPtr<IDisplayMetrics> metrics;
        res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
        metrics->GetDensityDpi(&mTargetDensity);
    } else {
        mTargetDensity = state->mTargetDensity;
    }

    mTintFilter = UpdateTintFilter(mTintFilter, state->mTint, state->mTintMode);
    ComputeBitmapSize();
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
