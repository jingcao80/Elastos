
#ifndef __ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONS_H__
#define __ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONS_H__

#include "_Elastos_Droid_Widget_CAppSecurityPermissions.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/content/pm/PermissionInfo.h"
#include "elastos/droid/content/pm/PermissionGroupInfo.h"
#include <elastos/utility/etl/HashMap.h>
#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::PermissionGroupInfo;
using Elastos::Droid::Content::Pm::IPermissionGroupInfo;
using Elastos::Droid::Content::Pm::PermissionInfo;
using Elastos::Droid::Content::Pm::IPermissionInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Core::IComparator;
using Elastos::Text::ICollator;
using Elastos::Utility::IList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

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
CarClass(CAppSecurityPermissions)
    , public Object
    , public IAppSecurityPermissions
{
private:
    class MyPermissionGroupInfo : public PermissionGroupInfo
    {
    public:
        MyPermissionGroupInfo(
            /* [in] */ PermissionInfo* perm);

        MyPermissionGroupInfo(
            /* [in] */ PermissionGroupInfo* info);

        CARAPI_(AutoPtr<IDrawable>) LoadGroupIcon(
            /* [in] */ IPackageManager* pm);

    public:
        AutoPtr<ICharSequence> mLabel;

        AutoPtr<IList> mNewPermissions; // List<MyPermissionInfo>
        AutoPtr<IList> mPersonalPermissions; // List<MyPermissionInfo>
        AutoPtr<IList> mDevicePermissions; // List<MyPermissionInfo>
        AutoPtr<IList> mAllPermissions; // List<MyPermissionInfo>
    };

    class MyPermissionInfo : public PermissionInfo
    {
    public:
        MyPermissionInfo(
            /* [in] */ PermissionInfo* info);

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
    };

public:
    class PermissionItemView
        : public LinearLayout
        , public IPermissionItemView
    {
    private:
        class ViewOnClickListener
            : public Object
            , public IViewOnClickListener
        {
        public:
            ViewOnClickListener(
                /* [in] */ PermissionItemView* host);

            CAR_INTERFACE_DECL()

            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            PermissionItemView* mHost;
        };

        class DialogInterfaceOnClickListener
            : public Object
            , public IDialogInterfaceOnClickListener
        {
        public:
            DialogInterfaceOnClickListener(
                /* [in] */ PermissionItemView* host);

            CAR_INTERFACE_DECL()

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);

        private:
            PermissionItemView* mHost;
        };

    public:
        PermissionItemView();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI SetPermission(
            /* [in] */ MyPermissionGroupInfo* grp,
            /* [in] */ MyPermissionInfo* perm,
            /* [in] */ Boolean first,
            /* [in] */ ICharSequence* newPermPrefix,
            /* [in] */ const String& packageName,
            /* [in] */ Boolean showRevokeUI);

        CARAPI OnClick(
            /* [in] */ IView* v);

    protected:
        CARAPI OnDetachedFromWindow();

    private:
        CARAPI_(void) AddRevokeUIIfNecessary(
            /* [in] */ IAlertDialogBuilder* builder);

    public:
        AutoPtr<MyPermissionGroupInfo> mGroup;
        AutoPtr<MyPermissionInfo> mPerm;
        AutoPtr<IAlertDialog> mDialog;

    private:
        Boolean mShowRevokeUI;
        String mPackageName;
    };

private:
    class PermissionGroupInfoComparator
        : public Object
        , public IComparator
    {
    public:
        PermissionGroupInfoComparator();

        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* _a,
            /* [in] */ IInterface* _b,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> sCollator;
    };

    class PermissionInfoComparator
        : public Object
        , public IComparator
    {
    public:
        PermissionInfoComparator();

        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* _a,
            /* [in] */ IInterface* _b,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> sCollator;
    };

public:
    CAppSecurityPermissions();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName);

    CARAPI constructor(
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

    CARAPI GetPermissionCount(
        /* [out] */ Int32* count);

    CARAPI GetPermissionCount(
        /* [in] */ Int32 which,
        /* [out] */ Int32* count);

    CARAPI GetPermissionsView(
        /* [out] */ IView** view);

    CARAPI GetPermissionsViewWithRevokeButtons(
        /* [out] */ IView** view);

    CARAPI GetPermissionsView(
        /* [in] */ Int32 which,
        /* [out] */ IView** view);

private:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(void) GetAllUsedPermissions(
        /* [in] */ Int32 sharedUid,
        /* [in] */ ISet* permSet);

    CARAPI_(void) GetPermissionsForPackage(
        /* [in] */ const String& packageName,
        /* [in] */ ISet* permSet);

    CARAPI_(void) ExtractPerms(
        /* [in] */ IPackageInfo* info,
        /* [in] */ ISet* permSet,
        /* [in] */ IPackageInfo* installedPkgInfo);

    CARAPI_(AutoPtr<IList>) GetPermissionList(
        /* [in] */ MyPermissionGroupInfo* grp,
        /* [in] */ Int32 which);

    CARAPI GetPermissionsView(
        /* [in] */ Int32 which,
        /* [in] */ Boolean showRevokeUI,
        /* [out] */ IView** view);

    /**
     * Utility method that displays permissions from a map containing group name and
     * list of permission descriptions.
     */
    CARAPI_(void) DisplayPermissions(
        /* [in] */ IList* groups,
        /* [in] */ ILinearLayout* _permListView,
        /* [in] */ Int32 which,
        /* [in] */ Boolean showRevokeUI);

    CARAPI_(AutoPtr<PermissionItemView>) GetPermissionItemView(
        /* [in] */ MyPermissionGroupInfo* grp,
        /* [in] */ MyPermissionInfo* perm,
        /* [in] */ Boolean first,
        /* [in] */ ICharSequence* newPermPrefix,
        /* [in] */ Boolean showRevokeUI);

    static CARAPI_(AutoPtr<PermissionItemView>) GetPermissionItemView(
        /* [in] */ IContext* context,
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ MyPermissionGroupInfo* grp,
        /* [in] */ MyPermissionInfo* perm,
        /* [in] */ Boolean first,
        /* [in] */ ICharSequence* newPermPrefix,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean showRevokeUI);

    static CARAPI_(AutoPtr<IView>) GetPermissionItemViewOld(
        /* [in] */ IContext* context,
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ ICharSequence* grpName,
        /* [in] */ ICharSequence* permList,
        /* [in] */ Boolean dangerous,
        /* [in] */ IDrawable* icon);

    CARAPI_(Boolean) IsDisplayablePermission(
        /* [in] */ IPermissionInfo* pInfo,
        /* [in] */ Int32 newReqFlags,
        /* [in] */ Int32 existingReqFlags);

    CARAPI_(void) AddPermToList(
        /* [in] */ IList* permList,
        /* [in] */ MyPermissionInfo* pInfo);

    CARAPI_(void) SetPermissions(
        /* [in] */ IList* permList);

private:
    static const String TAG;
    static const Boolean localLOGV;

    AutoPtr<IContext> mContext;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IPackageManager> mPm;
    HashMap<String, AutoPtr<MyPermissionGroupInfo> > mPermGroups;
    AutoPtr<IList> mPermGroupsList; // List<MyPermissionGroupInfo>
    AutoPtr<PermissionGroupInfoComparator> mPermGroupComparator;
    AutoPtr<PermissionInfoComparator> mPermComparator;
    AutoPtr<IList> mPermsList; // List<MyPermissionInfo>
    AutoPtr<ICharSequence> mNewPermPrefix;
    String mPackageName;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONS_H__
