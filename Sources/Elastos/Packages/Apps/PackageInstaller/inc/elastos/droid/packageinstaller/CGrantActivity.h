//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CGRANTACTIVITY_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CGRANTACTIVITY_H__

#include "_Elastos_Droid_PackageInstaller_CGrantActivity.h"
#include "elastos/droid/app/Activity.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAppSecurityPermissions;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

/*
  * The activity which is responsible for asking the user to grant permissions
  * to applications.
  */
CarClass(CGrantActivity)
    , public Activity
    , public IViewOnClickListener
    , public IGrantActivity
{
private:
    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        ViewOnClickListener(
            /* [in] */ CGrantActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
          /* [in] */ IView* v);

    private:
        CGrantActivity* mHost;
    };

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
    AutoPtr< ArrayOf<String> > mRequested_permissions;
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CGRANTACTIVITY_H__

