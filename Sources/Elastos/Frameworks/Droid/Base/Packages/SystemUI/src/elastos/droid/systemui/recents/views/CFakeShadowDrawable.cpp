//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/recents/views/CFakeShadowDrawable.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "../R.h"

using Elastos::Droid::Graphics::CLinearGradient;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CRadialGradient;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PathFillType_EVEN_ODD;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

// used to calculate content padding
const Double CFakeShadowDrawable::COS_45 = Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(45));
const Float CFakeShadowDrawable::SHADOW_MULTIPLIER;

CAR_OBJECT_IMPL(CFakeShadowDrawable)

CFakeShadowDrawable::CFakeShadowDrawable()
    : mInsetShadow(.0)
    , mCornerRadius(.0)
    , mMaxShadowSize(.0)
    , mRawMaxShadowSize(.0)
    , mShadowSize(.0)
    , mRawShadowSize(.0)
    , mDirty(TRUE)
    , mShadowStartColor(0)
    , mShadowEndColor(0)
    , mAddPaddingForCorners(TRUE)
    , mPrintedShadowClipWarning(FALSE)
{}

ECode CFakeShadowDrawable::constructor(
    /* [in] */ IResources* resources,
    /* [in] */ IRecentsConfiguration* config)
{
    resources->GetColor(R::color::fake_shadow_start_color, &mShadowStartColor);
    resources->GetColor(R::color::fake_shadow_end_color, &mShadowEndColor);
    resources->GetDimension(R::dimen::fake_shadow_inset, &mInsetShadow);
    Int32 dpSize;
    resources->GetDimensionPixelSize(R::dimen::fake_shadow_size, &dpSize);
    SetShadowSize(dpSize, dpSize);
    CPaint::New(IPaint::ANTI_ALIAS_FLAG | IPaint::DITHER_FLAG, (IPaint**)&mCornerShadowPaint);
    mCornerShadowPaint->SetStyle(PaintStyle_FILL);
    mCornerShadowPaint->SetDither(TRUE);
    RecentsConfiguration* rc = (RecentsConfiguration*)config;
    mCornerRadius = rc->mTaskViewRoundedCornerRadiusPx;
    CRectF::New((IRectF**)&mCardBounds);
    CPaint::New(mCornerShadowPaint, (IPaint**)&mEdgeShadowPaint);
    return Drawable::constructor();
}

// @Override
ECode CFakeShadowDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    mCornerShadowPaint->SetAlpha(alpha);
    mEdgeShadowPaint->SetAlpha(alpha);
    return NOERROR;
}

// @Override
void CFakeShadowDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    Drawable::OnBoundsChange(bounds);
    mDirty = TRUE;
}

ECode CFakeShadowDrawable::SetShadowSize(
    /* [in] */ Float shadowSize,
    /* [in] */ Float maxShadowSize)
{
    if (shadowSize < 0 || maxShadowSize < 0) {
        Logger::E("CFakeShadowDrawable", "invalid shadow size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (shadowSize > maxShadowSize) {
        shadowSize = maxShadowSize;
        if (!mPrintedShadowClipWarning) {
            Logger::W("CardView", "Shadow size is being clipped by the max shadow size. See "
                "{CardView#setMaxCardElevation}.");
            mPrintedShadowClipWarning = TRUE;
        }
    }
    if (mRawShadowSize == shadowSize && mRawMaxShadowSize == maxShadowSize) {
        return NOERROR;
    }
    mRawShadowSize = shadowSize;
    mRawMaxShadowSize = maxShadowSize;
    mShadowSize = shadowSize * SHADOW_MULTIPLIER + mInsetShadow;
    mMaxShadowSize = maxShadowSize + mInsetShadow;
    mDirty = TRUE;
    InvalidateSelf();
    return NOERROR;
}

// @Override
ECode CFakeShadowDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 vOffset = (Int32)Elastos::Core::Math::Ceil(
        CalculateVerticalPadding(mRawMaxShadowSize, mCornerRadius, mAddPaddingForCorners));
    Int32 hOffset = (Int32)Elastos::Core::Math::Ceil(
        CalculateHorizontalPadding(mRawMaxShadowSize, mCornerRadius, mAddPaddingForCorners));
    padding->Set(hOffset, vOffset, hOffset, vOffset);
    *result = TRUE;
    return NOERROR;
}

