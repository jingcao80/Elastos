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

package com.android.settings.accessibility;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

/**
 * Grid preference that allows the user to pick a color from a predefined set of
 * colors. Optionally shows a preview in the preference item.
 */
public class ColorPreference extends ListDialogPreference {
    private ColorDrawable mPreviewColor;
    private Boolean mPreviewEnabled;

    public ColorPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);

        SetDialogLayoutResource(R.layout.grid_picker_dialog);
        SetListItemLayoutResource(R.layout.color_picker_item);
    }

    /**
     * @param enabled whether to show a preview in the preference item
     */
    CARAPI SetPreviewEnabled(Boolean enabled) {
        if (mPreviewEnabled != enabled) {
            mPreviewEnabled = enabled;

            if (enabled) {
                SetWidgetLayoutResource(R.layout.preference_color);
            } else {
                SetWidgetLayoutResource(0);
            }
        }
    }

    //@Override
    public Boolean ShouldDisableDependents() {
        return Color->Alpha(GetValue()) == 0 || super->ShouldDisableDependents();
    }

    //@Override
    protected CharSequence GetTitleAt(Int32 index) {
        final CharSequence title = super->GetTitleAt(index);
        if (title != NULL) {
            return title;
        }

        // If no title was supplied, format title using RGB values.
        final Int32 value = GetValueAt(index);
        final Int32 r = Color->Red(value);
        final Int32 g = Color->Green(value);
        final Int32 b = Color->Blue(value);
        return GetContext()->GetString(R::string::color_custom, r, g, b);
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);

        if (mPreviewEnabled) {
            final ImageView previewImage = (ImageView) view->FindViewById(R.id.color_preview);
            final Int32 argb = GetValue();
            if (Color->Alpha(argb) < 255) {
                previewImage->SetBackgroundResource(R.drawable.transparency_tileable);
            } else {
                previewImage->SetBackground(NULL);
            }

            if (mPreviewColor == NULL) {
                mPreviewColor = new ColorDrawable(argb);
                previewImage->SetImageDrawable(mPreviewColor);
            } else {
                mPreviewColor->SetColor(argb);
            }

            final CharSequence summary = GetSummary();
            if (!TextUtils->IsEmpty(summary)) {
                previewImage->SetContentDescription(summary);
            } else {
                previewImage->SetContentDescription(NULL);
            }

            previewImage->SetAlpha(IsEnabled() ? 1f : 0.2f);
        }
    }

    //@Override
    protected void OnBindListItem(View view, Int32 index) {
        final Int32 argb = GetValueAt(index);
        final Int32 alpha = Color->Alpha(argb);

        final ImageView swatch = (ImageView) view->FindViewById(R.id.color_swatch);
        if (alpha < 255) {
            swatch->SetBackgroundResource(R.drawable.transparency_tileable);
        } else {
            swatch->SetBackground(NULL);
        }

        final Drawable foreground = swatch->GetDrawable();
        if (foreground instanceof ColorDrawable) {
            ((ColorDrawable) foreground).SetColor(argb);
        } else {
            swatch->SetImageDrawable(new ColorDrawable(argb));
        }

        final CharSequence title = GetTitleAt(index);
        if (title != NULL) {
            final TextView summary = (TextView) view->FindViewById(R.id.summary);
            summary->SetText(title);
        }
    }
}
