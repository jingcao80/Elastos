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

package com.android.settings.quicklaunch;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IListActivity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISimpleAdapter;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ITreeMap;

/**
 * Activity to pick a bookmark that will be returned to the caller.
 * <p>
 * Currently, bookmarks are either:
 * <li> Activities that are in the launcher
 * <li> Activities that are within an app that is capable of being launched with
 * the {@link IIntent::ACTION_CREATE_SHORTCUT}.
 */
public class BookmarkPicker extends ListActivity implements SimpleAdapter.ViewBinder {

    private static const String TAG = "BookmarkPicker";

    /** Extra in the returned intent from this activity. */
    public static const String EXTRA_TITLE = "com.android.settings.quicklaunch.TITLE";

    /** Extra that should be provided, and will be returned. */
    public static const String EXTRA_SHORTCUT = "com.android.settings.quicklaunch.SHORTCUT";

    /**
     * The request code for the screen to create a bookmark that is WITHIN an
     * application. For example, Gmail can return a bookmark for the inbox
     * folder.
     */
    private static const Int32 REQUEST_CREATE_SHORTCUT = 1;

    /** Intent used to get all the activities that are launch-able */
    private static Intent sLaunchIntent;
    /** Intent used to get all the activities that are {@link #REQUEST_CREATE_SHORTCUT}-able */
    private static Intent sShortcutIntent;

    /**
     * List of ResolveInfo for activities that we can bookmark (either directly
     * to the activity, or by launching the activity and it returning a bookmark
     * WITHIN that application).
     */
    private List<ResolveInfo> mResolveList;

    // List adapter stuff
    private static const String KEY_TITLE = "TITLE";
    private static const String KEY_RESOLVE_INFO = "RESOLVE_INFO";
    private static const String sKeys[] = new String[] { KEY_TITLE, KEY_RESOLVE_INFO };
    private static const Int32 sResourceIds[] = new Int32[] { R.id.title, R.id.icon };
    private SimpleAdapter mMyAdapter;

    /** Display those activities that are launch-able */
    private static const Int32 DISPLAY_MODE_LAUNCH = 0;
    /** Display those activities that are able to have bookmarks WITHIN the application */
    private static const Int32 DISPLAY_MODE_SHORTCUT = 1;
    private Int32 mDisplayMode = DISPLAY_MODE_LAUNCH;

