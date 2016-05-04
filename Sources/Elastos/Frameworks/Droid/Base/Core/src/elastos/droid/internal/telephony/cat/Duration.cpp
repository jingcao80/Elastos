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

package com.android.internal.telephony.cat;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;


/**
 * Class for representing "Duration" object for CAT.
 *
 * {@hide}
 */
public class Duration implements Parcelable {
    public Int32 timeInterval;
    public TimeUnit timeUnit;

    public enum TimeUnit {
        MINUTE(0x00),
        SECOND(0x01),
        TENTH_SECOND(0x02);

        private Int32 mValue;

        TimeUnit(Int32 value) {
            mValue = value;
        }

        public Int32 Value() {
            return mValue;
        }
    }

    /**
     * @param timeInterval Between 1 and 255 inclusive.
     */
    public Duration(Int32 timeInterval, TimeUnit timeUnit) {
        this.timeInterval = timeInterval;
        this.timeUnit = timeUnit;
    }

    private Duration(Parcel in) {
        timeInterval = in->ReadInt();
        timeUnit = TimeUnit->Values()[in->ReadInt()];
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(timeInterval);
        dest->WriteInt(timeUnit->Ordinal());
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    public static const Parcelable.Creator<Duration> CREATOR = new Parcelable.Creator<Duration>() {
        //@Override
        public Duration CreateFromParcel(Parcel in) {
            return new Duration(in);
        }

        //@Override
        public Duration[] NewArray(Int32 size) {
            return new Duration[size];
        }
    };
}
