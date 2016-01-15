
#ifndef __APPSECURITYPERMISSIONS_H__
#define __APPSECURITYPERMISSIONS_H__

/**
 * This class contains the SecurityPermissions view implementation.
 * Initially the package's advanced or dangerous security permissions
 * are displayed under categorized
 * groups. Clicking on the additional permissions presents
 * extended information consisting of all groups and permissions.
 * To use this view define a LinearLayout or any ViewGroup and add this
 * view by instantiating AppSecurityPermissions and invoking getPermissionsView.
 *
 * {@hide}
 */
class AppSecurityPermissions
{
    class MyPermissionGroupInfo : public PermissionGroupInfo
    {
        AutoPtr<ICharSequence> mLabel;

        ArrayList<MyPermissionInfo> mNewPermissions = new ArrayList<MyPermissionInfo>();
        ArrayList<MyPermissionInfo> mPersonalPermissions = new ArrayList<MyPermissionInfo>();
        ArrayList<MyPermissionInfo> mDevicePermissions = new ArrayList<MyPermissionInfo>();
        ArrayList<MyPermissionInfo> mAllPermissions = new ArrayList<MyPermissionInfo>();

        MyPermissionGroupInfo(
            /* [in] */ PermissionInfo perm);

        MyPermissionGroupInfo(
            /* [in] */ PermissionGroupInfo info);

        public Drawable loadGroupIcon(
            /* [in] */ PackageManager pm);
    };

    class MyPermissionInfo : public PermissionInfo
    {
        AutoPtr<ICharSequence> mLabel;

        /**
         * PackageInfo.requestedPermissionsFlags for the new package being installed.
         */
        Int32 mNewReqFlags;

        /**
         * PackageInfo.requestedPermissionsFlags for the currently installed
         * package, if it is installed.
         */
        Int32 mExistingReqFlags;

        /**
         * True if this should be considered a new permission.
         */
        Boolean mNew;

        MyPermissionInfo();

        MyPermissionInfo(
            /* [in] */ PermissionInfo* info);

        MyPermissionInfo(
            /* [in] */ MyPermissionInfo* info);
    };


public:
    public static class PermissionItemView extends LinearLayout implements View.OnClickListener
    {
    public:
        PermissionItemView(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI_(void) SetPermission(
            /* [in] */ MyPermissionGroupInfo grp,
            /* [in] */ MyPermissionInfo perm,
            /* [in] */ Boolean first,
            /* [in] */ ICharSequence* newPermPrefix);

        //@Override
        CARAPI_(void) OnClick(
            /* [in] */ IView* v);

    protected:
        //@Override
        virtual CARAPI OnDetachedFromWindow();

    private:
        MyPermissionGroupInfo mGroup;
        MyPermissionInfo mPerm;
        AlertDialog mDialog;
    };

    AppSecurityPermissions(
        /* [in] */ IContext* context,
        /* [in] */ List<PermissionInfo> permList);

    AppSecurityPermissions(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName);

    AppSecurityPermissions(
        /* [in] */ IContext* context,
        /* [in] */ IPackageInfo* info);


    /**
     * Utility to retrieve a view displaying a single permission.  This provides
     * the old UI layout for permissions; it is only here for the device admin
     * settings to continue to use.
     */
    static CARAPI_(AutoPtr<IView>) GetPermissionItemView(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* grpName,
        /* [in] */ ICharSequence* description,
        /* [in] */ Boolean dangerous);

    CARAPI_(AutoPtr<IPackageInfo>) GetInstalledPackageInfo();

    CARAPI_(Int32) GetPermissionCount();

    CARAPI_(Int32) GetPermissionCount(
        /* [in] */ Int32 which);

    CARAPI_(AutoPtr<IView>) GetPermissionsView();

    CARAPI_(AutoPtr<IView>) GetPermissionsView(
        /* [in] */ Int32 which);


private:
    CARAPI_(void) LoadResources();

    CARAPI_(void) GetAllUsedPermissions(
        /* [in] */ Int32 sharedUid,
        /* [in] */ Set<MyPermissionInfo> permSet);

