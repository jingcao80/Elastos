

package com.android.settings.notification;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IListFragment;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

public abstract class ManagedServiceSettings extends ListFragment {
    private static const Boolean SHOW_PACKAGE_NAME = FALSE;

    private final Config mConfig;
    private PackageManager mPM;
    private ContentResolver mCR;

    private final HashSet<ComponentName> mEnabledServices = new HashSet<ComponentName>();
    private ServiceListAdapter mListAdapter;

    abstract protected Config GetConfig();

    public ManagedServiceSettings() {
        mConfig = GetConfig();
    }

    private final ContentObserver mSettingsObserver = new ContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            UpdateList();
        }
    };

    private final BroadcastReceiver mPackageReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            UpdateList();
        }
    };

    public class ScaryWarningDialogFragment extends DialogFragment {
        static const String KEY_COMPONENT = "c";
        static const String KEY_LABEL = "l";

        public ScaryWarningDialogFragment SetServiceInfo(ComponentName cn, String label) {
            Bundle args = new Bundle();
            args->PutString(KEY_COMPONENT, cn->FlattenToString());
            args->PutString(KEY_LABEL, label);
            SetArguments(args);
            return this;
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            super->OnCreate(savedInstanceState);
            final Bundle args = GetArguments();
            final String label = args->GetString(KEY_LABEL);
            final ComponentName cn = ComponentName->UnflattenFromString(args->GetString(KEY_COMPONENT));

            final String title = GetResources()->GetString(mConfig.warningDialogTitle, label);
            final String summary = GetResources()->GetString(mConfig.warningDialogSummary, label);
            return new AlertDialog->Builder(GetActivity())
                    .SetMessage(summary)
                    .SetTitle(title)
                    .SetCancelable(TRUE)
                    .SetPositiveButton(android.R::string::ok,
                            new DialogInterface->OnClickListener() {
                                CARAPI OnClick(DialogInterface dialog, Int32 id) {
                                    mEnabledServices->Add(cn);
                                    SaveEnabledServices();
                                }
                            })
                    .SetNegativeButton(android.R::string::cancel,
                            new DialogInterface->OnClickListener() {
                                CARAPI OnClick(DialogInterface dialog, Int32 id) {
                                    // pass
                                }
                            })
                    .Create();
        }
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mPM = GetActivity()->GetPackageManager();
        mCR = GetActivity()->GetContentResolver();
        mListAdapter = new ServiceListAdapter(GetActivity());
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View v =  inflater->Inflate(R.layout.managed_service_settings, container, FALSE);
        TextView empty = (TextView) v->FindViewById(android.R.id.empty);
        empty->SetText(mConfig.emptyText);
        return v;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        UpdateList();

        // listen for package changes
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
        filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
        filter->AddDataScheme("package");
        GetActivity()->RegisterReceiver(mPackageReceiver, filter);

        mCR->RegisterContentObserver(Settings.Secure->GetUriFor(mConfig.setting),
                FALSE, mSettingsObserver);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        GetActivity()->UnregisterReceiver(mPackageReceiver);
        mCR->UnregisterContentObserver(mSettingsObserver);
    }

    private void LoadEnabledServices() {
        mEnabledServices->Clear();
        final String flat = Settings.Secure->GetString(mCR, mConfig.setting);
        if (flat != NULL && !"".Equals(flat)) {
            final String[] names = flat->Split(":");
            for (Int32 i = 0; i < names.length; i++) {
                final ComponentName cn = ComponentName->UnflattenFromString(names[i]);
                if (cn != NULL) {
                    mEnabledServices->Add(cn);
                }
            }
        }
    }

    private void SaveEnabledServices() {
        StringBuilder sb = NULL;
        for (ComponentName cn : mEnabledServices) {
            if (sb == NULL) {
                sb = new StringBuilder();
            } else {
                sb->Append(':');
            }
            sb->Append(cn->FlattenToString());
        }
        Settings.Secure->PutString(mCR,
                mConfig.setting,
                sb != NULL ? sb->ToString() : "");
    }

    private void UpdateList() {
        LoadEnabledServices();

        GetServices(mConfig, mListAdapter, mPM);
        mListAdapter->Sort(new PackageItemInfo->DisplayNameComparator(mPM));

        GetListView()->SetAdapter(mListAdapter);
    }

    protected static Int32 GetEnabledServicesCount(Config config, Context context) {
        final String flat = Settings.Secure->GetString(context->GetContentResolver(), config.setting);
        if (flat == NULL || "".Equals(flat)) return 0;
        final String[] components = flat->Split(":");
        return components.length;
    }

    protected static Int32 GetServicesCount(Config c, PackageManager pm) {
        return GetServices(c, NULL, pm);
    }

    private static Int32 GetServices(Config c, ArrayAdapter<ServiceInfo> adapter, PackageManager pm) {
        Int32 services = 0;
        if (adapter != NULL) {
            adapter->Clear();
        }
        final Int32 user = ActivityManager->GetCurrentUser();

        List<ResolveInfo> installedServices = pm->QueryIntentServicesAsUser(
                new Intent(c.intentAction),
                PackageManager.GET_SERVICES | PackageManager.GET_META_DATA,
                user);

        for (Int32 i = 0, count = installedServices->Size(); i < count; i++) {
            ResolveInfo resolveInfo = installedServices->Get(i);
            ServiceInfo info = resolveInfo.serviceInfo;

            if (!c.permission->Equals(info.permission)) {
                Slogger::W(c.tag, "Skipping " + c.noun + " service "
                        + info.packageName + "/" + info.name
                        + ": it does not require the permission "
                        + c.permission);
                continue;
            }
            if (adapter != NULL) {
                adapter->Add(info);
            }
            services++;
        }
        return services;
    }

    private Boolean IsServiceEnabled(ServiceInfo info) {
        final ComponentName cn = new ComponentName(info.packageName, info.name);
        return mEnabledServices->Contains(cn);
    }

    //@Override
    CARAPI OnListItemClick(ListView l, View v, Int32 position, Int64 id) {
        ServiceInfo info = mListAdapter->GetItem(position);
        final ComponentName cn = new ComponentName(info.packageName, info.name);
        if (mEnabledServices->Contains(cn)) {
            // the simple version: disabling
            mEnabledServices->Remove(cn);
            SaveEnabledServices();
        } else {
            // show a scary dialog
            new ScaryWarningDialogFragment()
                .SetServiceInfo(cn, info->LoadLabel(mPM).ToString())
                .Show(GetFragmentManager(), "dialog");
        }
    }

    private static class ViewHolder {
        ImageView icon;
        TextView name;
        CheckBox checkbox;
        TextView description;
    }

    private class ServiceListAdapter extends ArrayAdapter<ServiceInfo> {
        final LayoutInflater mInflater;

        ServiceListAdapter(Context context) {
            Super(context, 0, 0);
            mInflater = (LayoutInflater)
                    GetActivity()->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public Boolean HasStableIds() {
            return TRUE;
        }

        public Int64 GetItemId(Int32 position) {
            return position;
        }

        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            View v;
            if (convertView == NULL) {
                v = NewView(parent);
            } else {
                v = convertView;
            }
            BindView(v, position);
            return v;
        }

        public View NewView(ViewGroup parent) {
            View v = mInflater->Inflate(R.layout.managed_service_item, parent, FALSE);
            ViewHolder h = new ViewHolder();
            h.icon = (ImageView) v->FindViewById(R.id.icon);
            h.name = (TextView) v->FindViewById(R.id.name);
            h.checkbox = (CheckBox) v->FindViewById(R.id.checkbox);
            h.description = (TextView) v->FindViewById(R.id.description);
            v->SetTag(h);
            return v;
        }

        CARAPI BindView(View view, Int32 position) {
            ViewHolder vh = (ViewHolder) view->GetTag();
            ServiceInfo info = GetItem(position);

            vh.icon->SetImageDrawable(info->LoadIcon(mPM));
            vh.name->SetText(info->LoadLabel(mPM));
            if (SHOW_PACKAGE_NAME) {
                vh.description->SetText(info.packageName);
                vh.description->SetVisibility(View.VISIBLE);
            } else {
                vh.description->SetVisibility(View.GONE);
            }
            vh.checkbox->SetChecked(IsServiceEnabled(info));
        }
    }

    protected static class Config {
        String tag;
        String setting;
        String intentAction;
        String permission;
        String noun;
        Int32 warningDialogTitle;
        Int32 warningDialogSummary;
        Int32 emptyText;
    }
}
