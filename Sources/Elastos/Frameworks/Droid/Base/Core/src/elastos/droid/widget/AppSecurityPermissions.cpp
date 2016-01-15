
#include "elastos/droid/widget/AppSecurityPermissions.h"


AppSecurityPermissions::MyPermissionGroupInfo::MyPermissionGroupInfo(
    /* [in] */ PermissionInfo perm)
{
    name = perm.packageName;
    packageName = perm.packageName;
}

AppSecurityPermissions::MyPermissionGroupInfo::MyPermissionGroupInfo(
    /* [in] */ PermissionGroupInfo info)
{
    super(info);
}

AutoPtr<IDrawable> AppSecurityPermissions::MyPermissionGroupInfo::LoadGroupIcon(
    /* [in] */ PackageManager pm)
{
    if (icon != 0) {
        return loadIcon(pm);
    } else {
        ApplicationInfo appInfo;
        try {
            appInfo = pm.getApplicationInfo(packageName, 0);
            return appInfo.loadIcon(pm);
        } catch (NameNotFoundException e) {
        }
    }
    return NULL;
}





AppSecurityPermissions::MyPermissionInfo::MyPermissionInfo()
{
}

AppSecurityPermissions::MyPermissionInfo::MyPermissionInfo(
    /* [in] */ PermissionInfo info)
{
    super(info);
}

AppSecurityPermissions::MyPermissionInfo::MyPermissionInfo(MyPermissionInfo info) {
    super(info);
    mNewReqFlags = info.mNewReqFlags;
    mExistingReqFlags = info.mExistingReqFlags;
    mNew = info.mNew;
}


public static class PermissionItemView extends LinearLayout implements View.OnClickListener {
    MyPermissionGroupInfo mGroup;
    MyPermissionInfo mPerm;
    AlertDialog mDialog;

    public PermissionItemView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setClickable(true);
    }

    public void setPermission(MyPermissionGroupInfo grp, MyPermissionInfo perm,
            Boolean first, CharSequence newPermPrefix) {
        mGroup = grp;
        mPerm = perm;

        ImageView permGrpIcon = (ImageView) findViewById(R.id.perm_icon);
        TextView permNameView = (TextView) findViewById(R.id.perm_name);

        PackageManager pm = getContext().getPackageManager();
        Drawable icon = NULL;
        if (first) {
            icon = grp.loadGroupIcon(pm);
        }
        CharSequence label = perm.mLabel;
        if (perm.mNew && newPermPrefix != NULL) {
            // If this is a new permission, format it appropriately.
            SpannableStringBuilder builder = new SpannableStringBuilder();
            Parcel parcel = Parcel.obtain();
            TextUtils.writeToParcel(newPermPrefix, parcel, 0);
            parcel.setDataPosition(0);
            CharSequence newStr = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(parcel);
            parcel.recycle();
            builder.append(newStr);
            builder.append(label);
            label = builder;
        }

        permGrpIcon.setImageDrawable(icon);
        permNameView.setText(label);
        setOnClickListener(this);
        if (localLOGV) Log.i(TAG, "Made perm item " + perm.name
                + ": " + label + " in group " + grp.name);
    }

    @Override
    public void onClick(View v) {
        if (mGroup != NULL && mPerm != NULL) {
            if (mDialog != NULL) {
                mDialog.dismiss();
            }
            PackageManager pm = getContext().getPackageManager();
            AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
            builder.setTitle(mGroup.mLabel);
            if (mPerm.descriptionRes != 0) {
                builder.setMessage(mPerm.loadDescription(pm));
            } else {
                CharSequence appName;
                try {
                    ApplicationInfo app = pm.getApplicationInfo(mPerm.packageName, 0);
                    appName = app.loadLabel(pm);
                } catch (NameNotFoundException e) {
                    appName = mPerm.packageName;
                }
                StringBuilder sbuilder = new StringBuilder(128);
                sbuilder.append(getContext().getString(
                        R.string.perms_description_app, appName));
                sbuilder.append("\n\n");
                sbuilder.append(mPerm.name);
                builder.setMessage(sbuilder.toString());
            }
            builder.setCancelable(true);
            builder.setIcon(mGroup.loadGroupIcon(pm));
            mDialog = builder.show();
            mDialog.setCanceledOnTouchOutside(true);
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        if (mDialog != NULL) {
            mDialog.dismiss();
        }
    }
}

