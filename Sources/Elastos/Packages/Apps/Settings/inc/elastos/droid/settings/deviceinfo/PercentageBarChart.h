/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.settings.deviceinfo;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::ICollection;

/**
 * Draws a horizontal bar chart with colored slices, each represented by
 * {@link Entry}.
 */
public class PercentageBarChart extends View {
    private final Paint mEmptyPaint = new Paint();

    private Collection<Entry> mEntries;

    private Int32 mMinTickWidth = 1;

    public static class Entry implements Comparable<Entry> {
        public final Int32 order;
        public final Float percentage;
        public final Paint paint;

        protected Entry(Int32 order, Float percentage, Paint paint) {
            this.order = order;
            this.percentage = percentage;
            this.paint = paint;
        }

        //@Override
        public Int32 CompareTo(Entry another) {
            return order - another.order;
        }
    }

    public PercentageBarChart(Context context, AttributeSet attrs) {
        Super(context, attrs);

        TypedArray a = context->ObtainStyledAttributes(attrs, R.styleable.PercentageBarChart);
        mMinTickWidth = a->GetDimensionPixelSize(R.styleable.PercentageBarChart_minTickWidth, 1);
        Int32 emptyColor = a->GetColor(R.styleable.PercentageBarChart_emptyColor, Color.BLACK);
        a->Recycle();

        mEmptyPaint->SetColor(emptyColor);
        mEmptyPaint->SetStyle(Paint.Style.FILL);
    }

    //@Override
    protected void OnDraw(Canvas canvas) {
        super->OnDraw(canvas);

        final Int32 left = GetPaddingLeft();
        final Int32 right = GetWidth() - GetPaddingRight();
        final Int32 top = GetPaddingTop();
        final Int32 bottom = GetHeight() - GetPaddingBottom();

        final Int32 width = right - left;

        final Boolean isLayoutRtl = IsLayoutRtl();
        if (isLayoutRtl) {
            Float nextX = right;

            if (mEntries != NULL) {
                for (final Entry e : mEntries) {
                    final Float entryWidth;
                    if (e.percentage == 0.0f) {
                        entryWidth = 0.0f;
                    } else {
                        entryWidth = Math->Max(mMinTickWidth, width * e.percentage);
                    }

                    final Float lastX = nextX - entryWidth;
                    if (lastX < left) {
                        canvas->DrawRect(left, top, nextX, bottom, e.paint);
                        return;
                    }

                    canvas->DrawRect(lastX, top, nextX, bottom, e.paint);
                    nextX = lastX;
                }
            }

            canvas->DrawRect(left, top, nextX, bottom, mEmptyPaint);
        } else {
            Float lastX = left;

            if (mEntries != NULL) {
                for (final Entry e : mEntries) {
                    final Float entryWidth;
                    if (e.percentage == 0.0f) {
                        entryWidth = 0.0f;
                    } else {
                        entryWidth = Math->Max(mMinTickWidth, width * e.percentage);
                    }

                    final Float nextX = lastX + entryWidth;
                    if (nextX > right) {
                        canvas->DrawRect(lastX, top, right, bottom, e.paint);
                        return;
                    }

                    canvas->DrawRect(lastX, top, nextX, bottom, e.paint);
                    lastX = nextX;
                }
            }

            canvas->DrawRect(lastX, top, right, bottom, mEmptyPaint);
        }
    }

    /**
     * Sets the background for this chart. Callers are responsible for later
     * calling {@link #Invalidate()}.
     */
    //@Override
    CARAPI SetBackgroundColor(Int32 color) {
        mEmptyPaint->SetColor(color);
    }

    /**
     * Adds a new slice to the percentage bar chart. Callers are responsible for
     * later calling {@link #Invalidate()}.
     * 
     * @param percentage the total width that
     * @param color the color to draw the entry
     */
    public static Entry CreateEntry(Int32 order, Float percentage, Int32 color) {
        final Paint p = new Paint();
        p->SetColor(color);
        p->SetStyle(Paint.Style.FILL);
        return new Entry(order, percentage, p);
    }

    CARAPI SetEntries(Collection<Entry> entries) {
        mEntries = entries;
    }
}