    CARAPI_(void) GetPermissionsForPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Set<MyPermissionInfo> permSet);

    CARAPI_(void) ExtractPerms(
        /* [in] */ PackageInfo info,
        /* [in] */ Set<MyPermissionInfo> permSet,
        /* [in] */ PackageInfo installedPkgInfo);


    CARAPI_(List<MyPermissionInfo>) GetPermissionList(
        /* [in] */ MyPermissionGroupInfo grp,
        /* [in] */ Int32 which);


    /**
     * Utility method that displays permissions from a map containing group name and
     * list of permission descriptions.
     */
    CARAPI_(void) DisplayPermissions(
        /* [in] */ List<MyPermissionGroupInfo> groups,
        /* [in] */ ILinearLayout* permListView,
        /* [in] */ Int32 which);

    CARAPI_(AutoPtr<IPermissionItemView>) GetPermissionItemView(
        /* [in] */ MyPermissionGroupInfo grp,
        /* [in] */ MyPermissionInfo perm,
        /* [in] */ Boolean first,
        /* [in] */ ICharSequence* newPermPrefix);

    CARAPI_(AutoPtr<IPermissionItemView>) GetPermissionItemView(
        /* [in] */ IContext* context,
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ MyPermissionGroupInfo grp,
        /* [in] */ MyPermissionInfo perm,
        /* [in] */ Boolean first,
        /* [in] */ ICharSequence* newPermPrefix);

    CARAPI_(AutoPtr<IView>) GetPermissionItemViewOld(
        /* [in] */ IContext* context,
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ ICharSequence* grpName,
        /* [in] */ ICharSequence* permList,
        /* [in] */ Boolean dangerous,
        /* [in] */ IDrawable* icon);

    CARAPI_(Boolean) IsDisplayablePermission(
        /* [in] */ PermissionInfo pInfo,
        /* [in] */ Int32 newReqFlags,
        /* [in] */ Int32 existingReqFlags);

    class PermissionGroupInfoComparator
    {
        PermissionGroupInfoComparator();

        CARAPI_(Int32) Compare(
            /* [in] */ MyPermissionGroupInfo a,
            /* [in] */ MyPermissionGroupInfo b);

        Collator sCollator = Collator.getInstance();
    };

    class PermissionInfoComparator
    {
    public:
        PermissionInfoComparator();

        CARAPI_(Int32) compare(
            /* [in] */ MyPermissionInfo a,
            /* [in] */ MyPermissionInfo b);

        Collator sCollator = Collator.getInstance();
    };

    CARAPI_(void) AddPermToList(
        /* [in] */ List<MyPermissionInfo> permList,
        /* [in] */ MyPermissionInfo pInfo);

    CARAPI_(void) SetPermissions(
        /* [in] */ List<MyPermissionInfo> permList);

public:
    static const Int32 WHICH_PERSONAL = 1<<0;
    static const Int32 WHICH_DEVICE = 1<<1;
    static const Int32 WHICH_NEW = 1<<2;
    static const Int32 WHICH_ALL = 0xffff;

private:
    static String TAG = "AppSecurityPermissions";
    static Boolean localLOGV = FALSE;
    AutoPtr<IContext> mContext;
    AutoPtr<ILayoutInflater> mInflater;
    PackageManager mPm;
    PackageInfo mInstalledPackageInfo;
    Map<String, MyPermissionGroupInfo> mPermGroups
        = new HashMap<String, MyPermissionGroupInfo>();
    List<MyPermissionGroupInfo> mPermGroupsList
        = new ArrayList<MyPermissionGroupInfo>();
    PermissionGroupInfoComparator mPermGroupComparator;
    PermissionInfoComparator mPermComparator;
    List<MyPermissionInfo> mPermsList;
    AutoPtr<ICharSequence> mNewPermPrefix;
    AutoPtr<IDrawable> mNormalIcon;
    AutoPtr<IDrawable> mDangerousIcon;
};

#endif