public AppSecurityPermissions(Context context, List<PermissionInfo> permList) {
    mContext = context;
    mPm = mContext.getPackageManager();
    loadResources();
    mPermComparator = new PermissionInfoComparator();
    mPermGroupComparator = new PermissionGroupInfoComparator();
    for (PermissionInfo pi : permList) {
        mPermsList.add(new MyPermissionInfo(pi));
    }
    setPermissions(mPermsList);
}

public AppSecurityPermissions(Context context, String packageName) {
    mContext = context;
    mPm = mContext.getPackageManager();
    loadResources();
    mPermComparator = new PermissionInfoComparator();
    mPermGroupComparator = new PermissionGroupInfoComparator();
    mPermsList = new ArrayList<MyPermissionInfo>();
    Set<MyPermissionInfo> permSet = new HashSet<MyPermissionInfo>();
    PackageInfo pkgInfo;
    try {
        pkgInfo = mPm.getPackageInfo(packageName, PackageManager.GET_PERMISSIONS);
    } catch (NameNotFoundException e) {
        Log.w(TAG, "Couldn't retrieve permissions for package:"+packageName);
        return;
    }
    // Extract all user permissions
    if((pkgInfo.applicationInfo != NULL) && (pkgInfo.applicationInfo.uid != -1)) {
        getAllUsedPermissions(pkgInfo.applicationInfo.uid, permSet);
    }
    for(MyPermissionInfo tmpInfo : permSet) {
        mPermsList.add(tmpInfo);
    }
    setPermissions(mPermsList);
}

public AppSecurityPermissions(Context context, PackageInfo info) {
    mContext = context;
    mPm = mContext.getPackageManager();
    loadResources();
    mPermComparator = new PermissionInfoComparator();
    mPermGroupComparator = new PermissionGroupInfoComparator();
    mPermsList = new ArrayList<MyPermissionInfo>();
    Set<MyPermissionInfo> permSet = new HashSet<MyPermissionInfo>();
    if(info == NULL) {
        return;
    }

    // Convert to a PackageInfo
    PackageInfo installedPkgInfo = NULL;
    // Get requested permissions
    if (info.requestedPermissions != NULL) {
        try {
            installedPkgInfo = mPm.getPackageInfo(info.packageName,
                    PackageManager.GET_PERMISSIONS);
        } catch (NameNotFoundException e) {
        }
        extractPerms(info, permSet, installedPkgInfo);
    }
    // Get permissions related to  shared user if any
    if (info.sharedUserId != NULL) {
        Int32 sharedUid;
        try {
            sharedUid = mPm.getUidForSharedUser(info.sharedUserId);
            getAllUsedPermissions(sharedUid, permSet);
        } catch (NameNotFoundException e) {
            Log.w(TAG, "Could'nt retrieve shared user id for:"+info.packageName);
        }
    }
    // Retrieve list of permissions
    for (MyPermissionInfo tmpInfo : permSet) {
        mPermsList.add(tmpInfo);
    }
    setPermissions(mPermsList);
}

private void loadResources() {
    // Pick up from framework resources instead.
    mNewPermPrefix = mContext.getText(R.string.perms_new_perm_prefix);
    mNormalIcon = mContext.getResources().getDrawable(R.drawable.ic_text_dot);
    mDangerousIcon = mContext.getResources().getDrawable(R.drawable.ic_bullet_key_permission);
}

/**
 * Utility to retrieve a view displaying a single permission.  This provides
 * the old UI layout for permissions; it is only here for the device admin
 * settings to continue to use.
 */
public static View getPermissionItemView(Context context,
        CharSequence grpName, CharSequence description, Boolean dangerous) {
    LayoutInflater inflater = (LayoutInflater)context.getSystemService(
            Context.LAYOUT_INFLATER_SERVICE);
    Drawable icon = context.getResources().getDrawable(dangerous
            ? R.drawable.ic_bullet_key_permission : R.drawable.ic_text_dot);
    return getPermissionItemViewOld(context, inflater, grpName,
            description, dangerous, icon);
}

public PackageInfo getInstalledPackageInfo() {
    return mInstalledPackageInfo;
}

private void getAllUsedPermissions(Int32 sharedUid, Set<MyPermissionInfo> permSet) {
    String sharedPkgList[] = mPm.getPackagesForUid(sharedUid);
    if(sharedPkgList == NULL || (sharedPkgList.length == 0)) {
        return;
    }
    for(String sharedPkg : sharedPkgList) {
        getPermissionsForPackage(sharedPkg, permSet);
    }
}

