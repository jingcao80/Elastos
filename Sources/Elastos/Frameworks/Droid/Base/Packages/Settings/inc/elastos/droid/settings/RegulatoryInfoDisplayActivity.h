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

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

/**
 * {@link Activity} that displays regulatory information for the "Regulatory information"
 * preference item, and when "*#07#" is dialed on the Phone keypad. To enable this feature,
 * set the "config_show_regulatory_info" Boolean to TRUE in a device overlay resource, and in the
 * same overlay, either add a drawable named "regulatory_info.png" containing a graphical version
 * of the required regulatory info (If ro.bootloader.hardware.sku property is set use
 * "regulatory_info_<sku>.png where sku is ro.bootloader.hardware.sku property value in lowercase"),
 * or add a string resource named "regulatory_info_text" with an HTML version of the required
 * information (text will be centered in the dialog).
 */
public class RegulatoryInfoDisplayActivity extends Activity implements
        DialogInterface.OnDismissListener {
    private final String REGULATORY_INFO_RESOURCE = "regulatory_info";

    /**
     * Display the regulatory info graphic in a dialog window.
     */
    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        Resources resources = GetResources();

        if (!resources->GetBoolean(R.bool.config_show_regulatory_info)) {
            Finish();   // no regulatory info to display for this device
        }

        AlertDialog.Builder builder = new AlertDialog->Builder(this)
                .SetTitle(R::string::regulatory_information)
                .SetOnDismissListener(this);

        Boolean regulatoryInfoDrawableExists = FALSE;
        Int32 resId = GetResourceId();
        if (resId != 0) {
            try {
                Drawable d = resources->GetDrawable(resId);
                // set to FALSE if the width or height is <= 2
                // (missing PNG can return an empty 2x2 pixel Drawable)
                regulatoryInfoDrawableExists = (d->GetIntrinsicWidth() > 2
                        && d->GetIntrinsicHeight() > 2);
            } catch (Resources.NotFoundException ignored) {
                regulatoryInfoDrawableExists = FALSE;
            }
        }

        CharSequence regulatoryText = resources->GetText(R::string::regulatory_info_text);

        if (regulatoryInfoDrawableExists) {
            View view = GetLayoutInflater()->Inflate(R.layout.regulatory_info, NULL);
            ImageView image = (ImageView) view->FindViewById(R.id.regulatoryInfo);
            image->SetImageResource(resId);
            builder->SetView(view);
            builder->Show();
        } else if (regulatoryText->Length() > 0) {
            builder->SetMessage(regulatoryText);
            AlertDialog dialog = builder->Show();
            // we have to show the dialog first, or the SetGravity() call will throw a NPE
            TextView messageText = (TextView) dialog->FindViewById(android.R.id.message);
            messageText->SetGravity(Gravity.CENTER);
        } else {
            // neither drawable nor text resource exists, finish activity
            Finish();
        }
    }

    private Int32 GetResourceId() {
        // Use regulatory_info by default.
        Int32 resId = GetResources()->GetIdentifier(
                REGULATORY_INFO_RESOURCE, "drawable", GetPackageName());

        // When hardware sku property exists, use regulatory_info_<sku> resource if valid.
        String sku = SystemProperties->Get("ro.boot.hardware.sku", "");
        if (!TextUtils->IsEmpty(sku)) {
            String regulatory_info_res = REGULATORY_INFO_RESOURCE + "_" + sku->ToLowerCase();
            Int32 id = GetResources()->GetIdentifier(
                    regulatory_info_res, "drawable", GetPackageName());
            if (id != 0) {
                resId = id;
            }
        }
        return resId;
    }

    //@Override
    CARAPI OnDismiss(DialogInterface dialog) {
        Finish();   // close the activity
    }
}
