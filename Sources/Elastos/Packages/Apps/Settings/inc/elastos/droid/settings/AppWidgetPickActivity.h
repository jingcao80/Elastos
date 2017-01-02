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
 * Copyright (C) 2009 The Android Open Source Project
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
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::ActivityPicker::IPickAdapter;

using Elastos::Utility::IList;

/**
 * Displays a list of {@link AppWidgetProviderInfo} widgets, along with any
 * injected special widgets specified through
 * {@link AppWidgetManager#EXTRA_CUSTOM_INFO} and
 * {@link AppWidgetManager#EXTRA_CUSTOM_EXTRAS}.
 * <p>
 * When an installed {@link AppWidgetProviderInfo} is selected, this activity
 * will bind it to the given {@link AppWidgetManager#EXTRA_APPWIDGET_ID},
 * otherwise it will return the requested extras.
 */
public class AppWidgetPickActivity extends ActivityPicker
    implements AppWidgetLoader.ItemConstructor<PickAdapter.Item>{
    private static const String TAG = "AppWidgetPickActivity";
    static const Boolean LOGD = FALSE;

    List<PickAdapter.Item> mItems;
    
    /**
     * The allocated {@link AppWidgetManager#EXTRA_APPWIDGET_ID} that this
     * activity is binding.
     */
    private Int32 mAppWidgetId;
    private AppWidgetLoader<PickAdapter.Item> mAppWidgetLoader;
    private AppWidgetManager mAppWidgetManager;
    private PackageManager mPackageManager;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        mPackageManager = GetPackageManager();
        mAppWidgetManager = AppWidgetManager->GetInstance(this);
        mAppWidgetLoader = new AppWidgetLoader<PickAdapter.Item>
            (this, mAppWidgetManager, this);
        
        super->OnCreate(icicle);
        
        // Set default return data
        SetResultData(RESULT_CANCELED, NULL);
        
        // Read the appWidgetId passed our direction, otherwise bail if not found
        final Intent intent = GetIntent();
        if (intent->HasExtra(AppWidgetManager.EXTRA_APPWIDGET_ID)) {
            mAppWidgetId = intent->GetIntExtra(AppWidgetManager.EXTRA_APPWIDGET_ID,
                    AppWidgetManager.INVALID_APPWIDGET_ID);
        } else {
            Finish();
        }
    }

    /**
     * Build and return list of items to be shown in dialog. This will mix both
     * installed {@link AppWidgetProviderInfo} and those provided through
     * {@link AppWidgetManager#EXTRA_CUSTOM_INFO}, sorting them alphabetically.
     */
    //@Override
    protected List<PickAdapter.Item> GetItems() {
        mItems = mAppWidgetLoader->GetItems(GetIntent());
        return mItems;
    }

    //@Override
    public PickAdapter.Item CreateItem(Context context, AppWidgetProviderInfo info, Bundle extras) {
        CharSequence label = info.label;
        Drawable icon = NULL;

        if (info.icon != 0) {
            try {
                final Resources res = context->GetResources();
                final Int32 density = res->GetDisplayMetrics().densityDpi;
                Int32 iconDensity;
                switch (density) {
                    case DisplayMetrics.DENSITY_MEDIUM:
                        iconDensity = DisplayMetrics.DENSITY_LOW;
                    case DisplayMetrics.DENSITY_TV:
                        iconDensity = DisplayMetrics.DENSITY_MEDIUM;
                    case DisplayMetrics.DENSITY_HIGH:
                        iconDensity = DisplayMetrics.DENSITY_MEDIUM;
                    case DisplayMetrics.DENSITY_XHIGH:
                        iconDensity = DisplayMetrics.DENSITY_HIGH;
                    case DisplayMetrics.DENSITY_XXHIGH:
                        iconDensity = DisplayMetrics.DENSITY_XHIGH;
                    default:
                        // The density is some abnormal value.  Return some other
                        // abnormal value that is a reasonable scaling of it.
                        iconDensity = (Int32)((density*0.75f)+.5f);
                }
                Resources packageResources = mPackageManager.
                        GetResourcesForApplication(info.provider->GetPackageName());
                icon = packageResources->GetDrawableForDensity(info.icon, iconDensity);
            } catch (NameNotFoundException e) {
                Logger::W(TAG, "Can't load icon drawable 0x" + Integer->ToHexString(info.icon)
                        + " for provider: " + info.provider);
            }
            if (icon == NULL) {
                Logger::W(TAG, "Can't load icon drawable 0x" + Integer->ToHexString(info.icon)
                        + " for provider: " + info.provider);
            }
        }

        PickAdapter.Item item = new PickAdapter->Item(context, label, icon);
        item.packageName = info.provider->GetPackageName();
        item.className = info.provider->GetClassName();
        item.extras = extras;
        return item;
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        Intent intent = GetIntentForPosition(which);
        PickAdapter.Item item = mItems->Get(which);

        Int32 result;
        if (item.extras != NULL) {
            // If these extras are present it's because this entry is custom.
            // Don't try to bind it, just pass it back to the app.
            SetResultData(RESULT_OK, intent);
        } else {
            try {
                Bundle options = NULL;
                if (intent->GetExtras() != NULL) {
                    options = intent->GetExtras()->GetBundle(
                            AppWidgetManager.EXTRA_APPWIDGET_OPTIONS);
                }
                mAppWidgetManager->BindAppWidgetId(mAppWidgetId, intent->GetComponent(), options);
                result = RESULT_OK;
            } catch (IllegalArgumentException e) {
                // This is thrown if they're already bound, or otherwise somehow
                // bogus.  Set the result to canceled, and exit.  The app *should*
                // clean up at this point.  We could pass the error along, but
                // it's not clear that that's useful -- the widget will simply not
                // appear.
                result = RESULT_CANCELED;
            }
            SetResultData(result, NULL);
        }

        Finish();
    }


    /**
     * Convenience method for setting the result code and intent. This method
     * correctly injects the {@link AppWidgetManager#EXTRA_APPWIDGET_ID} that
     * most hosts expect returned.
     */
    void SetResultData(Int32 code, Intent intent) {
        Intent result = intent != NULL ? intent : new Intent();
        result->PutExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, mAppWidgetId);
        SetResult(code, result);
    }
}
