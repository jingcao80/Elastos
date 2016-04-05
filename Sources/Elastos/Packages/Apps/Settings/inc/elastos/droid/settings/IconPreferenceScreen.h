/*
 * Copyright (C) 2009 The Android Open Source Project
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
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

public class IconPreferenceScreen extends Preference {

    private Drawable mIcon;

    // Whether or not the text and icon should be highlighted (as selected)
    private Boolean mHighlight;

    public IconPreferenceScreen(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public IconPreferenceScreen(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);
        SetLayoutResource(R.layout.preference_icon);
        TypedArray a = context->ObtainStyledAttributes(attrs,
                R.styleable.IconPreferenceScreen, defStyle, 0);
        mIcon = a->GetDrawable(R.styleable.IconPreferenceScreen_icon);
    }

    //@Override
    CARAPI OnBindView(View view) {
        super->OnBindView(view);
        ImageView imageView = (ImageView) view->FindViewById(R.id.icon);
        if (imageView != NULL && mIcon != NULL) {
            imageView->SetImageDrawable(mIcon);
        }
        TextView textView = (TextView) view->FindViewById(android.R.id.title);
    }

    /**
     * Sets the icon for this Preference with a Drawable.
     *
     * @param icon The icon for this Preference
     */
    CARAPI SetIcon(Drawable icon) {
        if ((icon == NULL && mIcon != NULL) || (icon != NULL && !icon->Equals(mIcon))) {
            mIcon = icon;
            NotifyChanged();
        }
    }

    /**
     * Returns the icon of this Preference.
     *
     * @return The icon.
     * @see #SetIcon(Drawable)
     */
    public Drawable GetIcon() {
        return mIcon;
    }

    CARAPI SetHighlighted(Boolean highlight) {
        mHighlight = highlight;
        NotifyChanged();
    }
}
