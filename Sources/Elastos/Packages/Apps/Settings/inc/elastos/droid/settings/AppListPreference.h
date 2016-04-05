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
using Elastos::Droid::App::AlertDialog::IBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ICheckedTextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListAdapter;

/**
 * Extends ListPreference to allow us to show the icons for a given list of applications. We do this
 * because the names of applications are very similar and the user may not be able to determine what
 * app they are selecting without an icon.
 */
public class AppListPreference extends ListPreference {
    private Drawable[] mEntryDrawables;

    public class AppArrayAdapter extends ArrayAdapter<CharSequence> {
        private Drawable[] mImageDrawables = NULL;
        private Int32 mSelectedIndex = 0;

        public AppArrayAdapter(Context context, Int32 textViewResourceId,
                CharSequence[] objects, Drawable[] imageDrawables, Int32 selectedIndex) {
            Super(context, textViewResourceId, objects);
            mSelectedIndex = selectedIndex;
            mImageDrawables = imageDrawables;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            LayoutInflater inflater = ((Activity)GetContext()).GetLayoutInflater();
            View view = inflater->Inflate(R.layout.app_preference_item, parent, FALSE);
            CheckedTextView checkedTextView = (CheckedTextView)view->FindViewById(R.id.app_label);
            checkedTextView->SetText(GetItem(position));
            if (position == mSelectedIndex) {
                checkedTextView->SetChecked(TRUE);
            }
            ImageView imageView = (ImageView)view->FindViewById(R.id.app_image);
            imageView->SetImageDrawable(mImageDrawables[position]);
            return view;
        }
    }

    public AppListPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    CARAPI SetPackageNames(String[] packageNames, String defaultPackageName) {
        // Look up all package names in PackageManager. Skip ones we can't find.
        Int32 foundPackages = 0;
        PackageManager pm = GetContext()->GetPackageManager();
        ApplicationInfo[] appInfos = new ApplicationInfo[packageNames.length];
        for (Int32 i = 0; i < packageNames.length; i++) {
            try {
                appInfos[i] = pm->GetApplicationInfo(packageNames[i], 0);
                foundPackages++;
            } catch (NameNotFoundException e) {
                // Leave appInfos[i] uninitialized; it will be skipped in the list.
            }
        }

        // Show the label and icon for each application package.
        CharSequence[] applicationNames = new CharSequence[foundPackages];
        mEntryDrawables = new Drawable[foundPackages];
        Int32 index = 0;
        Int32 selectedIndex = -1;
        for (ApplicationInfo appInfo : appInfos) {
            if (appInfo != NULL) {
                applicationNames[index] = appInfo->LoadLabel(pm);
                mEntryDrawables[index] = appInfo->LoadIcon(pm);
                if (defaultPackageName != NULL &&
                        appInfo.packageName->ContentEquals(defaultPackageName)) {
                    selectedIndex = index;
                }
                index++;
            }
        }
        SetEntries(applicationNames);
        SetEntryValues(packageNames);
        if (selectedIndex != -1) {
            SetValueIndex(selectedIndex);
        }
    }

    //@Override
    protected void OnPrepareDialogBuilder(Builder builder) {
        Int32 selectedIndex = FindIndexOfValue(GetValue());
        ListAdapter adapter = new AppArrayAdapter(GetContext(),
            R.layout.app_preference_item, GetEntries(), mEntryDrawables, selectedIndex);
        builder->SetAdapter(adapter, this);
        super->OnPrepareDialogBuilder(builder);
    }
}
