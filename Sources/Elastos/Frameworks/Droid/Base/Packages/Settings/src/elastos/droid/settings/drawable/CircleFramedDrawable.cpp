
#include "elastos/droid/settings/drawable/CircleFramedDrawable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IPorterDuff;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;

using Elastos::Droid::Settings::IR;

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
{
    Drawable::Drawable();
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
    CRectF::New(0f, 0f, mSize, mSize, (IRectF**)&circleRect);
    circleRect->Inset(mStrokeWidth / 2f, mStrokeWidth / 2f);
    circleRect->Inset(mShadowRadius, mShadowRadius);

    AutoPtr<IPath> fillPath;
    CPath::New((IPath**)&fillPath);
    fillPath->AddArc(circleRect, 0f, 360f);

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

    mScale = 1f;

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
    Float pad = (mSize - inside) / 2f;

    mDstRect->Set(pad, pad, mSize - pad, mSize - pad);
    canvas->DrawBitmap(mBitmap, mSrcRect, mDstRect, NULL);

    mFrameRect->Set(mDstRect);
    mFrameRect->Inset(mStrokeWidth / 2f, mStrokeWidth / 2f);
    mFrameRect->Inset(mShadowRadius, mShadowRadius);

    mFramePath->Reset();
    mFramePath->AddArc(mFrameRect, 0f, 360f);

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
    mPaint->SetShadowLayer(mShadowRadius, 0f, 0f, mFrameShadowColor);
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
