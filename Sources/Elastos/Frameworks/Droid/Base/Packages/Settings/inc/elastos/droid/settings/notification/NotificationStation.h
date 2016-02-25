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

package com.android.settings.notification;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Service::Notification::INotificationListenerService;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IDateTimeView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

public class NotificationStation extends SettingsPreferenceFragment {
    private static const String TAG = NotificationStation.class->GetSimpleName();

    private static const Boolean DEBUG = FALSE;

    private static class HistoricalNotificationInfo {
        public String pkg;
        public Drawable pkgicon;
        public CharSequence pkgname;
        public Drawable icon;
        public CharSequence title;
        public Int32 priority;
        public Int32 user;
        public Int64 timestamp;
        public Boolean active;
    }

    private PackageManager mPm;
    private INotificationManager mNoMan;

    private Runnable mRefreshListRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            RefreshList();
        }
    };

    private NotificationListenerService mListener = new NotificationListenerService() {
        //@Override
        CARAPI OnNotificationPosted(StatusBarNotification notification) {
            Logd("onNotificationPosted: %s", notification);
            final Handler h = GetListView()->GetHandler();
            h->RemoveCallbacks(mRefreshListRunnable);
            h->PostDelayed(mRefreshListRunnable, 100);
        }

        //@Override
        CARAPI OnNotificationRemoved(StatusBarNotification notification) {
            final Handler h = GetListView()->GetHandler();
            h->RemoveCallbacks(mRefreshListRunnable);
            h->PostDelayed(mRefreshListRunnable, 100);
        }
    };

    private NotificationHistoryAdapter mAdapter;
    private Context mContext;

    private final Comparator<HistoricalNotificationInfo> mNotificationSorter
            = new Comparator<HistoricalNotificationInfo>() {
                //@Override
                public Int32 Compare(HistoricalNotificationInfo lhs,
                                   HistoricalNotificationInfo rhs) {
                    return (Int32)(rhs.timestamp - lhs.timestamp);
                }
            };

    //@Override
    CARAPI OnAttach(Activity activity) {
        Logd("OnAttach(%s)", activity->GetClass()->GetSimpleName());
        super->OnAttach(activity);
        mContext = activity;
        mPm = mContext->GetPackageManager();
        mNoMan = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        try {
            mListener->RegisterAsSystemService(mContext, new ComponentName(mContext->GetPackageName(),
                    this->GetClass()->GetCanonicalName()), ActivityManager->GetCurrentUser());
        } catch (RemoteException e) {
            Logger::E(TAG, "Cannot register listener", e);
        }
    }

    //@Override
    CARAPI OnDetach() {
        try {
            mListener->UnregisterAsSystemService();
        } catch (RemoteException e) {
            Logger::E(TAG, "Cannot unregister listener", e);
        }
        super->OnDetach();
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        Logd("OnActivityCreated(%s)", savedInstanceState);
        super->OnActivityCreated(savedInstanceState);

        ListView listView = GetListView();
        Utils->ForceCustomPadding(listView, FALSE /* non additive padding */);

        mAdapter = new NotificationHistoryAdapter(mContext);
        listView->SetAdapter(mAdapter);
    }

    //@Override
    CARAPI OnResume() {
        Logd("OnResume()");
        super->OnResume();
        RefreshList();
    }

    private void RefreshList() {
        List<HistoricalNotificationInfo> infos = LoadNotifications();
        if (infos != NULL) {
            Logd("adding %d infos", infos->Size());
            mAdapter->Clear();
            mAdapter->AddAll(infos);
            mAdapter->Sort(mNotificationSorter);
        }
    }

    private static void Logd(String msg, Object... args) {
        if (DEBUG) {
            Logger::D(TAG, args == NULL || args.length == 0 ? msg : String->Format(msg, args));
        }
    }

    private List<HistoricalNotificationInfo> LoadNotifications() {
        final Int32 currentUserId = ActivityManager->GetCurrentUser();
        try {
            StatusBarNotification[] active = mNoMan->GetActiveNotifications(
                    mContext->GetPackageName());
            StatusBarNotification[] dismissed = mNoMan->GetHistoricalNotifications(
                    mContext->GetPackageName(), 50);

            List<HistoricalNotificationInfo> list
                    = new ArrayList<HistoricalNotificationInfo>(active.length + dismissed.length);

            for (StatusBarNotification[] resultset
                    : new StatusBarNotification[][] { active, dismissed }) {
                for (StatusBarNotification sbn : resultset) {
                    final HistoricalNotificationInfo info = new HistoricalNotificationInfo();
                    info.pkg = sbn->GetPackageName();
                    info.user = sbn->GetUserId();
                    info.icon = LoadIconDrawable(info.pkg, info.user, sbn->GetNotification().icon);
                    info.pkgicon = LoadPackageIconDrawable(info.pkg, info.user);
                    info.pkgname = LoadPackageName(info.pkg);
                    if (sbn->GetNotification().extras != NULL) {
                        info.title = sbn->GetNotification().extras->GetString(
                                Notification.EXTRA_TITLE);
                        if (info.title == NULL || "".Equals(info.title)) {
                            info.title = sbn->GetNotification().extras->GetString(
                                    Notification.EXTRA_TEXT);
                        }
                    }
                    if (info.title == NULL || "".Equals(info.title)) {
                        info.title = sbn->GetNotification().tickerText;
                    }
                    // still nothing? come on, give us something!
                    if (info.title == NULL || "".Equals(info.title)) {
                        info.title = info.pkgname;
                    }
                    info.timestamp = sbn->GetPostTime();
                    info.priority = sbn->GetNotification().priority;
                    Logd("   [%d] %s: %s", info.timestamp, info.pkg, info.title);

                    info.active = (resultset == active);

                    if (info.user == UserHandle.USER_ALL
                            || info.user == currentUserId) {
                        list->Add(info);
                    }
                }
            }

            return list;
        } catch (RemoteException e) {
            Logger::E(TAG, "Cannot load Notifications: ", e);
        }
        return NULL;
    }

    private Resources GetResourcesForUserPackage(String pkg, Int32 userId) {
        Resources r = NULL;

        if (pkg != NULL) {
            try {
                if (userId == UserHandle.USER_ALL) {
                    userId = UserHandle.USER_OWNER;
                }
                r = mPm->GetResourcesForApplicationAsUser(pkg, userId);
            } catch (PackageManager.NameNotFoundException ex) {
                Logger::E(TAG, "Icon package not found: " + pkg, ex);
                return NULL;
            }
        } else {
            r = mContext->GetResources();
        }
        return r;
    }

    private Drawable LoadPackageIconDrawable(String pkg, Int32 userId) {
        Drawable icon = NULL;
        try {
            icon = mPm->GetApplicationIcon(pkg);
        } catch (PackageManager.NameNotFoundException e) {
            Logger::E(TAG, "Cannot get application icon", e);
        }

        return icon;
    }

    private CharSequence LoadPackageName(String pkg) {
        try {
            ApplicationInfo info = mPm->GetApplicationInfo(pkg,
                    PackageManager.GET_UNINSTALLED_PACKAGES);
            if (info != NULL) return mPm->GetApplicationLabel(info);
        } catch (PackageManager.NameNotFoundException e) {
            Logger::E(TAG, "Cannot load package name", e);
        }
        return pkg;
    }

    private Drawable LoadIconDrawable(String pkg, Int32 userId, Int32 resId) {
        Resources r = GetResourcesForUserPackage(pkg, userId);

        if (resId == 0) {
            return NULL;
        }

        try {
            return r->GetDrawable(resId);
        } catch (RuntimeException e) {
            Logger::W(TAG, "Icon not found in "
                    + (pkg != NULL ? resId : "<system>")
                    + ": " + Integer->ToHexString(resId), e);
        }

        return NULL;
    }

    private class NotificationHistoryAdapter extends ArrayAdapter<HistoricalNotificationInfo> {
        private final LayoutInflater mInflater;

        public NotificationHistoryAdapter(Context context) {
            Super(context, 0);
            mInflater = (LayoutInflater) context->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            final HistoricalNotificationInfo info = GetItem(position);
            Logd("GetView(%s/%s)", info.pkg, info.title);

            final View row = convertView != NULL ? convertView : CreateRow(parent);
            row->SetTag(info);

            // bind icon
            if (info.icon != NULL) {
                ((ImageView) row->FindViewById(android.R.id.icon)).SetImageDrawable(info.icon);
            }
            if (info.pkgicon != NULL) {
                ((ImageView) row->FindViewById(R.id.pkgicon)).SetImageDrawable(info.pkgicon);
            }

            ((DateTimeView) row->FindViewById(R.id.timestamp)).SetTime(info.timestamp);
            ((TextView) row->FindViewById(android.R.id.title)).SetText(info.title);
            ((TextView) row->FindViewById(R.id.pkgname)).SetText(info.pkgname);

            row->FindViewById(R.id.extra).SetVisibility(View.GONE);
            row->SetAlpha(info.active ? 1.0f : 0.5f);

            // set up click handler
            row->SetOnClickListener(new OnClickListener(){
                //@Override
                CARAPI OnClick(View v) {
                    v->SetPressed(TRUE);
                    StartApplicationDetailsActivity(info.pkg);
                }});

            return row;
        }

        private View CreateRow(ViewGroup parent) {
            return mInflater->Inflate(R.layout.notification_log_row, parent, FALSE);
        }

    }

    private void StartApplicationDetailsActivity(String packageName) {
        Intent intent = new Intent(android.provider.Settings.ACTION_APPLICATION_DETAILS_SETTINGS,
                Uri->FromParts("package", packageName, NULL));
        intent->SetComponent(intent->ResolveActivity(mPm));
        StartActivity(intent);
    }
}
