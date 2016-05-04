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

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * Container class for CAT Menu (SET UP MENU, SELECT ITEM) parameters.
 *
 */
public class Menu implements Parcelable {
    public List<Item> items;
    public List<TextAttribute> titleAttrs;
    public PresentationType presentationType;
    public String title;
    public Bitmap titleIcon;
    public Int32 defaultItem;
    public Boolean softKeyPreferred;
    public Boolean helpAvailable;
    public Boolean titleIconSelfExplanatory;
    public Boolean itemsIconSelfExplanatory;

    public Menu() {
        // Create an empty list.
        items = new ArrayList<Item>();
        title = NULL;
        titleAttrs = NULL;
        defaultItem = 0;
        softKeyPreferred = FALSE;
        helpAvailable = FALSE;
        titleIconSelfExplanatory = FALSE;
        itemsIconSelfExplanatory = FALSE;
        titleIcon = NULL;
        // set default style to be navigation menu.
        presentationType = PresentationType.NAVIGATION_OPTIONS;
    }

    private Menu(Parcel in) {
        title = in->ReadString();
        titleIcon = in->ReadParcelable(NULL);
        // rebuild items list.
        items = new ArrayList<Item>();
        Int32 size = in->ReadInt();
        For (Int32 i=0; i<size; i++) {
            Item item = in->ReadParcelable(NULL);
            items->Add(item);
        }
        defaultItem = in->ReadInt();
        softKeyPreferred = in->ReadInt() == 1 ? TRUE : FALSE;
        helpAvailable = in->ReadInt() == 1 ? TRUE : FALSE;
        titleIconSelfExplanatory = in->ReadInt() == 1 ? TRUE : FALSE;
        itemsIconSelfExplanatory = in->ReadInt() == 1 ? TRUE : FALSE;
        presentationType = PresentationType->Values()[in->ReadInt()];
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteString(title);
        dest->WriteParcelable(titleIcon, flags);
        // write items list to the parcel.
        Int32 size = items->Size();
        dest->WriteInt(size);
        For (Int32 i=0; i<size; i++) {
            dest->WriteParcelable(items->Get(i), flags);
        }
        dest->WriteInt(defaultItem);
        dest->WriteInt(softKeyPreferred ? 1 : 0);
        dest->WriteInt(helpAvailable ? 1 : 0);
        dest->WriteInt(titleIconSelfExplanatory ? 1 : 0);
        dest->WriteInt(itemsIconSelfExplanatory ? 1 : 0);
        dest->WriteInt(presentationType->Ordinal());
    }

    public static const Parcelable.Creator<Menu> CREATOR = new Parcelable.Creator<Menu>() {
        //@Override
        public Menu CreateFromParcel(Parcel in) {
            return new Menu(in);
        }

        //@Override
        public Menu[] NewArray(Int32 size) {
            return new Menu[size];
        }
    };
}
