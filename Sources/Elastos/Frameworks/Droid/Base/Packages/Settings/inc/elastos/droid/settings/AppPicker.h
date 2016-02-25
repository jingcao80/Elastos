

package com.android.settings;

using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

using Elastos::Droid::Settings::Applications::IAppViewHolder;

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IListActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IListView;

public class AppPicker extends ListActivity {
    private AppListAdapter mAdapter;

    //@Override
    protected void OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mAdapter = new AppListAdapter(this);
        if (mAdapter->GetCount() <= 0) {
            Finish();
        } else {
            SetListAdapter(mAdapter);
        }
    }

    //@Override
    protected void OnResume() {
        super->OnResume();
    }

    //@Override
    protected void OnStop() {
        super->OnStop();
    }

    //@Override
    protected void OnListItemClick(ListView l, View v, Int32 position, Int64 id) {
        MyApplicationInfo app = mAdapter->GetItem(position);
        Intent intent = new Intent();
        if (app.info != NULL) intent->SetAction(app.info.packageName);
        SetResult(RESULT_OK, intent);
        Finish();
    }

    class MyApplicationInfo {
        ApplicationInfo info;
        CharSequence label;
    }

    public class AppListAdapter extends ArrayAdapter<MyApplicationInfo> {
        private final List<MyApplicationInfo> mPackageInfoList = new ArrayList<MyApplicationInfo>();
        private final LayoutInflater mInflater;

        public AppListAdapter(Context context) {
            Super(context, 0);
            mInflater = (LayoutInflater)context->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
            List<ApplicationInfo> pkgs = context->GetPackageManager()->GetInstalledApplications(0);
            for (Int32 i=0; i<pkgs->Size(); i++) {
                ApplicationInfo ai = pkgs->Get(i);
                if (ai.uid == Process.SYSTEM_UID) {
                    continue;
                }
                // On a user build, we only allow debugging of apps that
                // are marked as debuggable.  Otherwise (for platform development)
                // we allow all apps.
                if ((ai.flags&ApplicationInfo.FLAG_DEBUGGABLE) == 0
                        && "user".Equals(Build.TYPE)) {
                    continue;
                }
                MyApplicationInfo info = new MyApplicationInfo();
                info.info = ai;
                info.label = info.info->LoadLabel(GetPackageManager()).ToString();
                mPackageInfoList->Add(info);
            }
            Collections->Sort(mPackageInfoList, sDisplayNameComparator);
            MyApplicationInfo info = new MyApplicationInfo();
            info.label = context->GetText(R::string::no_application);
            mPackageInfoList->Add(0, info);
            AddAll(mPackageInfoList);
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            // A ViewHolder keeps references to children views to avoid unnecessary calls
            // to FindViewById() on each row.
            AppViewHolder holder = AppViewHolder->CreateOrRecycle(mInflater, convertView);
            convertView = holder.rootView;
            MyApplicationInfo info = GetItem(position);
            holder.appName->SetText(info.label);
            if (info.info != NULL) {
                holder.appIcon->SetImageDrawable(info.info->LoadIcon(GetPackageManager()));
                holder.appSize->SetText(info.info.packageName);
            } else {
                holder.appIcon->SetImageDrawable(NULL);
                holder.appSize->SetText("");
            }
            holder.disabled->SetVisibility(View.GONE);
            holder.checkBox->SetVisibility(View.GONE);
            return convertView;
        }
    }

    private static const Comparator<MyApplicationInfo> sDisplayNameComparator
            = new Comparator<MyApplicationInfo>() {
        public final Int32
        Compare(MyApplicationInfo a, MyApplicationInfo b) {
            return collator->Compare(a.label, b.label);
        }

        private final Collator collator = Collator->GetInstance();
    };
}
