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

package com.android.settings.bluetooth;

using Elastos::Droid::Settings::IProgressCategory;
using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;

/**
 * A Bluetooth discovery progress category
 */
public class BluetoothProgressCategory extends ProgressCategory {
    public BluetoothProgressCategory(Context context) {
        This(context, NULL);
    }

    public BluetoothProgressCategory(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public BluetoothProgressCategory(Context context, AttributeSet attrs, Int32 defStyleAttr) {
        This(context, attrs, defStyleAttr, 0);
    }

    public BluetoothProgressCategory(Context context, AttributeSet attrs, Int32 defStyleAttr,
            Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);

        SetEmptyTextRes(R::string::bluetooth_no_devices_found);
    }
}
