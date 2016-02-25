/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.drawable;

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

using Elastos::Droid::Settings::IR;

/**
 * Converts the user avatar icon to a circularly clipped one.
 * TODO: Move this to an internal framework class and share with the one in Keyguard.
 */
public class CircleFramedDrawable extends Drawable {

    private final Bitmap mBitmap;
    private final Int32 mSize;
    private final Paint mPaint;
    private final Float mShadowRadius;
    private final Float mStrokeWidth;
    private final Int32 mFrameColor;
    private final Int32 mHighlightColor;
    private final Int32 mFrameShadowColor;

    private Float mScale;
    private Path mFramePath;
    private Rect mSrcRect;
    private RectF mDstRect;
    private RectF mFrameRect;
    private Boolean mPressed;

    public static CircleFramedDrawable GetInstance(Context context, Bitmap icon) {
        Resources res = context->GetResources();
        Float iconSize = res->GetDimension(R.dimen.circle_avatar_size);
        Float strokeWidth = res->GetDimension(R.dimen.circle_avatar_frame_stroke_width);
        Float shadowRadius = res->GetDimension(R.dimen.circle_avatar_frame_shadow_radius);
        Int32 frameColor = res->GetColor(R::Color::circle_avatar_frame_color);
        Int32 frameShadowColor = res->GetColor(R::Color::circle_avatar_frame_shadow_color);
        Int32 highlightColor = res->GetColor(R::Color::circle_avatar_frame_pressed_color);

        CircleFramedDrawable instance = new CircleFramedDrawable(icon,
                (Int32) iconSize, frameColor, strokeWidth, frameShadowColor, shadowRadius,
                highlightColor);
        return instance;
    }

    public CircleFramedDrawable(Bitmap icon, Int32 size,
            Int32 frameColor, Float strokeWidth,
            Int32 frameShadowColor, Float shadowRadius,
            Int32 highlightColor) {
        Super();
        mSize = size;
        mShadowRadius = shadowRadius;
        mFrameColor = frameColor;
        mFrameShadowColor = frameShadowColor;
        mStrokeWidth = strokeWidth;
        mHighlightColor = highlightColor;

        mBitmap = Bitmap->CreateBitmap(mSize, mSize, Bitmap.Config.ARGB_8888);
        final Canvas canvas = new Canvas(mBitmap);

        final Int32 width = icon->GetWidth();
        final Int32 height = icon->GetHeight();
        final Int32 square = Math->Min(width, height);

        final Rect cropRect = new Rect((width - square) / 2, (height - square) / 2, square, square);
        final RectF circleRect = new RectF(0f, 0f, mSize, mSize);
        circleRect->Inset(mStrokeWidth / 2f, mStrokeWidth / 2f);
        circleRect->Inset(mShadowRadius, mShadowRadius);

        final Path fillPath = new Path();
        fillPath->AddArc(circleRect, 0f, 360f);

        canvas->DrawColor(0, PorterDuff.Mode.CLEAR);

        // opaque circle matte
        mPaint = new Paint();
        mPaint->SetAntiAlias(TRUE);
        mPaint->SetColor(Color.BLACK);
        mPaint->SetStyle(Paint.Style.FILL);
        canvas->DrawPath(fillPath, mPaint);

        // mask in the icon where the bitmap is opaque
        mPaint->SetXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC_IN));
        canvas->DrawBitmap(icon, cropRect, circleRect, mPaint);

        // prepare paint for frame drawing
        mPaint->SetXfermode(NULL);

        mScale = 1f;

        mSrcRect = new Rect(0, 0, mSize, mSize);
        mDstRect = new RectF(0, 0, mSize, mSize);
        mFrameRect = new RectF(mDstRect);
        mFramePath = new Path();
    }

    //@Override
    CARAPI Draw(Canvas canvas) {
        final Float inside = mScale * mSize;
        final Float pad = (mSize - inside) / 2f;

        mDstRect->Set(pad, pad, mSize - pad, mSize - pad);
        canvas->DrawBitmap(mBitmap, mSrcRect, mDstRect, NULL);

        mFrameRect->Set(mDstRect);
        mFrameRect->Inset(mStrokeWidth / 2f, mStrokeWidth / 2f);
        mFrameRect->Inset(mShadowRadius, mShadowRadius);

        mFramePath->Reset();
        mFramePath->AddArc(mFrameRect, 0f, 360f);

        // white frame
        if (mPressed) {
            mPaint->SetStyle(Paint.Style.FILL);
            mPaint->SetColor(Color->Argb((Int32) (0.33f * 255),
                            Color->Red(mHighlightColor),
                            Color->Green(mHighlightColor),
                            Color->Blue(mHighlightColor)));
            canvas->DrawPath(mFramePath, mPaint);
        }
        mPaint->SetStrokeWidth(mStrokeWidth);
        mPaint->SetStyle(Paint.Style.STROKE);
        mPaint->SetColor(mPressed ? mHighlightColor : mFrameColor);
        mPaint->SetShadowLayer(mShadowRadius, 0f, 0f, mFrameShadowColor);
        canvas->DrawPath(mFramePath, mPaint);
    }

    CARAPI SetScale(Float scale) {
        mScale = scale;
    }

    public Float GetScale() {
        return mScale;
    }

    CARAPI SetPressed(Boolean pressed) {
        mPressed = pressed;
    }

    //@Override
    public Int32 GetOpacity() {
        return PixelFormat.TRANSLUCENT;
    }

    //@Override
    CARAPI SetAlpha(Int32 alpha) {
    }

    //@Override
    CARAPI SetColorFilter(ColorFilter cf) {
    }
    
    //@Override
    public Int32 GetIntrinsicWidth() {
        return mSize;
    }

    //@Override
    public Int32 GetIntrinsicHeight() {
        return mSize;
    }
}
