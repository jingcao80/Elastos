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

using static com::Android::Settings::DataUsageSummary::IformatDateRange;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Text::IStaticLayout;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IView;

using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Settings::IR;

using Elastos::Utility::ILocale;

/**
 * Background of {@link ChartView} that renders grid lines as requested by
 * {@link ChartAxis#GetTickPoints()}.
 */
public class ChartGridView extends View {

    private ChartAxis mHoriz;
    private ChartAxis mVert;

    private Drawable mPrimary;
    private Drawable mSecondary;
    private Drawable mBorder;

    private Int32 mLabelSize;
    private Int32 mLabelColor;

    private Layout mLabelStart;
    private Layout mLabelMid;
    private Layout mLabelEnd;

    public ChartGridView(Context context) {
        This(context, NULL, 0);
    }

    public ChartGridView(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public ChartGridView(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);

        SetWillNotDraw(FALSE);

        final TypedArray a = context->ObtainStyledAttributes(
                attrs, R.styleable.ChartGridView, defStyle, 0);

        mPrimary = a->GetDrawable(R.styleable.ChartGridView_primaryDrawable);
        mSecondary = a->GetDrawable(R.styleable.ChartGridView_secondaryDrawable);
        mBorder = a->GetDrawable(R.styleable.ChartGridView_borderDrawable);

        final Int32 taId = a->GetResourceId(R.styleable.ChartGridView_android_textAppearance, -1);
        final TypedArray ta = context->ObtainStyledAttributes(taId,
                R.styleable.TextAppearance);
        mLabelSize = ta->GetDimensionPixelSize(
                R.styleable.TextAppearance_textSize, 0);
        ta->Recycle();

        final ColorStateList labelColor = a->GetColorStateList(
                R.styleable.ChartGridView_android_textColor);
        mLabelColor = labelColor->GetDefaultColor();

        a->Recycle();
    }

    void Init(ChartAxis horiz, ChartAxis vert) {
        mHoriz = Preconditions->CheckNotNull(horiz, "missing horiz");
        mVert = Preconditions->CheckNotNull(vert, "missing vert");
    }

    void SetBounds(Int64 start, Int64 end) {
        final Context context = GetContext();
        final Int64 mid = (start + end) / 2;
        mLabelStart = MakeLabel(FormatDateRange(context, start, start));
        mLabelMid = MakeLabel(FormatDateRange(context, mid, mid));
        mLabelEnd = MakeLabel(FormatDateRange(context, end, end));
        Invalidate();
    }

    //@Override
    protected void OnDraw(Canvas canvas) {
        final Int32 width = GetWidth();
        final Int32 height = GetHeight() - GetPaddingBottom();

        final Drawable secondary = mSecondary;
        if (secondary != NULL) {
            final Int32 secondaryHeight = secondary->GetIntrinsicHeight();

            final Float[] vertTicks = mVert->GetTickPoints();
            for (Float y : vertTicks) {
                final Int32 bottom = (Int32) Math->Min(y + secondaryHeight, height);
                secondary->SetBounds(0, (Int32) y, width, bottom);
                secondary->Draw(canvas);
            }
        }

        final Drawable primary = mPrimary;
        if (primary != NULL) {
            final Int32 primaryWidth = primary->GetIntrinsicWidth();
            final Int32 primaryHeight = primary->GetIntrinsicHeight();

            final Float[] horizTicks = mHoriz->GetTickPoints();
            for (Float x : horizTicks) {
                final Int32 right = (Int32) Math->Min(x + primaryWidth, width);
                primary->SetBounds((Int32) x, 0, right, height);
                primary->Draw(canvas);
            }
        }

        mBorder->SetBounds(0, 0, width, height);
        mBorder->Draw(canvas);

        final Int32 padding = mLabelStart != NULL ? mLabelStart->GetHeight() / 8 : 0;

        final Layout start = mLabelStart;
        if (start != NULL) {
            final Int32 saveCount = canvas->Save();
            canvas->Translate(0, height + padding);
            start->Draw(canvas);
            canvas->RestoreToCount(saveCount);
        }

        final Layout mid = mLabelMid;
        if (mid != NULL) {
            final Int32 saveCount = canvas->Save();
            canvas->Translate((width - mid->GetWidth()) / 2, height + padding);
            mid->Draw(canvas);
            canvas->RestoreToCount(saveCount);
        }

        final Layout end = mLabelEnd;
        if (end != NULL) {
            final Int32 saveCount = canvas->Save();
            canvas->Translate(width - end->GetWidth(), height + padding);
            end->Draw(canvas);
            canvas->RestoreToCount(saveCount);
        }
    }

    private Layout MakeLabel(CharSequence text) {
        final Resources res = GetResources();
        final TextPaint paint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
        paint.density = res->GetDisplayMetrics().density;
        paint->SetCompatibilityScaling(res->GetCompatibilityInfo().applicationScale);
        paint->SetColor(mLabelColor);
        paint->SetTextSize(mLabelSize);

        return new StaticLayout(text, paint,
                (Int32) Math->Ceil(Layout->GetDesiredWidth(text, paint)),
                Layout.Alignment.ALIGN_NORMAL, 1.f, 0, TRUE);
    }
}