private void getPermissionsForPackage(String packageName,
        Set<MyPermissionInfo> permSet) {
    PackageInfo pkgInfo;
    try {
        pkgInfo = mPm.getPackageInfo(packageName, PackageManager.GET_PERMISSIONS);
    } catch (NameNotFoundException e) {
        Log.w(TAG, "Couldn't retrieve permissions for package:"+packageName);
        return;
    }
    if ((pkgInfo != NULL) && (pkgInfo.requestedPermissions != NULL)) {
        extractPerms(pkgInfo, permSet, pkgInfo);
    }
}

private void extractPerms(PackageInfo info, Set<MyPermissionInfo> permSet,
        PackageInfo installedPkgInfo) {
    String[] strList = info.requestedPermissions;
    Int32[] flagsList = info.requestedPermissionsFlags;
    if ((strList == NULL) || (strList.length == 0)) {
        return;
    }
    mInstalledPackageInfo = installedPkgInfo;
    for (Int32 i=0; i<strList.length; i++) {
        String permName = strList[i];
        // If we are only looking at an existing app, then we only
        // care about permissions that have actually been granted to it.
        if (installedPkgInfo != NULL && info == installedPkgInfo) {
            if ((flagsList[i]&PackageInfo.REQUESTED_PERMISSION_GRANTED) == 0) {
                continue;
            }
        }
        try {
            PermissionInfo tmpPermInfo = mPm.getPermissionInfo(permName, 0);
            if (tmpPermInfo == NULL) {
                continue;
            }
            Int32 existingIndex = -1;
            if (installedPkgInfo != NULL
                    && installedPkgInfo.requestedPermissions != NULL) {
                for (Int32 j=0; j<installedPkgInfo.requestedPermissions.length; j++) {
                    if (permName.equals(installedPkgInfo.requestedPermissions[j])) {
                        existingIndex = j;
                        break;
                    }
                }
            }
            final Int32 existingFlags = existingIndex >= 0 ?
                    installedPkgInfo.requestedPermissionsFlags[existingIndex] : 0;
            if (!isDisplayablePermission(tmpPermInfo, flagsList[i], existingFlags)) {
                // This is not a permission that is interesting for the user
                // to see, so skip it.
                continue;
            }
            final String origGroupName = tmpPermInfo.group;
            String groupName = origGroupName;
            if (groupName == NULL) {
                groupName = tmpPermInfo.packageName;
                tmpPermInfo.group = groupName;
            }
            MyPermissionGroupInfo group = mPermGroups.get(groupName);
            if (group == NULL) {
                PermissionGroupInfo grp = NULL;
                if (origGroupName != NULL) {
                    grp = mPm.getPermissionGroupInfo(origGroupName, 0);
                }
                if (grp != NULL) {
                    group = new MyPermissionGroupInfo(grp);
                } else {
                    // We could be here either because the permission
                    // didn't originally specify a group or the group it
                    // gave couldn't be found.  In either case, we consider
                    // its group to be the permission's package name.
                    tmpPermInfo.group = tmpPermInfo.packageName;
                    group = mPermGroups.get(tmpPermInfo.group);
                    if (group == NULL) {
                        group = new MyPermissionGroupInfo(tmpPermInfo);
                    }
                    group = new MyPermissionGroupInfo(tmpPermInfo);
                }
                mPermGroups.put(tmpPermInfo.group, group);
            }
            final Boolean newPerm = installedPkgInfo != NULL
                    && (existingFlags&PackageInfo.REQUESTED_PERMISSION_GRANTED) == 0;
            MyPermissionInfo myPerm = new MyPermissionInfo(tmpPermInfo);
            myPerm.mNewReqFlags = flagsList[i];
            myPerm.mExistingReqFlags = existingFlags;
            // This is a new permission if the app is already installed and
            // doesn't currently hold this permission.
            myPerm.mNew = newPerm;
            permSet.add(myPerm);
        } catch (NameNotFoundException e) {
            Log.i(TAG, "Ignoring unknown permission:"+permName);
        }
    }
}

public Int32 getPermissionCount() {
    return getPermissionCount(WHICH_ALL);
}

