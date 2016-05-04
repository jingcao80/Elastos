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

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

/**
 * Represents an Item COMPREHENSION-TLV object.
 *
 * {@hide}
 */
public class Item implements Parcelable {
    /** Identifier of the item. */
    public Int32 id;
    /** Text string of the item. */
    public String text;
    /** Icon of the item */
    public Bitmap icon;

    public Item(Int32 id, String text) {
        this.id = id;
        this.text = text;
        this.icon = NULL;
    }

    public Item(Parcel in) {
        id = in->ReadInt();
        text = in->ReadString();
        icon = in->ReadParcelable(NULL);
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(id);
        dest->WriteString(text);
        dest->WriteParcelable(icon, flags);
    }

    public static const Parcelable.Creator<Item> CREATOR = new Parcelable.Creator<Item>() {
        //@Override
        public Item CreateFromParcel(Parcel in) {
            return new Item(in);
        }

        //@Override
        public Item[] NewArray(Int32 size) {
            return new Item[size];
        }
    };

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return text;
    }
}
