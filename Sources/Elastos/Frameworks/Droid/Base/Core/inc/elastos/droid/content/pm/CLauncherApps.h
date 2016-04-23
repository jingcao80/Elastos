
#ifndef __ELASTOS_DROID_CONTENT_PM_CLAUNCHER_APPS_INFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CLAUNCHER_APPS_INFO_H__

#include "_Elastos_Droid_Content_Pm_CLauncherApps.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Content::IComponentName;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Class for retrieving a list of launchable activities for the current user and any associated
 * managed profiles. This is mainly for use by launchers. Apps can be queried for each user profile.
 * Since the PackageManager will not deliver package broadcasts for other profiles, you can register
 * for package changes here.
 * <p>
 * To watch for managed profiles being added or removed, register for the following broadcasts:
 * {@link Intent#ACTION_MANAGED_PROFILE_ADDED} and {@link Intent#ACTION_MANAGED_PROFILE_REMOVED}.
 * <p>
 * You can retrieve the list of profiles associated with this user with
 * {@link UserManager#getUserProfiles()}.
 */
CarClass(CLauncherApps)
    , public Object
    , public ILauncherApps
{
private:
    class CallbackInfo : public Object {
    public:
        CallbackInfo() : mReplacing(FALSE) {}

        AutoPtr<ArrayOf<String> > mPackageNames;
        String mPackageName;
        Boolean mReplacing;
        AutoPtr<IUserHandle> mUser;
    };

    class CallbackMessageHandler : public Handler {
    public:
        TO_STRING_IMPL("CLauncherApps::CallbackMessageHandler")

        CallbackMessageHandler();

        CARAPI constructor(
            /* [in] */ ILooper* looper,
            /* [in] */ ILauncherAppsCallback* callback);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI PostOnPackageAdded(
            /* [in] */ const String& packageName,
            /* [in] */ IUserHandle* user);

        CARAPI PostOnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ IUserHandle* user);

        CARAPI PostOnPackageChanged(
            /* [in] */ const String& packageName,
            /* [in] */ IUserHandle* user);

        CARAPI PostOnPackagesAvailable(
            /* [in] */ ArrayOf<String>* packageName,
            /* [in] */ IUserHandle* user,
            /* [in] */ Boolean replacing);

        CARAPI PostOnPackagesUnavailable(
            /* [in] */ ArrayOf<String>* packageName,
            /* [in] */ IUserHandle* user,
            /* [in] */ Boolean replacing);

    private:
        friend class CLauncherApps;

        static const Int32 MSG_ADDED;// = 1;
        static const Int32 MSG_REMOVED;// = 2;
        static const Int32 MSG_CHANGED;// = 3;
        static const Int32 MSG_AVAILABLE;// = 4;
        static const Int32 MSG_UNAVAILABLE;// = 5;

        AutoPtr<ILauncherAppsCallback> mCallback;
    };

    class MyOnAppsChangedListener
        : public Object
        , public IOnAppsChangedListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnAppsChangedListener(
            /* [in] */ IWeakReference* weakHost);

        CARAPI OnPackageRemoved(
            /* [in] */ IUserHandle* user,
            /* [in] */ const String& packageName);

        CARAPI OnPackageAdded(
            /* [in] */ IUserHandle* user,
            /* [in] */ const String& packageName);

        CARAPI OnPackageChanged(
            /* [in] */ IUserHandle* user,
            /* [in] */ const String& packageName);

        CARAPI OnPackagesAvailable(
            /* [in] */ IUserHandle* user,
            /* [in] */ ArrayOf<String>* packageNames,
            /* [in] */ Boolean replacing);

        CARAPI OnPackagesUnavailable(
            /* [in] */ IUserHandle* user,
            /* [in] */ ArrayOf<String>* packageNames,
            /* [in] */ Boolean replacing);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLauncherApps();

    virtual ~CLauncherApps();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IILauncherApps* service);

    /**
     * Retrieves a list of launchable activities that match {@link Intent#ACTION_MAIN} and
     * {@link Intent#CATEGORY_LAUNCHER}, for a specified user.
     *
     * @param packageName The specific package to query. If null, it checks all installed packages
     *            in the profile.
     * @param user The UserHandle of the profile.
     * @return List of launchable activities. Can be an empty list but will not be null.
     */
    CARAPI GetActivityList(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle* user,
        /* [out] */ IList** infos);

    static AutoPtr<IComponentName> GetComponentName(
        /* [in] */ IResolveInfo* info);

    /**
     * Returns the activity info for a given intent and user handle, if it resolves. Otherwise it
     * returns null.
     *
     * @param intent The intent to find a match for.
     * @param user The profile to look in for a match.
     * @return An activity info object if there is a match.
     */
    CARAPI ResolveActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [out] */ ILauncherActivityInfo** info);

    /**
     * Starts a Main activity in the specified profile.
     *
     * @param component The ComponentName of the activity to launch
     * @param user The UserHandle of the profile
     * @param sourceBounds The Rect containing the source bounds of the clicked icon
     * @param opts Options to pass to startActivity
     */
    CARAPI StartMainActivity(
        /* [in] */ IComponentName* component,
        /* [in] */ IUserHandle* user,
        /* [in] */ IRect* sourceBounds,
        /* [in] */ IBundle* opts);

    /**
     * Starts the settings activity to show the application details for a
     * package in the specified profile.
     *
     * @param component The ComponentName of the package to launch settings for.
     * @param user The UserHandle of the profile
     * @param sourceBounds The Rect containing the source bounds of the clicked icon
     * @param opts Options to pass to startActivity
     */
    CARAPI StartAppDetailsActivity(
        /* [in] */ IComponentName* component,
        /* [in] */ IUserHandle* user,
        /* [in] */ IRect* sourceBounds,
        /* [in] */ IBundle* opts);

    /**
     * Checks if the package is installed and enabled for a profile.
     *
     * @param packageName The package to check.
     * @param user The UserHandle of the profile.
     *
     * @return true if the package exists and is enabled.
     */
    CARAPI IsPackageEnabled(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    /**
     * Checks if the activity exists and it enabled for a profile.
     *
     * @param component The activity to check.
     * @param user The UserHandle of the profile.
     *
     * @return true if the activity exists and is enabled.
     */
    CARAPI IsActivityEnabled(
        /* [in] */ IComponentName* component,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    /**
     * Registers a callback for changes to packages in current and managed profiles.
     *
     * @param callback The callback to register.
     */
    CARAPI RegisterCallback(
        /* [in] */ ILauncherAppsCallback* cb);

    /**
     * Registers a callback for changes to packages in current and managed profiles.
     *
     * @param callback The callback to register.
     * @param handler that should be used to post callbacks on, may be null.
     */
    CARAPI RegisterCallback(
        /* [in] */ ILauncherAppsCallback* cb,
        /* [in] */ IHandler* handler);

    /**
     * Unregisters a callback that was previously registered.
     *
     * @param callback The callback to unregister.
     * @see #registerCallback(Callback)
     */
    CARAPI UnregisterCallback(
        /* [in] */ ILauncherAppsCallback* cb);

    /**
     * TODO Remove after 2014-09-22
     * @hide
     */
    CARAPI AddCallback(
        /* [in] */ ILauncherAppsCallback* cb);

    /**
     * TODO Remove after 2014-09-22
     * @hide
     */
    CARAPI RemoveCallback(
        /* [in] */ ILauncherAppsCallback* cb);

private:
    CARAPI RemoveCallbackLocked(
        /* [in] */ ILauncherAppsCallback* cb);

    CARAPI AddCallbackLocked(
        /* [in] */ ILauncherAppsCallback* cb,
        /* [in] */ IHandler* handler);

private:
    friend class MyOnAppsChangedListener;

    static const String TAG;// = "LauncherApps";
    static const Boolean DEBUG;// = false;

    AutoPtr<IContext> mContext;
    AutoPtr<IILauncherApps> mService;
    AutoPtr<IPackageManager> mPm;

    AutoPtr<IList> mCallbacks;
    // private List<CallbackMessageHandler> mCallbacks
    //         = new ArrayList<CallbackMessageHandler>();

    AutoPtr<IOnAppsChangedListener> mAppsChangedListener;
};


}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CLAUNCHER_APPS_INFO_H__
