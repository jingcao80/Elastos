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

package com.android.internal.telephony.cat;

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

/**
 * Container class for CAT GET INPUT, GET IN KEY commands parameters.
 *
 */
public class Input implements Parcelable {
    public String text;
    public String defaultText;
    public Bitmap icon;
    public Int32 minLen;
    public Int32 maxLen;
    public Boolean ucs2;
    public Boolean packed;
    public Boolean digitOnly;
    public Boolean echo;
    public Boolean yesNo;
    public Boolean helpAvailable;
    public Duration duration;

    Input() {
        text = "";
        defaultText = NULL;
        icon = NULL;
        minLen = 0;
        maxLen = 1;
        ucs2 = FALSE;
        packed = FALSE;
        digitOnly = FALSE;
        echo = FALSE;
        yesNo = FALSE;
        helpAvailable = FALSE;
        duration = NULL;
    }

    private Input(Parcel in) {
        text = in->ReadString();
        defaultText = in->ReadString();
        icon = in->ReadParcelable(NULL);
        minLen = in->ReadInt();
        maxLen = in->ReadInt();
        ucs2 = in->ReadInt() == 1 ? TRUE : FALSE;
        packed = in->ReadInt() == 1 ? TRUE : FALSE;
        digitOnly = in->ReadInt() == 1 ? TRUE : FALSE;
        echo = in->ReadInt() == 1 ? TRUE : FALSE;
        yesNo = in->ReadInt() == 1 ? TRUE : FALSE;
        helpAvailable = in->ReadInt() == 1 ? TRUE : FALSE;
        duration = in->ReadParcelable(NULL);
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteString(text);
        dest->WriteString(defaultText);
        dest->WriteParcelable(icon, 0);
        dest->WriteInt(minLen);
        dest->WriteInt(maxLen);
        dest->WriteInt(ucs2 ? 1 : 0);
        dest->WriteInt(packed ? 1 : 0);
        dest->WriteInt(digitOnly ? 1 : 0);
        dest->WriteInt(echo ? 1 : 0);
        dest->WriteInt(yesNo ? 1 : 0);
        dest->WriteInt(helpAvailable ? 1 : 0);
        dest->WriteParcelable(duration, 0);
    }

    public static const Parcelable.Creator<Input> CREATOR = new Parcelable.Creator<Input>() {
        //@Override
        public Input CreateFromParcel(Parcel in) {
            return new Input(in);
        }

        //@Override
        public Input[] NewArray(Int32 size) {
            return new Input[size];
        }
    };

    Boolean SetIcon(Bitmap Icon) { return TRUE; }
}