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
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::ICaptioningManager;
using Elastos::Droid::View::Accessibility::CaptioningManager::ICaptionStyle;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Widget::ISubtitleView;
using Elastos::Droid::Settings::IR;

public class PresetPreference extends ListDialogPreference {
    private static const Float DEFAULT_FONT_SIZE = 32f;

    private final CaptioningManager mCaptioningManager;

    public PresetPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);

        SetDialogLayoutResource(R.layout.grid_picker_dialog);
        SetListItemLayoutResource(R.layout.preset_picker_item);

        mCaptioningManager = (CaptioningManager) context->GetSystemService(
                Context.CAPTIONING_SERVICE);
    }

    //@Override
    public Boolean ShouldDisableDependents() {
        return GetValue() != CaptionStyle.PRESET_CUSTOM
                || super->ShouldDisableDependents();
    }

    //@Override
    protected void OnBindListItem(View view, Int32 index) {
        final View previewViewport = view->FindViewById(R.id.preview_viewport);
        final SubtitleView previewText = (SubtitleView) view->FindViewById(R.id.preview);
        final Int32 value = GetValueAt(index);
        CaptionPropertiesFragment->ApplyCaptionProperties(
                mCaptioningManager, previewText, previewViewport, value);

        final Float density = GetContext()->GetResources().GetDisplayMetrics().density;
        previewText->SetTextSize(DEFAULT_FONT_SIZE * density);

        final CharSequence title = GetTitleAt(index);
        if (title != NULL) {
            final TextView summary = (TextView) view->FindViewById(R.id.summary);
            summary->SetText(title);
        }
    }
}
