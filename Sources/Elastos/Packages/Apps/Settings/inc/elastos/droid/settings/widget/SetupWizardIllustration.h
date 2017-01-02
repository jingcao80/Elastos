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

package com.android.settings.widget;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewOutlineProvider;
using Elastos::Droid::Widget::IFrameLayout;

using Elastos::Droid::Settings::IR;

/**
 * Class to draw the illustration of setup wizard. The aspectRatio attribute determines the aspect
 * ratio of the top padding, which is leaving space for the illustration. Draws an illustration
 * (foreground) to fit the width of the view and fills the rest with the background.
 *
 * Copied from com.google.android.setupwizard.util.SetupWizardIllustration
 */
public class SetupWizardIllustration extends FrameLayout {

    private static const String TAG = "SetupWizardIllustration";

    // Size of the baseline grid in pixels
    private Float mBaselineGridSize;
    private Drawable mBackground;
    private Drawable mForeground;
    private final Rect mViewBounds = new Rect();
    private final Rect mForegroundBounds = new Rect();
    private Float mScale = 1.0f;
    private Float mAspectRatio = 0.0f;

    public SetupWizardIllustration(Context context) {
        This(context, NULL);
    }

    public SetupWizardIllustration(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public SetupWizardIllustration(Context context, AttributeSet attrs, Int32 defStyleAttr) {
        This(context, attrs, defStyleAttr, 0);
    }

    public SetupWizardIllustration(Context context, AttributeSet attrs, Int32 defStyleAttr,
            Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);
        if (attrs != NULL) {
            TypedArray a = context->ObtainStyledAttributes(attrs,
                    R.styleable.SetupWizardIllustration, 0, 0);
            mAspectRatio = a->GetFloat(R.styleable.SetupWizardIllustration_aspectRatio, 0.0f);
            a->Recycle();
        }
        // Number of pixels of the 8dp baseline grid as defined in material design specs
        mBaselineGridSize = GetResources()->GetDisplayMetrics().density * 8;
        SetWillNotDraw(FALSE);
    }

    /**
     * The background will be drawn to fill up the rest of the view. It will also be scaled by the
     * same amount as the foreground so their textures look the same.
     */
    //@Override
    CARAPI SetBackground(Drawable background) {
        mBackground = background;
    }

    /**
     * Sets the drawable used as the illustration. THe drawable is expected to have intrinsic
     * width and height defined and will be scaled to fit the width of the view.
     */
    //@Override
    CARAPI SetForeground(Drawable foreground) {
        mForeground = foreground;
    }

    //@Override
    CARAPI OnResolveDrawables(Int32 layoutDirection) {
        mBackground->SetLayoutDirection(layoutDirection);
        mForeground->SetLayoutDirection(layoutDirection);
    }

    //@Override
    protected CARAPI OnMeasure(Int32 widthMeasureSpec, Int32 heightMeasureSpec) {
        if (mAspectRatio != 0.0f) {
            Int32 parentWidth = MeasureSpec->GetSize(widthMeasureSpec);
            Int32 illustrationHeight = (Int32) (parentWidth / mAspectRatio);
            illustrationHeight -= illustrationHeight % mBaselineGridSize;
            SetPaddingRelative(0, illustrationHeight, 0, 0);
        }
        SetOutlineProvider(ViewOutlineProvider.BOUNDS);
        super->OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    //@Override
    protected void OnLayout(Boolean changed, Int32 left, Int32 top, Int32 right, Int32 bottom) {
        final Int32 layoutWidth = right - left;
        final Int32 layoutHeight = bottom - top;
        if (mForeground != NULL) {
            Int32 intrinsicWidth = mForeground->GetIntrinsicWidth();
            Int32 intrinsicHeight = mForeground->GetIntrinsicHeight();
            final Int32 layoutDirection = GetLayoutDirection();

            mViewBounds->Set(0, 0, layoutWidth, layoutHeight);
            if (mAspectRatio != 0f) {
                mScale = layoutWidth / (Float) intrinsicWidth;
                intrinsicWidth = layoutWidth;
                intrinsicHeight = (Int32) (intrinsicHeight * mScale);
            }
            Gravity->Apply(Gravity.FILL_HORIZONTAL | Gravity.TOP, intrinsicWidth, intrinsicHeight,
                    mViewBounds, mForegroundBounds, layoutDirection);
            mForeground->SetBounds(mForegroundBounds);
        }
        if (mBackground != NULL) {
            // Scale the bounds by mScale to compensate for the scale done to the canvas before
            // drawing.
            mBackground->SetBounds(0, 0, (Int32) Math->Ceil(layoutWidth / mScale),
                    (Int32) Math->Ceil((layoutHeight - mForegroundBounds->Height()) / mScale));
        }
        super->OnLayout(changed, left, top, right, bottom);
    }

    //@Override
    CARAPI_(void) OnDraw(Canvas canvas) {
        if (mBackground != NULL) {
            canvas->Save();
            // Draw the background filling parts not covered by the illustration
            canvas->Translate(0, mForegroundBounds->Height());
            // Scale the background so its size matches the foreground
            canvas->Scale(mScale, mScale, 0, 0);
            mBackground->Draw(canvas);
            canvas->Restore();
        }
        if (mForeground != NULL) {
            canvas->Save();
            // Draw the illustration
            mForeground->Draw(canvas);
            canvas->Restore();
        }
        super->OnDraw(canvas);
    }
}
