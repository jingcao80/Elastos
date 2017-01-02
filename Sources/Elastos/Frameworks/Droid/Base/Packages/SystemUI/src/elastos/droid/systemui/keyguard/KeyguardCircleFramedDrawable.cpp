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

#include "elastos/droid/systemui/keyguard/KeyguardCircleFramedDrawable.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(KeyguardCircleFramedDrawable, Drawable,
        IKeyguardCircleFramedDrawable)

KeyguardCircleFramedDrawable::KeyguardCircleFramedDrawable(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 size,
    /* [in] */ Int32 frameColor,
    /* [in] */ Float strokeWidth,
    /* [in] */ Int32 frameShadowColor,
    /* [in] */ Float shadowRadius,
    /* [in] */ Int32 highlightColor)
{
    Drawable::constructor();
    mSize = size;
    mShadowRadius = shadowRadius;
    mFrameColor = frameColor;
    mFrameShadowColor = frameShadowColor;
    mStrokeWidth = strokeWidth;
    mHighlightColor = highlightColor;

    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    helper->CreateBitmap(mSize, mSize, BitmapConfig_ARGB_8888, (IBitmap**)&mBitmap);
    AutoPtr<ICanvas> canvas;
    CCanvas::New(mBitmap, (ICanvas**)&canvas);

    Int32 width;
    bitmap->GetWidth(&width);
    Int32 height;
    bitmap->GetHeight(&height);
    Int32 square = Elastos::Core::Math::Min(width, height);

    AutoPtr<IRect> cropRect;
    CRect::New((width - square) / 2, (height - square) / 2, square, square, (IRect**)&cropRect);
    AutoPtr<IRectF> circleRect;
    CRectF::New(0.0f, 0.0f, mSize, mSize, (IRectF**)&circleRect);
    circleRect->Inset(mStrokeWidth / 2.0f, mStrokeWidth / 2.0f);
    circleRect->Inset(mShadowRadius, mShadowRadius);

    AutoPtr<IPath> fillPath;
    CPath::New((IPath**)&fillPath);
    fillPath->AddArc(circleRect, 0.0f, 360.0f);

    canvas->DrawColor(0, PorterDuffMode_CLEAR);

    // opaque circle matte
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetColor(IColor::BLACK);
    mPaint->SetStyle(PaintStyle_FILL);
    canvas->DrawPath(fillPath, mPaint);

    // mask in the icon where the bitmap is opaque
    AutoPtr<IPorterDuffXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_SRC_ATOP, (IPorterDuffXfermode**)&mode);
    mPaint->SetXfermode(IXfermode::Probe(mode));
    canvas->DrawBitmap(bitmap, cropRect, circleRect, mPaint);

    // prepare paint for frame drawing
    mPaint->SetXfermode(NULL);

    mScale = 1.0f;

    CRect::New(0, 0, mSize, mSize, (IRect**)&mSrcRect);
    CRectF::New(0, 0, mSize, mSize, (IRectF**)&mDstRect);
    CRectF::New(mDstRect, (IRectF**)&mFrameRect);
    CPath::New((IPath**)&mFramePath);
}

ECode KeyguardCircleFramedDrawable::Reset()
{
    mScale = 1.0f;
    mPressed = FALSE;
    return NOERROR;
}

ECode KeyguardCircleFramedDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    // clear background
    Int32 width;
    canvas->GetWidth(&width);
    Int32 height;
    canvas->GetHeight(&height);
    Float outside = Elastos::Core::Math::Min(width, height);
    Float inside = mScale * outside;
    Float pad = (outside - inside) / 2.0f;

    mDstRect->Set(pad, pad, outside - pad, outside - pad);
    canvas->DrawBitmap(mBitmap, mSrcRect, mDstRect, NULL);

    mFrameRect->Set(mDstRect);
    mFrameRect->Inset(mStrokeWidth / 2.0f, mStrokeWidth / 2.0f);
    mFrameRect->Inset(mShadowRadius, mShadowRadius);

    mFramePath->Reset();
    mFramePath->AddArc(mFrameRect, 0.0f, 360.0f);

    // white frame
    if (mPressed) {
        mPaint->SetStyle(PaintStyle_FILL);
        AutoPtr<IColor> helper;
        CColor::AcquireSingleton((IColor**)&helper);
        Int32 red;
        helper->Red(mHighlightColor, &red);
        Int32 green;
        helper->Green(mHighlightColor, &green);
        Int32 blue;
        helper->Blue(mHighlightColor, &blue);
        Int32 argb;
        helper->Argb((Int32) (0.33f * 255), red, green, blue, &argb);
        mPaint->SetColor(argb);
        canvas->DrawPath(mFramePath, mPaint);
    }
    mPaint->SetStrokeWidth(mStrokeWidth);
    mPaint->SetStyle(PaintStyle_STROKE);
    mPaint->SetColor(mPressed ? mHighlightColor : mFrameColor);
    mPaint->SetShadowLayer(mShadowRadius, 0.0f, 0.0f, mFrameShadowColor);
    return canvas->DrawPath(mFramePath, mPaint);
}

ECode KeyguardCircleFramedDrawable::SetScale(
    /* [in] */ Float scale)
{
    mScale = scale;
    return NOERROR;
}

ECode KeyguardCircleFramedDrawable::GetScale(
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale)

    *scale = mScale;
    return NOERROR;
}

ECode KeyguardCircleFramedDrawable::SetPressed(
    /* [in] */ Boolean pressed)
{
    mPressed = pressed;
    return NOERROR;
}

ECode KeyguardCircleFramedDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity)

    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode KeyguardCircleFramedDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return NOERROR;
}

ECode KeyguardCircleFramedDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

ECode KeyguardCircleFramedDrawable::VerifyParams(
    /* [in] */ Float iconSize,
    /* [in] */ Int32 frameColor,
    /* [in] */ Float stroke,
    /* [in] */ Int32 frameShadowColor,
    /* [in] */ Float shadowRadius,
    /* [in] */ Int32 highlightColor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSize == iconSize
            && mFrameColor == frameColor
            && mStrokeWidth == stroke
            && mFrameShadowColor == frameShadowColor
            && mShadowRadius == shadowRadius
            && mHighlightColor == highlightColor;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
