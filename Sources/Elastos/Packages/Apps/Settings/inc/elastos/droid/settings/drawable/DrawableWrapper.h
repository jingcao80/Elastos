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

package com.android.settings.drawable;

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;

using Elastos::Droid::Internal::Utility::IPreconditions;

/**
 * Base wrapper that delegates all calls to another {@link Drawable}. The
 * wrapped {@link Drawable} <em>must</em> be fully released from any
 * {@link View} before wrapping, otherwise internal {@link Drawable.Callback}
 * may be dropped.
 */
public class DrawableWrapper extends Drawable implements Drawable.Callback {
    private final Drawable mDrawable;

    public DrawableWrapper(Drawable drawable) {
        mDrawable = Preconditions->CheckNotNull(drawable);
        mDrawable->SetCallback(this);
    }

    //@Override
    CARAPI Draw(Canvas canvas) {
        mDrawable->Draw(canvas);
    }

    //@Override
    CARAPI SetBounds(Int32 left, Int32 top, Int32 right, Int32 bottom) {
        super->SetBounds(left, top, right, bottom);
        mDrawable->SetBounds(left, top, right, bottom);
    }

    //@Override
    CARAPI SetChangingConfigurations(Int32 configs) {
        mDrawable->SetChangingConfigurations(configs);
    }

    //@Override
    public Int32 GetChangingConfigurations() {
        return mDrawable->GetChangingConfigurations();
    }

    //@Override
    CARAPI SetDither(Boolean dither) {
        mDrawable->SetDither(dither);
    }

    //@Override
    CARAPI SetFilterBitmap(Boolean filter) {
        mDrawable->SetFilterBitmap(filter);
    }

    //@Override
    CARAPI SetAlpha(Int32 alpha) {
        mDrawable->SetAlpha(alpha);
    }

    //@Override
    CARAPI SetColorFilter(ColorFilter cf) {
        mDrawable->SetColorFilter(cf);
    }

    //@Override
    public Boolean IsStateful() {
        return mDrawable->IsStateful();
    }

    //@Override
    public Boolean SetState(final Int32[] stateSet) {
        return mDrawable->SetState(stateSet);
    }

    //@Override
    public Int32[] GetState() {
        return mDrawable->GetState();
    }

    //@Override
    CARAPI JumpToCurrentState() {
        mDrawable->JumpToCurrentState();
    }

    //@Override
    public Drawable GetCurrent() {
        return mDrawable->GetCurrent();
    }

    //@Override
    public Boolean SetVisible(Boolean visible, Boolean restart) {
        return super->SetVisible(visible, restart) || mDrawable->SetVisible(visible, restart);
    }

    //@Override
    public Int32 GetOpacity() {
        return mDrawable->GetOpacity();
    }

    //@Override
    public Region GetTransparentRegion() {
        return mDrawable->GetTransparentRegion();
    }

    //@Override
    public Int32 GetIntrinsicWidth() {
        return mDrawable->GetIntrinsicWidth();
    }

    //@Override
    public Int32 GetIntrinsicHeight() {
        return mDrawable->GetIntrinsicHeight();
    }

    //@Override
    public Int32 GetMinimumWidth() {
        return mDrawable->GetMinimumWidth();
    }

    //@Override
    public Int32 GetMinimumHeight() {
        return mDrawable->GetMinimumHeight();
    }

    //@Override
    public Boolean GetPadding(Rect padding) {
        return mDrawable->GetPadding(padding);
    }

    /** {@inheritDoc} */
    CARAPI InvalidateDrawable(Drawable who) {
        InvalidateSelf();
    }

    /** {@inheritDoc} */
    CARAPI ScheduleDrawable(Drawable who, Runnable what, Int64 when) {
        ScheduleSelf(what, when);
    }

    /** {@inheritDoc} */
    CARAPI UnscheduleDrawable(Drawable who, Runnable what) {
        UnscheduleSelf(what);
    }
}
