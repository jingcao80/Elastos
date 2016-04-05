/*
 * Copyright (C) 2010 The Android Open Source Project
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

using Elastos::Droid::App::ILauncherActivity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IListView;

using Elastos::Droid::Settings::Settings::ITetherSettingsActivity;

using Elastos::Utility::IList;

public class CreateShortcut extends LauncherActivity {

    //@Override
    protected Intent GetTargetIntent() {
        Intent targetIntent = new Intent(IIntent::ACTION_MAIN, NULL);
        targetIntent->AddCategory("com.android.settings.SHORTCUT");
        targetIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        return targetIntent;
    }

    //@Override
    protected void OnListItemClick(ListView l, View v, Int32 position, Int64 id) {
        Intent shortcutIntent = IntentForPosition(position);
        shortcutIntent->SetFlags(IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
        Intent intent = new Intent();
        intent->PutExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE,
                Intent.ShortcutIconResource->FromContext(this, R.mipmap.ic_launcher_settings));
        intent->PutExtra(Intent.EXTRA_SHORTCUT_INTENT, shortcutIntent);
        intent->PutExtra(Intent.EXTRA_SHORTCUT_NAME, ItemForPosition(position).label);
        SetResult(RESULT_OK, intent);
        Finish();
    }

    //@Override
    protected Boolean OnEvaluateShowIcons() {
        return FALSE;
    }

    /**
     * Perform query on package manager for list items.  The default
     * implementation queries for activities.
     */
    protected List<ResolveInfo> OnQueryPackageManager(Intent queryIntent) {
        List<ResolveInfo> activities = super->OnQueryPackageManager(queryIntent);
        if (activities == NULL) return NULL;
        for (Int32 i = activities->Size() - 1; i >= 0; i--) {
            ResolveInfo info = activities->Get(i);
            if (info.activityInfo.name->EndsWith(TetherSettingsActivity.class->GetSimpleName())) {
                if (!TetherSettings->ShowInShortcuts(this)) {
                    activities->Remove(i);
                }
            }
        }
        return activities;
    }
}