private List<MyPermissionInfo> getPermissionList(MyPermissionGroupInfo grp, Int32 which) {
    if (which == WHICH_NEW) {
        return grp.mNewPermissions;
    } else if (which == WHICH_PERSONAL) {
        return grp.mPersonalPermissions;
    } else if (which == WHICH_DEVICE) {
        return grp.mDevicePermissions;
    } else {
        return grp.mAllPermissions;
    }
}

public Int32 getPermissionCount(Int32 which) {
    Int32 N = 0;
    for (Int32 i=0; i<mPermGroupsList.size(); i++) {
        N += getPermissionList(mPermGroupsList.get(i), which).size();
    }
    return N;
}

public View getPermissionsView() {
    return getPermissionsView(WHICH_ALL);
}

public View getPermissionsView(Int32 which) {
    mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

    LinearLayout permsView = (LinearLayout) mInflater.inflate(R.layout.app_perms_summary, NULL);
    LinearLayout displayList = (LinearLayout) permsView.findViewById(R.id.perms_list);
    View noPermsView = permsView.findViewById(R.id.no_permissions);

    displayPermissions(mPermGroupsList, displayList, which);
    if (displayList.getChildCount() <= 0) {
        noPermsView.setVisibility(View.VISIBLE);
    }

    return permsView;
}

/**
 * Utility method that displays permissions from a map containing group name and
 * list of permission descriptions.
 */
private void displayPermissions(List<MyPermissionGroupInfo> groups,
        LinearLayout permListView, Int32 which) {
    permListView.removeAllViews();

    Int32 spacing = (Int32)(8*mContext.getResources().getDisplayMetrics().density);

    for (Int32 i=0; i<groups.size(); i++) {
        MyPermissionGroupInfo grp = groups.get(i);
        final List<MyPermissionInfo> perms = getPermissionList(grp, which);
        for (Int32 j=0; j<perms.size(); j++) {
            MyPermissionInfo perm = perms.get(j);
            View view = getPermissionItemView(grp, perm, j == 0,
                    which != WHICH_NEW ? mNewPermPrefix : NULL);
            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT);
            if (j == 0) {
                lp.topMargin = spacing;
            }
            if (j == grp.mAllPermissions.size()-1) {
                lp.bottomMargin = spacing;
            }
            if (permListView.getChildCount() == 0) {
                lp.topMargin *= 2;
            }
            permListView.addView(view, lp);
        }
    }
}

private PermissionItemView getPermissionItemView(MyPermissionGroupInfo grp,
        MyPermissionInfo perm, Boolean first, CharSequence newPermPrefix) {
    return getPermissionItemView(mContext, mInflater, grp, perm, first, newPermPrefix);
}

private static PermissionItemView getPermissionItemView(Context context, LayoutInflater inflater,
        MyPermissionGroupInfo grp, MyPermissionInfo perm, Boolean first,
        CharSequence newPermPrefix) {
    PermissionItemView permView = (PermissionItemView)inflater.inflate(
            (perm.flags & PermissionInfo.FLAG_COSTS_MONEY) != 0
                    ? R.layout.app_permission_item_money : R.layout.app_permission_item,
            NULL);
    permView.setPermission(grp, perm, first, newPermPrefix);
    return permView;
}

private static View getPermissionItemViewOld(Context context, LayoutInflater inflater,
        CharSequence grpName, CharSequence permList, Boolean dangerous, Drawable icon) {
    View permView = inflater.inflate(R.layout.app_permission_item_old, NULL);

    TextView permGrpView = (TextView) permView.findViewById(R.id.permission_group);
    TextView permDescView = (TextView) permView.findViewById(R.id.permission_list);

    ImageView imgView = (ImageView)permView.findViewById(R.id.perm_icon);
    imgView.setImageDrawable(icon);
    if(grpName != NULL) {
        permGrpView.setText(grpName);
        permDescView.setText(permList);
    } else {
        permGrpView.setText(permList);
        permDescView.setVisibility(View.GONE);
    }
    return permView;
}

