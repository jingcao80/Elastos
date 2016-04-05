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
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::CaptioningManager::ICaptionStyle;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Widget::ISubtitleView;
using Elastos::Droid::Settings::IR;

/**
 * Grid preference that allows the user to pick a captioning edge type.
 */
public class EdgeTypePreference extends ListDialogPreference {
    private static const Int32 DEFAULT_FOREGROUND_COLOR = Color.WHITE;
    private static const Int32 DEFAULT_BACKGROUND_COLOR = Color.TRANSPARENT;
    private static const Int32 DEFAULT_EDGE_COLOR = Color.BLACK;
    private static const Float DEFAULT_FONT_SIZE = 32f;

    public EdgeTypePreference(Context context, AttributeSet attrs) {
        Super(context, attrs);

        final Resources res = context->GetResources();
        SetValues(res->GetIntArray(R.array.captioning_edge_type_selector_values));
        SetTitles(res->GetStringArray(R.array.captioning_edge_type_selector_titles));
        SetDialogLayoutResource(R.layout.grid_picker_dialog);
        SetListItemLayoutResource(R.layout.preset_picker_item);
    }

    //@Override
    public Boolean ShouldDisableDependents() {
        return GetValue() == CaptionStyle.EDGE_TYPE_NONE || super->ShouldDisableDependents();
    }

    //@Override
    protected void OnBindListItem(View view, Int32 index) {
        final SubtitleView preview = (SubtitleView) view->FindViewById(R.id.preview);

        preview->SetForegroundColor(DEFAULT_FOREGROUND_COLOR);
        preview->SetBackgroundColor(DEFAULT_BACKGROUND_COLOR);

        final Float density = GetContext()->GetResources().GetDisplayMetrics().density;
        preview->SetTextSize(DEFAULT_FONT_SIZE * density);

        final Int32 value = GetValueAt(index);
        preview->SetEdgeType(value);
        preview->SetEdgeColor(DEFAULT_EDGE_COLOR);

        final CharSequence title = GetTitleAt(index);
        if (title != NULL) {
            final TextView summary = (TextView) view->FindViewById(R.id.summary);
            summary->SetText(title);
        }
    }
}
