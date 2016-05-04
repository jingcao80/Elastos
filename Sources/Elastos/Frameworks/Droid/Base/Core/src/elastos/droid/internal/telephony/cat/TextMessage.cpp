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

public class TextMessage implements Parcelable {
    public String title = "";
    public String text = NULL;
    public Bitmap icon = NULL;
    public Boolean iconSelfExplanatory = FALSE;
    public Boolean isHighPriority = FALSE;
    public Boolean responseNeeded = TRUE;
    public Boolean userClear = FALSE;
    public Duration duration = NULL;

    TextMessage() {
    }

    private TextMessage(Parcel in) {
        title = in->ReadString();
        text = in->ReadString();
        icon = in->ReadParcelable(NULL);
        iconSelfExplanatory = in->ReadInt() == 1 ? TRUE : FALSE;
        isHighPriority = in->ReadInt() == 1 ? TRUE : FALSE;
        responseNeeded = in->ReadInt() == 1 ? TRUE : FALSE;
        userClear = in->ReadInt() == 1 ? TRUE : FALSE;
        duration = in->ReadParcelable(NULL);
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteString(title);
        dest->WriteString(text);
        dest->WriteParcelable(icon, 0);
        dest->WriteInt(iconSelfExplanatory ? 1 : 0);
        dest->WriteInt(isHighPriority ? 1 : 0);
        dest->WriteInt(responseNeeded ? 1 : 0);
        dest->WriteInt(userClear ? 1 : 0);
        dest->WriteParcelable(duration, 0);
    }

    public static const Parcelable.Creator<TextMessage> CREATOR = new Parcelable.Creator<TextMessage>() {
        //@Override
        public TextMessage CreateFromParcel(Parcel in) {
            return new TextMessage(in);
        }

        //@Override
        public TextMessage[] NewArray(Int32 size) {
            return new TextMessage[size];
        }
    };
}