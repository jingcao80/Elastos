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

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

/**
 * Container class for PlayTone commands parameters.
 *
 */
public class ToneSettings implements Parcelable {
    public Duration duration;
    public Tone tone;
    public Boolean vibrate;

    public ToneSettings(Duration duration, Tone tone, Boolean vibrate) {
        this.duration = duration;
        this.tone = tone;
        this.vibrate = vibrate;
    }

    private ToneSettings(Parcel in) {
        duration = in->ReadParcelable(NULL);
        tone = in->ReadParcelable(NULL);
        vibrate = in->ReadInt() == 1;
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteParcelable(duration, 0);
        dest->WriteParcelable(tone, 0);
        dest->WriteInt(vibrate ? 1 : 0);
    }

    public static const Parcelable.Creator<ToneSettings> CREATOR = new Parcelable.Creator<ToneSettings>() {
        //@Override
        public ToneSettings CreateFromParcel(Parcel in) {
            return new ToneSettings(in);
        }

        //@Override
        public ToneSettings[] NewArray(Int32 size) {
            return new ToneSettings[size];
        }
    };
}