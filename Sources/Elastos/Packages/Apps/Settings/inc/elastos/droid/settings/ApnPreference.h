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

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::IRelativeLayout;

public class ApnPreference extends Preference implements
        CompoundButton.OnCheckedChangeListener, OnClickListener {
    static const String TAG = "ApnPreference";

    public ApnPreference(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);
    }

    public ApnPreference(Context context, AttributeSet attrs) {
        This(context, attrs, R.attr.apnPreferenceStyle);
    }

    public ApnPreference(Context context) {
        This(context, NULL);
    }

    private static String mSelectedKey = NULL;
    private static CompoundButton mCurrentChecked = NULL;
    private Boolean mProtectFromCheckedChange = FALSE;
    private Boolean mSelectable = TRUE;

    //@Override
    public View GetView(View convertView, ViewGroup parent) {
        View view = super->GetView(convertView, parent);

        View widget = view->FindViewById(R.id.apn_radiobutton);
        if ((widget != NULL) && widget instanceof RadioButton) {
            RadioButton rb = (RadioButton) widget;
            if (mSelectable) {
                rb->SetOnCheckedChangeListener(this);

                Boolean isChecked = GetKey()->Equals(mSelectedKey);
                if (isChecked) {
                    mCurrentChecked = rb;
                    mSelectedKey = GetKey();
                }

                mProtectFromCheckedChange = TRUE;
                rb->SetChecked(isChecked);
                mProtectFromCheckedChange = FALSE;
                rb->SetVisibility(View.VISIBLE);
            } else {
                rb->SetVisibility(View.GONE);
            }
        }

        View textLayout = view->FindViewById(R.id.text_layout);
        if ((textLayout != NULL) && textLayout instanceof RelativeLayout) {
            textLayout->SetOnClickListener(this);
        }

        return view;
    }

    public Boolean IsChecked() {
        return GetKey()->Equals(mSelectedKey);
    }

    CARAPI SetChecked() {
        mSelectedKey = GetKey();
    }

    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
        Logger::I(TAG, "ID: " + GetKey() + " :" + isChecked);
        if (mProtectFromCheckedChange) {
            return;
        }

        if (isChecked) {
            if (mCurrentChecked != NULL) {
                mCurrentChecked->SetChecked(FALSE);
            }
            mCurrentChecked = buttonView;
            mSelectedKey = GetKey();
            CallChangeListener(mSelectedKey);
        } else {
            mCurrentChecked = NULL;
            mSelectedKey = NULL;
        }
    }

    CARAPI OnClick(android.view.View v) {
        if ((v != NULL) && (R.id.text_layout == v->GetId())) {
            Context context = GetContext();
            if (context != NULL) {
                Int32 pos = Integer->ParseInt(GetKey());
                Uri url = ContentUris->WithAppendedId(Telephony.Carriers.CONTENT_URI, pos);
                context->StartActivity(new Intent(IIntent::ACTION_EDIT, url));
            }
        }
    }

    CARAPI SetSelectable(Boolean selectable) {
        mSelectable = selectable;
    }

    public Boolean GetSelectable() {
        return mSelectable;
    }
}
