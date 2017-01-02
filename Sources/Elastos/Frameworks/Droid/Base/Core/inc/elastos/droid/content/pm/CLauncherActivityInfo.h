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

#ifndef __ELASTOS_DROID_CONTENT_PM_CLAUNCHER_ACTIVITY_INFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CLAUNCHER_ACTIVITY_INFO_H__

#include "_Elastos_Droid_Content_Pm_CLauncherActivityInfo.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::ICharSequence;


namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * A representation of an activity that can belong to this user or a managed
 * profile associated with this user. It can be used to query the label, icon
 * and badged icon for the activity.
 */
CarClass(CLauncherActivityInfo)
    , public Object
    , public ILauncherActivityInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLauncherActivityInfo();

    virtual ~CLauncherActivityInfo();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* info,
        /* [in] */ IUserHandle* user,
        /* [in] */ Int64 firstInstallTime);

    /**
     * Returns the component name of this activity.
     *
     * @return ComponentName of the activity
     */
    CARAPI GetComponentName(
        /* [out] */ IComponentName** name);

    /**
     * Returns the user handle of the user profile that this activity belongs to. In order to
     * persist the identity of the profile, do not store the UserHandle. Instead retrieve its
     * serial number from UserManager. You can convert the serial number back to a UserHandle
     * for later use.
     *
     * @see UserManager#getSerialNumberForUser(UserHandle)
     * @see UserManager#getUserForSerialNumber(long)
     *
     * @return The UserHandle of the profile.
     */
    CARAPI GetUser(
        /* [out] */ IUserHandle** handle);

    /**
     * Retrieves the label for the activity.
     *
     * @return The label for the activity.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** seq);

    /**
     * Returns the icon for this activity, without any badging for the profile.
     * @param density The preferred density of the icon, zero for default density. Use
     * density DPI values from {@link DisplayMetrics}.
     * @see #getBadgedIcon(int)
     * @see DisplayMetrics
     * @return The drawable associated with the activity
     */
    CARAPI GetIcon(
        /* [in] */ Int32 density,
        /* [out] */ IDrawable** drawable);

    /**
     * Returns the application flags from the ApplicationInfo of the activity.
     *
     * @return Application flags
     * @hide remove before shipping
     */
    CARAPI GetApplicationFlags(
        /* [out] */ Int32* result);

    /**
     * Returns the application info for the appliction this activity belongs to.
     * @return
     */
    CARAPI GetApplicationInfo(
        /* [out] */ IApplicationInfo** info);

    /**
     * Returns the time at which the package was first installed.
     *
     * @return The time of installation of the package, in milliseconds.
     */
    CARAPI GetFirstInstallTime(
        /* [out] */ Int64* time);

    /**
     * Returns the name for the acitivty from  android:name in the manifest.
     * @return the name from android:name for the acitivity.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the activity icon with badging appropriate for the profile.
     * @param density Optional density for the icon, or 0 to use the default density. Use
     * {@link DisplayMetrics} for DPI values.
     * @see DisplayMetrics
     * @return A badged icon for the activity.
     */
    CARAPI GetBadgedIcon(
        /* [in] */ Int32 density,
        /* [out] */ IDrawable** drawable);

private:
    static const String TAG;// = "LauncherActivityInfo";

    AutoPtr<IPackageManager> mPm;

    AutoPtr<IActivityInfo> mActivityInfo;
    AutoPtr<IComponentName> mComponentName;
    AutoPtr<IUserHandle> mUser;
    Int64 mFirstInstallTime;
};


}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CLAUNCHER_ACTIVITY_INFO_H__