Float CFakeShadowDrawable::CalculateVerticalPadding(
    /* [in] */ Float maxShadowSize,
    /* [in] */ Float cornerRadius,
    /* [in] */ Boolean addPaddingForCorners)
{
    if (addPaddingForCorners) {
        return (Float) (maxShadowSize * SHADOW_MULTIPLIER + (1 - COS_45) * cornerRadius);
    }
    else {
        return maxShadowSize * SHADOW_MULTIPLIER;
    }
}

Float CFakeShadowDrawable::CalculateHorizontalPadding(
    /* [in] */ Float maxShadowSize,
    /* [in] */ Float cornerRadius,
    /* [in] */ Boolean addPaddingForCorners)
{
    if (addPaddingForCorners) {
        return (Float) (maxShadowSize + (1 - COS_45) * cornerRadius);
    }
    else {
        return maxShadowSize;
    }
}

// @Override
ECode CFakeShadowDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    mCornerShadowPaint->SetColorFilter(cf);
    mEdgeShadowPaint->SetColorFilter(cf);
    return NOERROR;
}

// @Override
ECode CFakeShadowDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode CFakeShadowDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (mDirty) {
        AutoPtr<IRect> rect;
        GetBounds((IRect**)&rect);
        BuildComponents(rect);
        mDirty = FALSE;
    }
    canvas->Translate(0, mRawShadowSize / 4);
    DrawShadow(canvas);
    canvas->Translate(0, -mRawShadowSize / 4);
    return NOERROR;
}

void CFakeShadowDrawable::DrawShadow(
    /* [in] */ ICanvas* canvas)
{
    Float edgeShadowTop = -mCornerRadius - mShadowSize;
    Float inset = mCornerRadius + mInsetShadow + mRawShadowSize / 2;
    Float width, height;
    mCardBounds->GetWidth(&width);
    mCardBounds->GetHeight(&height);
    Boolean drawHorizontalEdges = width - 2 * inset > 0;
    Boolean drawVerticalEdges = height - 2 * inset > 0;
    // LT
    Int32 saved;
    canvas->Save(&saved);
    Float left, right, top, bottom;
    mCardBounds->Get(&left, &top, &right, &bottom);
    canvas->Translate(left + inset, top + inset);
    canvas->DrawPath(mCornerShadowPath, mCornerShadowPaint);
    if (drawHorizontalEdges) {
        canvas->DrawRect(0, edgeShadowTop, width - 2 * inset,
            -mCornerRadius, mEdgeShadowPaint);
    }
    canvas->RestoreToCount(saved);
    // RB
    canvas->Save(&saved);
    canvas->Translate(right - inset, bottom - inset);
    canvas->Rotate(180.0f);
    canvas->DrawPath(mCornerShadowPath, mCornerShadowPaint);
    if (drawHorizontalEdges) {
        canvas->DrawRect(0, edgeShadowTop, width - 2 * inset,
            -mCornerRadius + mShadowSize, mEdgeShadowPaint);
    }
    canvas->RestoreToCount(saved);
    // LB
    canvas->Save(&saved);
    canvas->Translate(left + inset, bottom - inset);
    canvas->Rotate(270.0f);
    canvas->DrawPath(mCornerShadowPath, mCornerShadowPaint);
    if (drawVerticalEdges) {
        canvas->DrawRect(0, edgeShadowTop, height - 2 * inset,
            -mCornerRadius, mEdgeShadowPaint);
    }
    canvas->RestoreToCount(saved);
    // RT
    canvas->Save(&saved);
    canvas->Translate(right - inset, top + inset);
    canvas->Rotate(90.0f);
    canvas->DrawPath(mCornerShadowPath, mCornerShadowPaint);
    if (drawVerticalEdges) {
        canvas->DrawRect(0, edgeShadowTop, height - 2 * inset,
            -mCornerRadius, mEdgeShadowPaint);
    }
    canvas->RestoreToCount(saved);
}

