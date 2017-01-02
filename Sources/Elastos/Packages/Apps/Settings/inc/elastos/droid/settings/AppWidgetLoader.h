//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

package com.android.settings;

using Elastos::Droid::Appwidget::IAppWidgetManager;
using Elastos::Droid::Appwidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Utility::ILog;

using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

public class AppWidgetLoader<Item extends AppWidgetLoader.LabelledItem> {
    private static const String TAG = "AppWidgetAdapter";
    private static const Boolean LOGD = AppWidgetPickActivity.LOGD;

    private Context mContext;
    private AppWidgetManager mAppWidgetManager;
    ItemConstructor<Item> mItemConstructor;

    interface LabelledItem {
        CharSequence GetLabel();
    }

    public AppWidgetLoader(Context context, AppWidgetManager appWidgetManager,
            ItemConstructor<Item> itemConstructor) {
        mContext = context;
        mAppWidgetManager = appWidgetManager;
        mItemConstructor = itemConstructor;
    }

    /**
     * Create list entries for any custom widgets requested through
     * {@link AppWidgetManager#EXTRA_CUSTOM_INFO}.
     */
    void PutCustomAppWidgets(List<Item> items, Intent intent) {
        // get and validate the extras they gave us
        ArrayList<AppWidgetProviderInfo> customInfo = NULL;
        ArrayList<Bundle> customExtras = NULL;
        try_custom_items: {
            customInfo = intent->GetParcelableArrayListExtra(AppWidgetManager.EXTRA_CUSTOM_INFO);
            if (customInfo == NULL || customInfo->Size() == 0) {
                Logger::I(TAG, "EXTRA_CUSTOM_INFO not present.");
                break try_custom_items;
            }

            Int32 customInfoSize = customInfo->Size();
            for (Int32 i=0; i<customInfoSize; i++) {
                Parcelable p = customInfo->Get(i);
                if (p == NULL || !(p instanceof AppWidgetProviderInfo)) {
                    customInfo = NULL;
                    Logger::E(TAG, "error using EXTRA_CUSTOM_INFO index=" + i);
                    break try_custom_items;
                }
            }

            customExtras = intent->GetParcelableArrayListExtra(AppWidgetManager.EXTRA_CUSTOM_EXTRAS);
            if (customExtras == NULL) {
                customInfo = NULL;
                Logger::E(TAG, "EXTRA_CUSTOM_INFO without EXTRA_CUSTOM_EXTRAS");
                break try_custom_items;
            }

            Int32 customExtrasSize = customExtras->Size();
            if (customInfoSize != customExtrasSize) {
                customInfo = NULL;
                customExtras = NULL;
                Logger::E(TAG, "list size mismatch: EXTRA_CUSTOM_INFO: " + customInfoSize
                        + " EXTRA_CUSTOM_EXTRAS: " + customExtrasSize);
                break try_custom_items;
            }


            for (Int32 i=0; i<customExtrasSize; i++) {
                Parcelable p = customExtras->Get(i);
                if (p == NULL || !(p instanceof Bundle)) {
                    customInfo = NULL;
                    customExtras = NULL;
                    Logger::E(TAG, "error using EXTRA_CUSTOM_EXTRAS index=" + i);
                    break try_custom_items;
                }
            }
        }

        if (LOGD) Logger::D(TAG, "Using " + customInfo->Size() + " custom items");
        PutAppWidgetItems(customInfo, customExtras, items, 0, TRUE);
    }


    /**
     * Create list entries for the given {@link AppWidgetProviderInfo} widgets,
     * inserting extras if provided.
     */
    void PutAppWidgetItems(List<AppWidgetProviderInfo> appWidgets,
            List<Bundle> customExtras, List<Item> items, Int32 categoryFilter,
            Boolean ignoreFilter) {
        if (appWidgets == NULL) return;
        final Int32 size = appWidgets->Size();
        for (Int32 i = 0; i < size; i++) {
            AppWidgetProviderInfo info = appWidgets->Get(i);

            // We remove any widgets whose category isn't included in the filter
            if (!ignoreFilter && (info.widgetCategory & categoryFilter) == 0) {
                continue;
            }

            Item item = mItemConstructor->CreateItem(mContext, info,
                    customExtras != NULL ? customExtras->Get(i) : NULL);

            items->Add(item);
        }
    }

    public interface ItemConstructor<Item> {
        Item CreateItem(Context context, AppWidgetProviderInfo info, Bundle extras);
    }


    /**
     * Build and return list of items to be shown in dialog. This will mix both
     * installed {@link AppWidgetProviderInfo} and those provided through
     * {@link AppWidgetManager#EXTRA_CUSTOM_INFO}, sorting them alphabetically.
     */
    protected List<Item> GetItems(Intent intent) {
        Boolean sortCustomAppWidgets =
                intent->GetBooleanExtra(AppWidgetManager.EXTRA_CUSTOM_SORT, TRUE);

        List<Item> items = new ArrayList<Item>();

        // Default category is home screen
        Int32 categoryFilter = intent->GetIntExtra(AppWidgetManager.EXTRA_CATEGORY_FILTER,
                AppWidgetProviderInfo.WIDGET_CATEGORY_HOME_SCREEN);

        PutInstalledAppWidgets(items, categoryFilter);

        // Sort all items together by label
        if (sortCustomAppWidgets) {
            PutCustomAppWidgets(items, intent);
        }
        Collections->Sort(items, new Comparator<Item>() {
            Collator mCollator = Collator->GetInstance();

            public Int32 Compare(Item lhs, Item rhs) {
                return mCollator->Compare(lhs->GetLabel(), rhs->GetLabel());
            }
        });
        if (!sortCustomAppWidgets) {
            List<Item> customItems = new ArrayList<Item>();
            PutCustomAppWidgets(customItems, intent);
            items->AddAll(customItems);
        }
        return items;
    }

    /**
     * Create list entries for installed {@link AppWidgetProviderInfo} widgets.
     */
    void PutInstalledAppWidgets(List<Item> items, Int32 categoryFilter) {
        List<AppWidgetProviderInfo> installed =
                mAppWidgetManager->GetInstalledProviders(categoryFilter);
        PutAppWidgetItems(installed, NULL, items, categoryFilter, FALSE);
    }
}
