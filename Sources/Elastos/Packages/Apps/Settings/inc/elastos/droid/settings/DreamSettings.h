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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::MenuItem::IOnMenuItemClickListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::View::IOnTouchListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::DreamBackend::IDreamInfo;
using Elastos::Droid::Settings::Widget::ISwitchBar;

using Elastos::Utility::IList;

public class DreamSettings extends SettingsPreferenceFragment implements
        SwitchBar.OnSwitchChangeListener {
    private static const String TAG = DreamSettings.class->GetSimpleName();
    static const Boolean DEBUG = FALSE;
    private static const Int32 DIALOG_WHEN_TO_DREAM = 1;
    private static const String PACKAGE_SCHEME = "package";

    private final PackageReceiver mPackageReceiver = new PackageReceiver();

    private Context mContext;
    private DreamBackend mBackend;
    private DreamInfoAdapter mAdapter;
    private SwitchBar mSwitchBar;
    private MenuItem[] mMenuItemsWhenEnabled;
    private Boolean mRefreshing;

    //@Override
    public Int32 GetHelpResource() {
        return R::string::help_url_dreams;
    }

    //@Override
    CARAPI OnAttach(Activity activity) {
        Logd("OnAttach(%s)", activity->GetClass()->GetSimpleName());
        super->OnAttach(activity);
        mContext = activity;
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        Logd("OnCreate(%s)", icicle);
        super->OnCreate(icicle);

        mBackend = new DreamBackend(GetActivity());

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        if (!mRefreshing) {
            mBackend->SetEnabled(isChecked);
            RefreshFromBackend();
        }
    }

    //@Override
    CARAPI OnStart() {
        Logd("OnStart()");
        super->OnStart();
    }

    //@Override
    CARAPI OnDestroyView() {
        Logd("OnDestroyView()");
        super->OnDestroyView();

        mSwitchBar->RemoveOnSwitchChangeListener(this);
        mSwitchBar->Hide();
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        Logd("OnActivityCreated(%s)", savedInstanceState);
        super->OnActivityCreated(savedInstanceState);

        ListView listView = GetListView();
        listView->SetItemsCanFocus(TRUE);

        TextView emptyView = (TextView) GetView()->FindViewById(android.R.id.empty);
        emptyView->SetText(R::string::screensaver_settings_disabled_prompt);
        listView->SetEmptyView(emptyView);

        mAdapter = new DreamInfoAdapter(mContext);
        listView->SetAdapter(mAdapter);

        final SettingsActivity sa = (SettingsActivity) GetActivity();
        mSwitchBar = sa->GetSwitchBar();
        mSwitchBar->AddOnSwitchChangeListener(this);
        mSwitchBar->Show();
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        Logd("OnCreateOptionsMenu()");

        Boolean isEnabled = mBackend->IsEnabled();

        // create "start" action
        MenuItem start = CreateMenuItem(menu, R::string::screensaver_settings_dream_start,
                MenuItem.SHOW_AS_ACTION_NEVER,
                isEnabled, new Runnable(){
                    //@Override
                    CARAPI Run() {
                        mBackend->StartDreaming();
                    }});

        // create "when to dream" overflow menu item
        MenuItem whenToDream = CreateMenuItem(menu,
                R::string::screensaver_settings_when_to_dream,
                MenuItem.SHOW_AS_ACTION_NEVER,
                isEnabled,
                new Runnable() {
                    //@Override
                    CARAPI Run() {
                        ShowDialog(DIALOG_WHEN_TO_DREAM);
                    }});

        // create "help" overflow menu item (make sure it appears last)
        super->OnCreateOptionsMenu(menu, inflater);

        mMenuItemsWhenEnabled = new MenuItem[] { start, whenToDream };
    }

    private MenuItem CreateMenuItem(Menu menu,
            Int32 titleRes, Int32 actionEnum, Boolean isEnabled, final Runnable onClick) {
        MenuItem item = menu->Add(titleRes);
        item->SetShowAsAction(actionEnum);
        item->SetEnabled(isEnabled);
        item->SetOnMenuItemClickListener(new OnMenuItemClickListener() {
            //@Override
            public Boolean OnMenuItemClick(MenuItem item) {
                onClick->Run();
                return TRUE;
            }
        });
        return item;
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        Logd("OnCreateDialog(%s)", dialogId);
        if (dialogId == DIALOG_WHEN_TO_DREAM)
            return CreateWhenToDreamDialog();
        return super->OnCreateDialog(dialogId);
    }

    private Dialog CreateWhenToDreamDialog() {
        final CharSequence[] items = {
                mContext->GetString(R::string::screensaver_settings_summary_dock),
                mContext->GetString(R::string::screensaver_settings_summary_sleep),
                mContext->GetString(R::string::screensaver_settings_summary_either_short)
        };

        Int32 initialSelection = mBackend->IsActivatedOnDock() && mBackend->IsActivatedOnSleep() ? 2
                : mBackend->IsActivatedOnDock() ? 0
                : mBackend->IsActivatedOnSleep() ? 1
                : -1;

        return new AlertDialog->Builder(mContext)
                .SetTitle(R::string::screensaver_settings_when_to_dream)
                .SetSingleChoiceItems(items, initialSelection, new DialogInterface->OnClickListener() {
                    CARAPI OnClick(DialogInterface dialog, Int32 item) {
                        mBackend->SetActivatedOnDock(item == 0 || item == 2);
                        mBackend->SetActivatedOnSleep(item == 1 || item == 2);
                        dialog->Dismiss();
                    }
                })
                .Create();
    }

    //@Override
    CARAPI OnPause() {
        Logd("OnPause()");
        super->OnPause();

        mContext->UnregisterReceiver(mPackageReceiver);
    }

    //@Override
    CARAPI OnResume() {
        Logd("OnResume()");
        super->OnResume();
        RefreshFromBackend();

        // listen for package changes
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
        filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
        filter->AddDataScheme(PACKAGE_SCHEME);
        mContext->RegisterReceiver(mPackageReceiver , filter);
    }

    public static Int32 GetSummaryResource(Context context) {
        DreamBackend backend = new DreamBackend(context);
        Boolean isEnabled = backend->IsEnabled();
        Boolean activatedOnSleep = backend->IsActivatedOnSleep();
        Boolean activatedOnDock = backend->IsActivatedOnDock();
        Boolean activatedOnEither = activatedOnSleep && activatedOnDock;
        return !isEnabled ? R::string::screensaver_settings_summary_off
                : activatedOnEither ? R::string::screensaver_settings_summary_either_long
                : activatedOnSleep ? R::string::screensaver_settings_summary_sleep
                : activatedOnDock ? R::string::screensaver_settings_summary_dock
                : 0;
    }

    public static CharSequence GetSummaryTextWithDreamName(Context context) {
        DreamBackend backend = new DreamBackend(context);
        Boolean isEnabled = backend->IsEnabled();
        if (!isEnabled) {
            return context->GetString(R::string::screensaver_settings_summary_off);
        } else {
            return backend->GetActiveDreamName();
        }
    }

    private void RefreshFromBackend() {
        Logd("RefreshFromBackend()");
        mRefreshing = TRUE;
        Boolean dreamsEnabled = mBackend->IsEnabled();
        if (mSwitchBar->IsChecked() != dreamsEnabled)
            mSwitchBar->SetChecked(dreamsEnabled);

        mAdapter->Clear();
        if (dreamsEnabled) {
            List<DreamInfo> dreamInfos = mBackend->GetDreamInfos();
            mAdapter->AddAll(dreamInfos);
        }
        if (mMenuItemsWhenEnabled != NULL)
            for (MenuItem menuItem : mMenuItemsWhenEnabled)
                menuItem->SetEnabled(dreamsEnabled);
        mRefreshing = FALSE;
    }

    private static void Logd(String msg, Object... args) {
        if (DEBUG)
            Logger::D(TAG, args == NULL || args.length == 0 ? msg : String->Format(msg, args));
    }

    private class DreamInfoAdapter extends ArrayAdapter<DreamInfo> {
        private final LayoutInflater mInflater;

        public DreamInfoAdapter(Context context) {
            Super(context, 0);
            mInflater = (LayoutInflater) context->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            DreamInfo dreamInfo = GetItem(position);
            Logd("GetView(%s)", dreamInfo.caption);
            final View row = convertView != NULL ? convertView : CreateDreamInfoRow(parent);
            row->SetTag(dreamInfo);

            // bind icon
            ((ImageView) row->FindViewById(android.R.id.icon)).SetImageDrawable(dreamInfo.icon);

            // bind caption
            ((TextView) row->FindViewById(android.R.id.title)).SetText(dreamInfo.caption);

            // bind radio button
            RadioButton radioButton = (RadioButton) row->FindViewById(android.R.id.button1);
            radioButton->SetChecked(dreamInfo.isActive);
            radioButton->SetOnTouchListener(new OnTouchListener() {
                //@Override
                public Boolean OnTouch(View v, MotionEvent event) {
                    row->OnTouchEvent(event);
                    return FALSE;
                }});

            // bind settings button + divider
            Boolean showSettings = dreamInfo.settingsComponentName != NULL;
            View settingsDivider = row->FindViewById(R.id.divider);
            settingsDivider->SetVisibility(showSettings ? View.VISIBLE : View.INVISIBLE);

            ImageView settingsButton = (ImageView) row->FindViewById(android.R.id.button2);
            settingsButton->SetVisibility(showSettings ? View.VISIBLE : View.INVISIBLE);
            settingsButton->SetAlpha(dreamInfo.isActive ? 1f : Utils.DISABLED_ALPHA);
            settingsButton->SetEnabled(dreamInfo.isActive);
            settingsButton->SetFocusable(dreamInfo.isActive);
            settingsButton->SetOnClickListener(new OnClickListener(){
                //@Override
                CARAPI OnClick(View v) {
                    mBackend->LaunchSettings((DreamInfo) row->GetTag());
                }});

            return row;
        }

        private View CreateDreamInfoRow(ViewGroup parent) {
            final View row =  mInflater->Inflate(R.layout.dream_info_row, parent, FALSE);
            final View header = row->FindViewById(android.R.id.widget_frame);
            header->SetOnClickListener(new OnClickListener(){
                //@Override
                CARAPI OnClick(View v) {
                    v->SetPressed(TRUE);
                    Activate((DreamInfo) row->GetTag());
                }});
            return row;
        }

        private DreamInfo GetCurrentSelection() {
            for (Int32 i = 0; i < GetCount(); i++) {
                DreamInfo dreamInfo = GetItem(i);
                if (dreamInfo.isActive)
                    return dreamInfo;
            }
            return NULL;
        }
        private void Activate(DreamInfo dreamInfo) {
            if (dreamInfo->Equals(GetCurrentSelection()))
                return;
            for (Int32 i = 0; i < GetCount(); i++) {
                GetItem(i).isActive = FALSE;
            }
            dreamInfo.isActive = TRUE;
            mBackend->SetActiveDream(dreamInfo.componentName);
            NotifyDataSetChanged();
        }
    }

    private class PackageReceiver extends BroadcastReceiver {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            Logd("PackageReceiver.onReceive");
            RefreshFromBackend();
        }
    }
}
