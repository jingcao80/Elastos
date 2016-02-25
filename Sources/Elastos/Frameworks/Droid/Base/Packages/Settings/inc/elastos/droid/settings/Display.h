/*
 * Copyright (C) 2006 The Android Open Source Project
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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;


public class Display extends Activity implements View.OnClickListener {
    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        SetContentView(R.layout.display);

        mFontSize = (Spinner) FindViewById(R.id.fontSize);
        mFontSize->SetOnItemSelectedListener(mFontSizeChanged);
        String[] states = new String[3];
        Resources r = GetResources();
        states[0] = r->GetString(R::string::small_font);
        states[1] = r->GetString(R::string::medium_font);
        states[2] = r->GetString(R::string::large_font);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, states);
        adapter->SetDropDownViewResource(
                android.R.layout.simple_spinner_dropdown_item);
        mFontSize->SetAdapter(adapter);

        mPreview = (TextView) FindViewById(R.id.preview);
        mPreview->SetText(r->GetText(R::string::font_size_preview_text));

        Button save = (Button) FindViewById(R.id.save);
        save->SetText(r->GetText(R::string::font_size_save));
        save->SetOnClickListener(this);

        mTextSizeTyped = new TypedValue();
        TypedArray styledAttributes = 
            ObtainStyledAttributes(android.R.styleable.TextView);
        styledAttributes->GetValue(android.R.styleable.TextView_textSize,
                mTextSizeTyped);

        DisplayMetrics metrics = GetResources()->GetDisplayMetrics();
        mDisplayMetrics = new DisplayMetrics();
        mDisplayMetrics.density = metrics.density;
        mDisplayMetrics.heightPixels = metrics.heightPixels;
        mDisplayMetrics.scaledDensity = metrics.scaledDensity;
        mDisplayMetrics.widthPixels = metrics.widthPixels;
        mDisplayMetrics.xdpi = metrics.xdpi;
        mDisplayMetrics.ydpi = metrics.ydpi;
        
        styledAttributes->Recycle();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        try {
            mCurConfig->UpdateFrom(
                ActivityManagerNative->GetDefault()->GetConfiguration());
        } catch (RemoteException e) {
        }
        if (mCurConfig.fontScale < 1) {
            mFontSize->SetSelection(0);
        } else if (mCurConfig.fontScale > 1) {
            mFontSize->SetSelection(2);
        } else {
            mFontSize->SetSelection(1);
        }
        UpdateFontScale();
    }

    private void UpdateFontScale() {
        mDisplayMetrics.scaledDensity = mDisplayMetrics.density *
                mCurConfig.fontScale;

        Float size = mTextSizeTyped->GetDimension(mDisplayMetrics);
        mPreview->SetTextSize(TypedValue.COMPLEX_UNIT_PX, size);
    }

    CARAPI OnClick(View v) {
        try {
            ActivityManagerNative->GetDefault()->UpdatePersistentConfiguration(mCurConfig);
        } catch (RemoteException e) {
        }
        Finish();
    }

    private Spinner.OnItemSelectedListener mFontSizeChanged
                                    = new Spinner->OnItemSelectedListener() {
        CARAPI OnItemSelected(android.widget.AdapterView av, View v,
                                    Int32 position, Int64 id) {
            if (position == 0) {
                mCurConfig.fontScale = .75f;
            } else if (position == 2) {
                mCurConfig.fontScale = 1.25f;
            } else {
                mCurConfig.fontScale = 1.0f;
            }

            UpdateFontScale();
        }

        CARAPI OnNothingSelected(android.widget.AdapterView av) {
        }
    };

    private Spinner mFontSize;
    private TextView mPreview;
    private TypedValue mTextSizeTyped;
    private DisplayMetrics mDisplayMetrics;
    private Configuration mCurConfig = new Configuration();
}
