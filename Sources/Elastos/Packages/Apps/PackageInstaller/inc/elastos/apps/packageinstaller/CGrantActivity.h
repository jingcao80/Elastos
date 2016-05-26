
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CGRANTACTIVITY_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CGRANTACTIVITY_H__

//#include "elastos/droid/app/Activity.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include "_Elastos_Apps_PackageInstaller_CGrantActivity.h"
#include "elastos/core/Object.h"

//using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAppSecurityPermissions;
using Elastos::Droid::Widget::IButton;
using Elastos::Core::Object;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

/*
  * The activity which is responsible for asking the user to grant permissions
  * to applications.
  */
CarClass(CGrantActivity)
    , public Object // Activity
    , public IViewOnClickListener
    , public IGrantActivity
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CGrantActivity();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* v);

private:
    /**
      * Returns a PackageInfo object representing the results of adding all the permissions
      * in {@code requested_permissions} to {@code mRequestingPackage}. This is the package
      * permissions the user will have if they accept the grant request.
      */
    CARAPI_(AutoPtr<IPackageInfo>) GetUpdatedPackageInfo();

    CARAPI_(AutoPtr<IView>) GetPermissionList(
        /* [in] */ IAppSecurityPermissions* perms);

    /**
      * Return an array of permissions requested by the caller, filtered to exclude
      * irrelevant or otherwise malicious permission requests from untrusted callers.
      */
    CARAPI_(AutoPtr< ArrayOf<String> >) GetRequestedPermissions();

    /**
      * Remove any permissions in {@code permissions} which are not present
      * in {@code mRequestingPackage} and return the result. We also filter out
      * permissions which are required by {@code mRequestingPackage}, and permissions
      * which have already been granted to {@code mRequestingPackage}, as those permissions
      * are useless to change.
      */
    CARAPI_(AutoPtr< ArrayOf<String> >) KeepRequestingPackagePermissions(
        /* [in] */ ArrayOf<String>* permissions);

    /**
      * Filter the permissions in {@code permissions}, keeping only the NORMAL or DANGEROUS
      * permissions.
      *
      * @param permissions the permissions to filter
      * @return A subset of {@code permissions} with only the
      *     NORMAL or DANGEROUS permissions kept
      */
    CARAPI_(AutoPtr< ArrayOf<String> >) KeepNormalDangerousPermissions(
        /* [in] */ ArrayOf<String>* permissions);

private:
    AutoPtr<IButton> mOk;
    AutoPtr<IButton> mCancel;
    AutoPtr<IPackageManager> mPm;
    String mRequestingPackage;
    AutoPtr< ArrayOf<String> > requested_permissions;
};

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CGRANTACTIVITY_H__