    private Handler mUiHandler = new Handler();

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        UpdateListAndAdapter();
    }

    //@Override
    public Boolean OnCreateOptionsMenu(Menu menu) {
        menu->Add(0, DISPLAY_MODE_LAUNCH, 0, R::string::quick_launch_display_mode_applications)
                .SetIcon(R.drawable.ic_menu_archive);
        menu->Add(0, DISPLAY_MODE_SHORTCUT, 0, R::string::quick_launch_display_mode_shortcuts)
                .SetIcon(R.drawable.ic_menu_goto);
        return TRUE;
    }

    //@Override
    public Boolean OnPrepareOptionsMenu(Menu menu) {
        menu->FindItem(DISPLAY_MODE_LAUNCH).SetVisible(mDisplayMode != DISPLAY_MODE_LAUNCH);
        menu->FindItem(DISPLAY_MODE_SHORTCUT).SetVisible(mDisplayMode != DISPLAY_MODE_SHORTCUT);
        return TRUE;
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {

        switch (item->GetItemId()) {

            case DISPLAY_MODE_LAUNCH:
                mDisplayMode = DISPLAY_MODE_LAUNCH;
                break;

            case DISPLAY_MODE_SHORTCUT:
                mDisplayMode = DISPLAY_MODE_SHORTCUT;
                break;

            default:
                return FALSE;
        }

        UpdateListAndAdapter();
        return TRUE;
    }

    private void EnsureIntents() {
        if (sLaunchIntent == NULL) {
            sLaunchIntent = new Intent(IIntent::ACTION_MAIN).AddCategory(Intent.CATEGORY_LAUNCHER);
            sShortcutIntent = new Intent(IIntent::ACTION_CREATE_SHORTCUT);
        }
    }

    /**
     * This should be called from the UI thread.
     */
    private void UpdateListAndAdapter() {
        // Get the activities in a separate thread
        new Thread("data updater") {
            //@Override
            CARAPI Run() {
                {    AutoLock syncLock(BookmarkPicker.this);
                    /*
                     * Don't touch any of the lists that are being used by the
                     * adapter in this thread!
                     */
                    ArrayList<ResolveInfo> newResolveList = new ArrayList<ResolveInfo>();
                    ArrayList<Map<String, ?>> newAdapterList = new ArrayList<Map<String, ?>>();

                    FillResolveList(newResolveList);
                    Collections->Sort(newResolveList,
                            new ResolveInfo->DisplayNameComparator(GetPackageManager()));

                    FillAdapterList(newAdapterList, newResolveList);

                    UpdateAdapterToUseNewLists(newAdapterList, newResolveList);
                }
            }
        }.Start();
    }

    private void UpdateAdapterToUseNewLists(final ArrayList<Map<String, ?>> newAdapterList,
            final ArrayList<ResolveInfo> newResolveList) {
        // Post this back on the UI thread
        mUiHandler->Post(new Runnable() {
            CARAPI Run() {
                /*
                 * SimpleAdapter does not support changing the lists after it
                 * has been created. We just create a new instance.
                 */
                mMyAdapter = CreateResolveAdapter(newAdapterList);
                mResolveList = newResolveList;
                SetListAdapter(mMyAdapter);
            }
        });
    }

    /**
     * Gets all activities matching our current display mode.
     *
     * @param list The list to fill.
     */
    private void FillResolveList(List<ResolveInfo> list) {
        EnsureIntents();
        PackageManager pm = GetPackageManager();
        list->Clear();

        if (mDisplayMode == DISPLAY_MODE_LAUNCH) {
            list->AddAll(pm->QueryIntentActivities(sLaunchIntent, 0));
        } else if (mDisplayMode == DISPLAY_MODE_SHORTCUT) {
            list->AddAll(pm->QueryIntentActivities(sShortcutIntent, 0));
        }
    }

    private SimpleAdapter CreateResolveAdapter(List<Map<String, ?>> list) {
        SimpleAdapter adapter = new SimpleAdapter(this, list,
                R.layout.bookmark_picker_item, sKeys, sResourceIds);
        adapter->SetViewBinder(this);
        return adapter;
    }

    private void FillAdapterList(List<Map<String, ?>> list,
            List<ResolveInfo> resolveList) {
        list->Clear();
        Int32 resolveListSize = resolveList->Size();
        for (Int32 i = 0; i < resolveListSize; i++) {
            ResolveInfo info = resolveList->Get(i);
            /*
             * Simple adapter craziness. For each item, we need to create a map
             * from a key to its value (the value can be any object--the view
             * binder will take care of filling the View with a representation
             * of that object).
             */
            Map<String, Object> map = new TreeMap<String, Object>();
            map->Put(KEY_TITLE, GetResolveInfoTitle(info));
            map->Put(KEY_RESOLVE_INFO, info);
            list->Add(map);
        }
    }

    /** Get the title for a resolve info. */
    private String GetResolveInfoTitle(ResolveInfo info) {
        CharSequence label = info->LoadLabel(GetPackageManager());
        if (label == NULL) label = info.activityInfo.name;
        return label != NULL ? label->ToString() : NULL;
    }

    //@Override
    protected void OnListItemClick(ListView l, View v, Int32 position, Int64 id) {
        if (position >= mResolveList->Size()) return;

        ResolveInfo info = mResolveList->Get(position);

        switch (mDisplayMode) {

            case DISPLAY_MODE_LAUNCH:
                // We can go ahead and return the clicked info's intent
                Intent intent = GetIntentForResolveInfo(info, IIntent::ACTION_MAIN);
                intent->AddCategory(Intent.CATEGORY_LAUNCHER);
                Finish(intent, GetResolveInfoTitle(info));
                break;

            case DISPLAY_MODE_SHORTCUT:
                // Start the shortcut activity so the user can pick the actual intent
                // (example: Gmail's shortcut activity shows a list of mailboxes)
                StartShortcutActivity(info);
                break;
        }

    }

    private static Intent GetIntentForResolveInfo(ResolveInfo info, String action) {
        Intent intent = new Intent(action);
        ActivityInfo ai = info.activityInfo;
        intent->SetClassName(ai.packageName, ai.name);
        return intent;
    }

    /**
     * Starts an activity to get a shortcut.
     * <p>
     * For example, Gmail has an activity that lists the available labels. It
     * returns a shortcut intent for going directly to this label.
     */
    private void StartShortcutActivity(ResolveInfo info) {
        Intent intent = GetIntentForResolveInfo(info, IIntent::ACTION_CREATE_SHORTCUT);
        StartActivityForResult(intent, REQUEST_CREATE_SHORTCUT);

        // Will get a callback to onActivityResult
    }

    //@Override
    protected void OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (resultCode != RESULT_OK) {
            return;
        }

        switch (requestCode) {

            case REQUEST_CREATE_SHORTCUT:
                if (data != NULL) {
                    Finish((Intent) data->GetParcelableExtra(Intent.EXTRA_SHORTCUT_INTENT),
                            data->GetStringExtra(Intent.EXTRA_SHORTCUT_NAME));
                }
                break;

            default:
                super->OnActivityResult(requestCode, resultCode, data);
                break;
        }
    }

    /**
     * Finishes the activity and returns the given data.
     */
    private void Finish(Intent intent, String title) {
        // Give back what was given to us (it will have the shortcut, for example)
        intent->PutExtras(GetIntent());
        // Put our information
        intent->PutExtra(EXTRA_TITLE, title);
        SetResult(RESULT_OK, intent);
        Finish();
    }

    /**
     * {@inheritDoc}
     */
    public Boolean SetViewValue(View view, Object data, String textRepresentation) {
        if (view->GetId() == R.id.icon) {
            Drawable icon = ((ResolveInfo) data).LoadIcon(GetPackageManager());
            if (icon != NULL) {
                ((ImageView) view).SetImageDrawable(icon);
            }
            return TRUE;
        } else {
            return FALSE;
        }
    }

}