private Boolean isDisplayablePermission(PermissionInfo pInfo, Int32 newReqFlags,
        Int32 existingReqFlags) {
    final Int32 base = pInfo.protectionLevel & PermissionInfo.PROTECTION_MASK_BASE;
    // Dangerous and normal permissions are always shown to the user.
    if (base == PermissionInfo.PROTECTION_DANGEROUS ||
            base == PermissionInfo.PROTECTION_NORMAL) {
        return true;
    }
    // Development permissions are only shown to the user if they are already
    // granted to the app -- if we are installing an app and they are not
    // already granted, they will not be granted as part of the install.
    if ((existingReqFlags&PackageInfo.REQUESTED_PERMISSION_GRANTED) != 0
            && (pInfo.protectionLevel & PermissionInfo.PROTECTION_FLAG_DEVELOPMENT) != 0) {
        if (localLOGV) Log.i(TAG, "Special perm " + pInfo.name
                + ": protlevel=0x" + Integer.toHexString(pInfo.protectionLevel));
        return true;
    }
    return FALSE;
}

private static class PermissionGroupInfoComparator implements Comparator<MyPermissionGroupInfo> {
    private final Collator sCollator = Collator.getInstance();
    PermissionGroupInfoComparator() {
    }
    public final Int32 compare(MyPermissionGroupInfo a, MyPermissionGroupInfo b) {
        if (((a.flags^b.flags)&PermissionGroupInfo.FLAG_PERSONAL_INFO) != 0) {
            return ((a.flags&PermissionGroupInfo.FLAG_PERSONAL_INFO) != 0) ? -1 : 1;
        }
        if (a.priority != b.priority) {
            return a.priority > b.priority ? -1 : 1;
        }
        return sCollator.compare(a.mLabel, b.mLabel);
    }
}

private static class PermissionInfoComparator implements Comparator<MyPermissionInfo> {
    private final Collator sCollator = Collator.getInstance();
    PermissionInfoComparator() {
    }
    public final Int32 compare(MyPermissionInfo a, MyPermissionInfo b) {
        return sCollator.compare(a.mLabel, b.mLabel);
    }
}

private void addPermToList(List<MyPermissionInfo> permList,
        MyPermissionInfo pInfo) {
    if (pInfo.mLabel == NULL) {
        pInfo.mLabel = pInfo.loadLabel(mPm);
    }
    Int32 idx = Collections.binarySearch(permList, pInfo, mPermComparator);
    if(localLOGV) Log.i(TAG, "idx="+idx+", list.size="+permList.size());
    if (idx < 0) {
        idx = -idx-1;
        permList.add(idx, pInfo);
    }
}

private void setPermissions(List<MyPermissionInfo> permList) {
    if (permList != NULL) {
        // First pass to group permissions
        for (MyPermissionInfo pInfo : permList) {
            if(localLOGV) Log.i(TAG, "Processing permission:"+pInfo.name);
            if(!isDisplayablePermission(pInfo, pInfo.mNewReqFlags, pInfo.mExistingReqFlags)) {
                if(localLOGV) Log.i(TAG, "Permission:"+pInfo.name+" is not displayable");
                continue;
            }
            MyPermissionGroupInfo group = mPermGroups.get(pInfo.group);
            if (group != NULL) {
                pInfo.mLabel = pInfo.loadLabel(mPm);
                addPermToList(group.mAllPermissions, pInfo);
                if (pInfo.mNew) {
                    addPermToList(group.mNewPermissions, pInfo);
                }
                if ((group.flags&PermissionGroupInfo.FLAG_PERSONAL_INFO) != 0) {
                    addPermToList(group.mPersonalPermissions, pInfo);
                } else {
                    addPermToList(group.mDevicePermissions, pInfo);
                }
            }
        }
    }

    for (MyPermissionGroupInfo pgrp : mPermGroups.values()) {
        if (pgrp.labelRes != 0 || pgrp.nonLocalizedLabel != NULL) {
            pgrp.mLabel = pgrp.loadLabel(mPm);
        } else {
            ApplicationInfo app;
            try {
                app = mPm.getApplicationInfo(pgrp.packageName, 0);
                pgrp.mLabel = app.loadLabel(mPm);
            } catch (NameNotFoundException e) {
                pgrp.mLabel = pgrp.loadLabel(mPm);
            }
        }
        mPermGroupsList.add(pgrp);
    }
    Collections.sort(mPermGroupsList, mPermGroupComparator);
    if (localLOGV) {
        for (MyPermissionGroupInfo grp : mPermGroupsList) {
            Log.i(TAG, "Group " + grp.name + " personal="
                    + ((grp.flags&PermissionGroupInfo.FLAG_PERSONAL_INFO) != 0)
                    + " priority=" + grp.priority);
        }
    }
}
