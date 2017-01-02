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

#include <Elastos.Droid.Content.h>
#include "elastos/droid/settings/drawable/CircleFramedDrawable.h"
#include "../R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_IN;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Drawable {

CircleFramedDrawable::CircleFramedDrawable(
    /* [in] */ IBitmap* icon,
    /* [in] */ Int32 size,
    /* [in] */ Int32 frameColor,
    /* [in] */ Float strokeWidth,
    /* [in] */ Int32 frameShadowColor,
    /* [in] */ Float shadowRadius,
    /* [in] */ Int32 highlightColor)
    : mSize(0)
    , mShadowRadius(0.f)
    , mStrokeWidth(0.f)
    , mFrameColor(0)
    , mHighlightColor(0)
    , mFrameShadowColor(0)
    , mScale(0.f)
    , mPressed(FALSE)
{
    assert(0 && "TODO");
    // Drawable::Drawable();
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
    icon->GetWidth(&width);
    Int32 height;
    icon->GetHeight(&height);
    Int32 square = Elastos::Core::Math::Min(width, height);

    AutoPtr<IRect> cropRect;
    CRect::New((width - square) / 2, (height - square) / 2, square, square,
            (IRect**)&cropRect);
    AutoPtr<IRectF> circleRect;
    CRectF::New(0.f, 0.f, mSize, mSize, (IRectF**)&circleRect);
    circleRect->Inset(mStrokeWidth / 2.f, mStrokeWidth / 2.f);
    circleRect->Inset(mShadowRadius, mShadowRadius);

    AutoPtr<IPath> fillPath;
    CPath::New((IPath**)&fillPath);
    fillPath->AddArc(circleRect, 0.f, 360.f);

    canvas->DrawColor(0, PorterDuffMode_CLEAR);

    // opaque circle matte
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetColor(IColor::BLACK);
    mPaint->SetStyle(PaintStyle_FILL);
    canvas->DrawPath(fillPath, mPaint);

    // mask in the icon where the bitmap is opaque
    AutoPtr<IXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_SRC_IN, (IXfermode**)&mode);
    mPaint->SetXfermode(mode);
    canvas->DrawBitmap(icon, cropRect, circleRect, mPaint);

    // prepare paint for frame drawing
    mPaint->SetXfermode(NULL);

    mScale = 1.f;

    CRect::New(0, 0, mSize, mSize, (IRect**)&mSrcRect);
    CRectF::New(0, 0, mSize, mSize, (IRectF**)&mDstRect);
    CRectF::New(mDstRect, (IRectF**)&mFrameRect);
    CPath::New((IPath**)&mFramePath);
}

CircleFramedDrawable::~CircleFramedDrawable()
{}

AutoPtr<CircleFramedDrawable> CircleFramedDrawable::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ IBitmap* icon)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Float iconSize;
    res->GetDimension(R::dimen::circle_avatar_size, &iconSize);
    Float strokeWidth;
    res->GetDimension(R::dimen::circle_avatar_frame_stroke_width, &strokeWidth);
    Float shadowRadius;
    res->GetDimension(R::dimen::circle_avatar_frame_shadow_radius, &shadowRadius);
    Int32 frameColor;
    res->GetColor(R::color::circle_avatar_frame_color, &frameColor);
    Int32 frameShadowColor;
    res->GetColor(R::color::circle_avatar_frame_shadow_color, &frameShadowColor);
    Int32 highlightColor;
    res->GetColor(R::color::circle_avatar_frame_pressed_color, &highlightColor);

    AutoPtr<CircleFramedDrawable> instance = new CircleFramedDrawable(icon,
            (Int32) iconSize, frameColor, strokeWidth, frameShadowColor,
            shadowRadius, highlightColor);
    return instance;
}

ECode CircleFramedDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Float inside = mScale * mSize;
    Float pad = (mSize - inside) / 2.f;

    mDstRect->Set(pad, pad, mSize - pad, mSize - pad);
    canvas->DrawBitmap(mBitmap, mSrcRect, mDstRect, NULL);

    mFrameRect->Set(mDstRect);
    mFrameRect->Inset(mStrokeWidth / 2.f, mStrokeWidth / 2.f);
    mFrameRect->Inset(mShadowRadius, mShadowRadius);

    mFramePath->Reset();
    mFramePath->AddArc(mFrameRect, 0.f, 360.f);

    // white frame
    if (mPressed) {
        mPaint->SetStyle(PaintStyle_FILL);
        AutoPtr<IColor> color;
        CColor::AcquireSingleton((IColor**)&color);
        Int32 red, green, blue;
        color->Red(mHighlightColor, &red);
        color->Green(mHighlightColor, &green);
        color->Blue(mHighlightColor, &blue);
        Int32 argb;
        color->Argb((Int32) (0.33f * 255), red, green, blue, &argb);
        mPaint->SetColor(argb);
        canvas->DrawPath(mFramePath, mPaint);
    }
    mPaint->SetStrokeWidth(mStrokeWidth);
    mPaint->SetStyle(PaintStyle_STROKE);
    mPaint->SetColor(mPressed ? mHighlightColor : mFrameColor);
    mPaint->SetShadowLayer(mShadowRadius, 0.f, 0.f, mFrameShadowColor);
    canvas->DrawPath(mFramePath, mPaint);
    return NOERROR;
}

ECode CircleFramedDrawable::SetScale(
    /* [in] */ Float scale)
{
    mScale = scale;
    return NOERROR;
}

Float CircleFramedDrawable::GetScale()
{
    return mScale;
}

ECode CircleFramedDrawable::SetPressed(
    /* [in] */ Boolean pressed)
{
    mPressed = pressed;
    return NOERROR;
}

ECode CircleFramedDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode CircleFramedDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return NOERROR;
}

ECode CircleFramedDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

ECode CircleFramedDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mSize;
    return NOERROR;
}

ECode CircleFramedDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mSize;
    return NOERROR;
}

} // namespace Drawable
} // namespace Settings
} // namespace Droid
} // namespace Elastos
