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

package com.android.settings.location;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

/**
 * Check box preference with check box replaced by radio button.
 *
 * Functionally speaking, it's actually a CheckBoxPreference. We only modified
 * the widget to RadioButton to make it "look like" a RadioButtonPreference.
 *
 * In other words, there's no "RadioButtonPreferenceGroup" in this
 * implementation. When you check one RadioButtonPreference, if you want to
 * uncheck all the other preferences, you should do that by code yourself.
 */
public class RadioButtonPreference extends CheckBoxPreference {
    public interface OnClickListener {
        public abstract void OnRadioButtonClicked(RadioButtonPreference emiter);
    }

    private OnClickListener mListener = NULL;

    public RadioButtonPreference(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);
        SetWidgetLayoutResource(R.layout.preference_widget_radiobutton);
    }

    public RadioButtonPreference(Context context, AttributeSet attrs) {
        This(context, attrs, R.attr.checkBoxPreferenceStyle);
    }

    public RadioButtonPreference(Context context) {
        This(context, NULL);
    }

    void SetOnClickListener(OnClickListener listener) {
        mListener = listener;
    }

    //@Override
    CARAPI OnClick() {
        if (mListener != NULL) {
            mListener->OnRadioButtonClicked(this);
        }
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);

        TextView title = (TextView) view->FindViewById(android.R.id.title);
        if (title != NULL) {
            title->SetSingleLine(FALSE);
            title->SetMaxLines(3);
        }
    }
}
