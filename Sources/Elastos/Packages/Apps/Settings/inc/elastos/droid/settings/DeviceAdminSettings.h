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

using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IListFragment;
using Elastos::Droid::App::Admin::IDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDeviceAdminReceiver;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

public class DeviceAdminSettings extends ListFragment {
    static const String TAG = "DeviceAdminSettings";

    static const Int32 DIALOG_WARNING = 1;
    private DevicePolicyManager mDPM;
    private UserManager mUm;

    /**
     * Internal collection of device admin info objects for all profiles associated with the current
     * user.
     */
    private final SparseArray<ArrayList<DeviceAdminInfo>>
            mAdminsByProfile = new SparseArray<ArrayList<DeviceAdminInfo>>();

    private String mDeviceOwnerPkg;
    private SparseArray<ComponentName> mProfileOwnerComponents = new SparseArray<ComponentName>();

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mDPM = (DevicePolicyManager) GetActivity()->GetSystemService(Context.DEVICE_POLICY_SERVICE);
        mUm = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        return inflater->Inflate(R.layout.device_admin_settings, container, FALSE);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        Utils->ForceCustomPadding(GetListView(), TRUE /* additive padding */);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mDeviceOwnerPkg = mDPM->GetDeviceOwner();
        if (mDeviceOwnerPkg != NULL && !mDPM->IsDeviceOwner(mDeviceOwnerPkg)) {
            mDeviceOwnerPkg = NULL;
        }
        mProfileOwnerComponents->Clear();
        final List<UserHandle> profiles = mUm->GetUserProfiles();
        final Int32 profilesSize = profiles->Size();
        for (Int32 i = 0; i < profilesSize; ++i) {
            final Int32 profileId = profiles->Get(i).GetIdentifier();
            mProfileOwnerComponents->Put(profileId, mDPM->GetProfileOwnerAsUser(profileId));
        }
        UpdateList();
    }

    /**
     * Update the internal collection of available admins for all profiles associated with the
     * current user.
     */
    void UpdateList() {
        mAdminsByProfile->Clear();

        final List<UserHandle> profiles = mUm->GetUserProfiles();
        final Int32 profilesSize = profiles->Size();
        for (Int32 i = 0; i < profilesSize; ++i) {
            final Int32 profileId = profiles->Get(i).GetIdentifier();
            UpdateAvailableAdminsForProfile(profileId);
        }

        GetListView()->SetAdapter(new PolicyListAdapter());
    }

    //@Override
    CARAPI OnListItemClick(ListView l, View v, Int32 position, Int64 id) {
        Object o = l->GetAdapter()->GetItem(position);
        if (!(o instanceof DeviceAdminInfo)) {
            // race conditions may cause this
            return;
        }
        DeviceAdminInfo dpi = (DeviceAdminInfo) o;
        final Activity activity = GetActivity();
        final Int32 userId = GetUserId(dpi);
        if (userId == UserHandle->MyUserId() || !IsProfileOwner(dpi)) {
            Intent intent = new Intent();
            intent->SetClass(activity, DeviceAdminAdd.class);
            intent->PutExtra(DevicePolicyManager.EXTRA_DEVICE_ADMIN, dpi->GetComponent());
            activity->StartActivityAsUser(intent, new UserHandle(userId));
        } else {
            AlertDialog.Builder builder = new AlertDialog->Builder(activity);
            builder->SetMessage(GetString(R::string::managed_profile_device_admin_info,
                    dpi->LoadLabel(activity->GetPackageManager())));
            builder->SetPositiveButton(android.R::string::ok, NULL);
            builder->Create()->Show();
        }
    }

    static class ViewHolder {
        ImageView icon;
        TextView name;
        CheckBox checkbox;
        TextView description;
    }

    class PolicyListAdapter extends BaseAdapter {
        final LayoutInflater mInflater;

        PolicyListAdapter() {
            mInflater = (LayoutInflater)
                    GetActivity()->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        //@Override
        public Boolean HasStableIds() {
            return FALSE;
        }

        //@Override
        public Int32 GetCount() {
            Int32 adminCount = 0;
            final Int32 profileCount = mAdminsByProfile->Size();
            for (Int32 i = 0; i < profileCount; ++i) {
                adminCount += mAdminsByProfile->ValueAt(i).Size();
            }
            // Add 'profileCount' for title items.
            return adminCount + profileCount;
        }

        /**
         * The item for the given position in the list.
         *
         * @return a String object for title items and a DeviceAdminInfo object for actual device
         *         admins.
         */
        //@Override
        public Object GetItem(Int32 position) {
            if (position < 0) {
                throw new ArrayIndexOutOfBoundsException();
            }
            // The position of the item in the list of admins.
            // We start from the given position and discount the length of the upper lists until we
            // get the one for the right profile
            Int32 adminPosition = position;
            final Int32 n = mAdminsByProfile->Size();
            Int32 i = 0;
            for (; i < n; ++i) {
                // The elements in that section including the title item (that's why adding one).
                final Int32 listSize = mAdminsByProfile->ValueAt(i).Size() + 1;
                if (adminPosition < listSize) {
                    break;
                }
                adminPosition -= listSize;
            }
            if (i == n) {
                throw new ArrayIndexOutOfBoundsException();
            }
            // If countdown == 0 that means the title item
            if (adminPosition == 0) {
                Resources res = GetActivity()->GetResources();
                if (mAdminsByProfile->KeyAt(i) == UserHandle->MyUserId()) {
                    return res->GetString(R::string::personal_device_admin_title);
                } else {
                    return res->GetString(R::string::managed_device_admin_title);
                }
            } else {
                // Subtracting one for the title.
                return mAdminsByProfile->ValueAt(i).Get(adminPosition - 1);
            }
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return position;
        }

        //@Override
        public Boolean AreAllItemsEnabled() {
            return FALSE;
        }

        /**
         * See {@link #getItemViewType} for the view types.
         */
        //@Override
        public Int32 GetViewTypeCount() {
            return 2;
        }

        /**
         * Returns 1 for title items and 0 for anything else.
         */
        //@Override
        public Int32 GetItemViewType(Int32 position) {
            Object o = GetItem(position);
            return (o instanceof String) ? 1 : 0;
        }

        //@Override
        public Boolean IsEnabled(Int32 position) {
            Object o = GetItem(position);
            return IsEnabled(o);
        }

        private Boolean IsEnabled(Object o) {
            if (!(o instanceof DeviceAdminInfo)) {
                // Title item
                return FALSE;
            }
            DeviceAdminInfo info = (DeviceAdminInfo) o;
            if (IsActiveAdmin(info) && GetUserId(info) == UserHandle->MyUserId()
                    && (IsDeviceOwner(info) || IsProfileOwner(info))) {
                return FALSE;
            }
            return TRUE;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            Object o = GetItem(position);
            if (o instanceof DeviceAdminInfo) {
                if (convertView == NULL) {
                    convertView = NewDeviceAdminView(parent);
                }
                BindView(convertView, (DeviceAdminInfo) o);
            } else {
                if (convertView == NULL) {
                    convertView = NewTitleView(parent);
                }
                final TextView title = (TextView) convertView->FindViewById(android.R.id.title);
                title->SetText((String)o);
            }
            return convertView;
        }

        private View NewDeviceAdminView(ViewGroup parent) {
            View v = mInflater->Inflate(R.layout.device_admin_item, parent, FALSE);
            ViewHolder h = new ViewHolder();
            h.icon = (ImageView)v->FindViewById(R.id.icon);
            h.name = (TextView)v->FindViewById(R.id.name);
            h.checkbox = (CheckBox)v->FindViewById(R.id.checkbox);
            h.description = (TextView)v->FindViewById(R.id.description);
            v->SetTag(h);
            return v;
        }

        private View NewTitleView(ViewGroup parent) {
            final TypedArray a = mInflater->GetContext()->ObtainStyledAttributes(NULL,
                    R.styleable.Preference,
                    R.attr.preferenceCategoryStyle, 0);
            final Int32 resId = a->GetResourceId(R.styleable.Preference_layout,
                    0);
            return mInflater->Inflate(resId, parent, FALSE);
        }

        private void BindView(View view, DeviceAdminInfo item) {
            final Activity activity = GetActivity();
            ViewHolder vh = (ViewHolder) view->GetTag();
            Drawable activityIcon = item->LoadIcon(activity->GetPackageManager());
            Drawable badgedIcon = activity->GetPackageManager()->GetUserBadgedIcon(
                    activityIcon, new UserHandle(GetUserId(item)));
            vh.icon->SetImageDrawable(badgedIcon);
            vh.name->SetText(item->LoadLabel(activity->GetPackageManager()));
            vh.checkbox->SetChecked(IsActiveAdmin(item));
            final Boolean enabled = IsEnabled(item);
            try {
                vh.description->SetText(item->LoadDescription(activity->GetPackageManager()));
            } catch (Resources.NotFoundException e) {
            }
            vh.checkbox->SetEnabled(enabled);
            vh.name->SetEnabled(enabled);
            vh.description->SetEnabled(enabled);
            vh.icon->SetEnabled(enabled);
        }
    }

    private Boolean IsDeviceOwner(DeviceAdminInfo item) {
        return GetUserId(item) == UserHandle->MyUserId()
                && item->GetPackageName()->Equals(mDeviceOwnerPkg);
    }

    private Boolean IsProfileOwner(DeviceAdminInfo item) {
        ComponentName profileOwner = mProfileOwnerComponents->Get(GetUserId(item));
        return item->GetComponent()->Equals(profileOwner);
    }

    private Boolean IsActiveAdmin(DeviceAdminInfo item) {
        return mDPM->IsAdminActiveAsUser(item->GetComponent(), GetUserId(item));
    }

    /**
     * Add device admins to the internal collection that belong to a profile.
     *
     * @param profileId the profile identifier.
     */
    private void UpdateAvailableAdminsForProfile(final Int32 profileId) {
        // We are adding the union of two sets 'A' and 'B' of device admins to mAvailableAdmins.
        // Set 'A' is the set of active admins for the profile whereas set 'B' is the set of
        // listeners to DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED for the profile.

        // Add all of set 'A' to mAvailableAdmins.
        List<ComponentName> activeAdminsListForProfile = mDPM->GetActiveAdminsAsUser(profileId);
        AddActiveAdminsForProfile(activeAdminsListForProfile, profileId);

        // Collect set 'B' and add B-A to mAvailableAdmins.
        AddDeviceAdminBroadcastReceiversForProfile(activeAdminsListForProfile, profileId);
    }

    /**
     * Add a profile's device admins that are receivers of
     * {@code DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED} to the internal collection if they
     * haven't been added yet.
     *
     * @param alreadyAddedComponents the set of active admin component names. Receivers of
     *            {@code DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED} whose component is in this
     *            set are not added to the internal collection again.
     * @param profileId the identifier of the profile
     */
    private void AddDeviceAdminBroadcastReceiversForProfile(
            Collection<ComponentName> alreadyAddedComponents, final Int32 profileId) {
        final PackageManager pm = GetActivity()->GetPackageManager();
        List<ResolveInfo> enabledForProfile = pm->QueryBroadcastReceivers(
                new Intent(DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED),
                PackageManager.GET_META_DATA | PackageManager.GET_DISABLED_UNTIL_USED_COMPONENTS,
                profileId);
        if (enabledForProfile == NULL) {
            enabledForProfile = Collections->EmptyList();
        }
        final Int32 n = enabledForProfile->Size();
        ArrayList<DeviceAdminInfo> deviceAdmins = mAdminsByProfile->Get(profileId);
        if (deviceAdmins == NULL) {
            deviceAdmins = new ArrayList<DeviceAdminInfo>(n);
        }
        for (Int32 i = 0; i < n; ++i) {
            ResolveInfo resolveInfo = enabledForProfile->Get(i);
            ComponentName riComponentName =
                    new ComponentName(resolveInfo.activityInfo.packageName,
                            resolveInfo.activityInfo.name);
            if (alreadyAddedComponents == NULL
                    || !alreadyAddedComponents->Contains(riComponentName)) {
                DeviceAdminInfo deviceAdminInfo = CreateDeviceAdminInfo(resolveInfo);
                // add only visible ones (note: active admins are added regardless of visibility)
                if (deviceAdminInfo != NULL && deviceAdminInfo->IsVisible()) {
                    deviceAdmins->Add(deviceAdminInfo);
                }
            }
        }
        if (!deviceAdmins->IsEmpty()) {
            mAdminsByProfile->Put(profileId, deviceAdmins);
        }
    }

    /**
     * Add a {@link DeviceAdminInfo} object to the internal collection of available admins for all
     * active admin components associated with a profile.
     *
     * @param profileId a profile identifier.
     */
    private void AddActiveAdminsForProfile(final List<ComponentName> activeAdmins,
            final Int32 profileId) {
        if (activeAdmins != NULL) {
            final PackageManager packageManager = GetActivity()->GetPackageManager();
            final Int32 n = activeAdmins->Size();
            ArrayList<DeviceAdminInfo> deviceAdmins = new ArrayList<DeviceAdminInfo>(n);
            for (Int32 i = 0; i < n; ++i) {
                ComponentName activeAdmin = activeAdmins->Get(i);
                List<ResolveInfo> resolved = packageManager->QueryBroadcastReceivers(
                        new Intent()->SetComponent(activeAdmin), PackageManager.GET_META_DATA
                                | PackageManager.GET_DISABLED_UNTIL_USED_COMPONENTS, profileId);
                if (resolved != NULL) {
                    final Int32 resolvedMax = resolved->Size();
                    for (Int32 j = 0; j < resolvedMax; ++j) {
                        DeviceAdminInfo deviceAdminInfo = CreateDeviceAdminInfo(resolved->Get(j));
                        if (deviceAdminInfo != NULL) {
                            deviceAdmins->Add(deviceAdminInfo);
                        }
                    }
                }
            }
            if (!deviceAdmins->IsEmpty()) {
                mAdminsByProfile->Put(profileId, deviceAdmins);
            }
        }
    }

    /**
     * Creates a device admin info object for the resolved intent that points to the component of
     * the device admin.
     *
     * @param resolved resolved intent.
     * @return new {@link DeviceAdminInfo} object or NULL if there was an error.
     */
    private DeviceAdminInfo CreateDeviceAdminInfo(ResolveInfo resolved) {
        try {
            return new DeviceAdminInfo(GetActivity(), resolved);
        } catch (XmlPullParserException e) {
            Logger::W(TAG, "Skipping " + resolved.activityInfo, e);
        } catch (IOException e) {
            Logger::W(TAG, "Skipping " + resolved.activityInfo, e);
        }
        return NULL;
    }

    /**
     * Extracts the user id from a device admin info object.
     * @param adminInfo the device administrator info.
     * @return identifier of the user associated with the device admin.
     */
    private Int32 GetUserId(DeviceAdminInfo adminInfo) {
        return UserHandle->GetUserId(adminInfo->GetActivityInfo().applicationInfo.uid);
    }
}