void CFakeShadowDrawable::BuildShadowCorners()
{
    AutoPtr<IRectF> innerBounds;
    CRectF::New(-mCornerRadius, -mCornerRadius, mCornerRadius, mCornerRadius, (IRectF**)&innerBounds);
    AutoPtr<IRectF> outerBounds;
    CRectF::New(innerBounds, (IRectF**)&outerBounds);
    outerBounds->Inset(-mShadowSize, -mShadowSize);

    if (mCornerShadowPath == NULL) {
        CPath::New((IPath**)&mCornerShadowPath);
    }
    else {
        mCornerShadowPath->Reset();
    }
    mCornerShadowPath->SetFillType(PathFillType_EVEN_ODD);
    mCornerShadowPath->MoveTo(-mCornerRadius, 0);
    mCornerShadowPath->RLineTo(-mShadowSize, 0);
    // outer arc
    mCornerShadowPath->ArcTo(outerBounds, 180.0f, 90.0f, FALSE);
    // inner arc
    mCornerShadowPath->ArcTo(innerBounds, 270.0f, -90.0f, FALSE);
    mCornerShadowPath->Close();

    Float startRatio = mCornerRadius / (mCornerRadius + mShadowSize);
    AutoPtr<ArrayOf<Int32> > colors = ArrayOf<Int32>::Alloc(3);
    (*colors)[0] = mShadowStartColor;
    (*colors)[1] = mShadowStartColor;
    (*colors)[2] = mShadowEndColor;
    AutoPtr<ArrayOf<Float> > positions = ArrayOf<Float>::Alloc(3);
    (*positions)[0] = 0.0f;
    (*positions)[1] = startRatio;
    (*positions)[2] = 1.0f;
    AutoPtr<IRadialGradient> radialGradient;
    CRadialGradient::New(0, 0, mCornerRadius + mShadowSize, colors, positions,
        ShaderTileMode_CLAMP, (IRadialGradient**)&radialGradient);
    mCornerShadowPaint->SetShader(IShader::Probe(radialGradient));

    // we offset the content shadowSize/2 pixels up to make it more realistic.
    // this is why edge shadow shader has some extra space
    // When drawing bottom edge shadow, we use that extra space.
    AutoPtr<ArrayOf<Int32> > colors2 = ArrayOf<Int32>::Alloc(3);
    (*colors2)[0] = mShadowStartColor;
    (*colors2)[1] = mShadowStartColor;
    (*colors2)[2] = mShadowEndColor;
    AutoPtr<ArrayOf<Float> > positions2 = ArrayOf<Float>::Alloc(3);
    (*positions2)[0] = 0.0f;
    (*positions2)[1] = 5.0f;
    (*positions2)[2] = 1.0f;
    AutoPtr<ILinearGradient> linearGradient;
    CLinearGradient::New(0, -mCornerRadius + mShadowSize, 0, -mCornerRadius - mShadowSize,
        colors2, positions2, ShaderTileMode_CLAMP, (ILinearGradient**)&linearGradient);
    mEdgeShadowPaint->SetShader(IShader::Probe(linearGradient));
}

void CFakeShadowDrawable::BuildComponents(
    /* [in] */ IRect* bounds)
{
    // Card is offset SHADOW_MULTIPLIER * maxShadowSize to account for the shadow shift.
    // We could have different top-bottom offsets to avoid extra gap above but in that case
    // center aligning Views inside the CardView would be problematic.
    Float verticalOffset = mMaxShadowSize * SHADOW_MULTIPLIER;
    Int32 left, right, top, bottom;
    bounds->Get(&left, &top, &right, &bottom);
    mCardBounds->Set(
        left + mMaxShadowSize, top + verticalOffset,
        right - mMaxShadowSize, bottom - verticalOffset);
    BuildShadowCorners();
}

Float CFakeShadowDrawable::GetMinWidth()
{
    Float content = 2 * Elastos::Core::Math::Max(
        mRawMaxShadowSize, mCornerRadius + mInsetShadow + mRawMaxShadowSize / 2);
    return content + (mRawMaxShadowSize + mInsetShadow) * 2;
}

Float CFakeShadowDrawable::GetMinHeight()
{
    Float content = 2 * Elastos::Core::Math::Max(mRawMaxShadowSize, mCornerRadius + mInsetShadow
        + mRawMaxShadowSize * SHADOW_MULTIPLIER / 2);
    return content + (mRawMaxShadowSize * SHADOW_MULTIPLIER + mInsetShadow) * 2;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
