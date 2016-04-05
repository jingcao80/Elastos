/*
 * Copyright (C) 2007 The Android Open Source Project
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
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IRingtonePreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;

public class DefaultRingtonePreference extends RingtonePreference {
    private static const String TAG = "DefaultRingtonePreference";
    
    public DefaultRingtonePreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    //@Override
    protected void OnPrepareRingtonePickerIntent(Intent ringtonePickerIntent) {
        super->OnPrepareRingtonePickerIntent(ringtonePickerIntent);
        
        /*
         * Since this preference is for choosing the default ringtone, it
         * doesn't make sense to show a 'Default' item.
         */
        ringtonePickerIntent->PutExtra(RingtoneManager.EXTRA_RINGTONE_SHOW_DEFAULT, FALSE);
    }

    //@Override
    protected void OnSaveRingtone(Uri ringtoneUri) {
        RingtoneManager->SetActualDefaultRingtoneUri(GetContext(), GetRingtoneType(), ringtoneUri);
    }

    //@Override
    protected Uri OnRestoreRingtone() {
        return RingtoneManager->GetActualDefaultRingtoneUri(GetContext(), GetRingtoneType());
    }
    
}
