/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.settings;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IEditTextPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IEditText;

public class SelectableEditTextPreference extends EditTextPreference {

    private Int32 mSelectionMode;

    public static const Int32 SELECTION_CURSOR_END   = 0;
    public static const Int32 SELECTION_CURSOR_START = 1;
    public static const Int32 SELECTION_SELECT_ALL   = 2;

    public SelectableEditTextPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    /**
     * Sets the selection mode for the text when it shows up in the dialog
     * @hide
     * @param selectionMode can be SELECTION_CURSOR_START, SELECTION_CURSOR_END or
     * SELECTION_SELECT_ALL. Default is SELECTION_CURSOR_END
     */
    CARAPI SetInitialSelectionMode(Int32 selectionMode) {
        mSelectionMode = selectionMode;
    }

    //@Override
    protected void OnBindDialogView(View view) {
        super->OnBindDialogView(view);

        EditText editText = GetEditText();
        // Set the selection based on the mSelectionMode
        Int32 length = editText->GetText() != NULL ? editText->GetText()->Length() : 0;
        if (!TextUtils->IsEmpty(editText->GetText())) {
            switch (mSelectionMode) {
            case SELECTION_CURSOR_END:
                editText->SetSelection(length);
                break;
            case SELECTION_CURSOR_START:
                editText->SetSelection(0);
                break;
            case SELECTION_SELECT_ALL:
                editText->SetSelection(0, length);
                break;
            }
        }
    }
}

