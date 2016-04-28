
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_MANAGEDSERVICES_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_MANAGEDSERVICES_H__

#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

// from class NotificationManagerService;
class DumpFilter
    : public Object
{
public:
    DumpFilter();

    ~DumpFilter();

    static AutoPtr<DumpFilter> ParseFromArguments(
        /* [in] */ ArrayOf<String>* args);

    CARAPI_(Boolean) Matches(
        /* [in] */ IStatusBarNotification* sbn);

    CARAPI_(Boolean) Matches(
        /* [in] */ IComponentName* component);

    CARAPI_(Boolean) Matches(
        /* [in] */ const String& pkg);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    String mPkgFilter;
    Boolean mZen;
};

/**
 * Manages the lifecycle of application-provided services bound by system server.
 *
 * Services managed by this helper must have:
 *  - An associated system settings value with a list of enabled component names.
 *  - A well-known action for services to use in their intent-filter.
 *  - A system permission for services to require in order to ensure system has exclusive binding.
 *  - A settings page for user configuration of enabled services, and associated intent action.
 *  - A remote interface definition (aidl) provided by the service used for communication.
 */
class ManagedServices
    : public Object
{
    friend class ManagedServiceInfo;
public:
    class ManagedServiceInfo
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL();

        ManagedServiceInfo(
            /* [in] */ ManagedServices* host,
            /* [in] */ IInterface* service,
            /* [in] */ IComponentName* component,
            /* [in] */ Int32 userid,
            /* [in] */ Boolean isSystem,
            /* [in] */ IServiceConnection* connection,
            /* [in] */ Int32 targetSdkVersion);

        ~ManagedServiceInfo();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI_(Boolean) EnabledAndUserMatches(
            /* [in] */ Int32 nid);

        CARAPI_(Boolean) SupportsProfiles();

        // @Override
        CARAPI ProxyDied();

        /** convenience method for looking in mEnabledServicesForCurrentProfiles */
        CARAPI_(Boolean) IsEnabledForCurrentProfiles();

    public:
        AutoPtr<IInterface> mService;
        AutoPtr<IComponentName> mComponent;
        Int32 mUserid;
        Boolean mIsSystem;
        AutoPtr<IServiceConnection> mConnection;
        Int32 mTargetSdkVersion;

    private:
        ManagedServices* mHost;
    };

    class UserProfiles
        : public Object
    {
    public:
        UserProfiles();

        ~UserProfiles();

        CARAPI_(void) UpdateCache(
            /* [in] */ IContext* context);

        CARAPI_(AutoPtr< ArrayOf<Int32> >) GetCurrentProfileIds();

        CARAPI_(Boolean) IsCurrentProfile(
            /* [in] */ Int32 userId);

    private:
        AutoPtr<ISparseArray> mCurrentProfiles; //  = new SparseArray<UserInfo>();
    };

protected:
    class Config
        : public Object
    {
        friend class ManagedServices;
    public:
        String mCaption;
        String mServiceInterface;
        String mSecureSettingName;
        String mBindPermission;
        String mSettingsAction;
        Int32 mClientLabel;
    };

private:
    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("ManagedServices::SettingsObserver")

        SettingsObserver(
            /* [in] */ ManagedServices* host,
            /* [in] */ IHandler* handler);

        ~SettingsObserver();

        CARAPI_(void) Observe();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI_(void) Update(
            /* [in] */ IUri* uri);

    private:
        ManagedServices* mHost;
        AutoPtr<IUri> mSecureSettingsUri;
    };

    class MyServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL();

        MyServiceConnection(
            /* [in] */ ManagedServices* host,
            /* [in] */ const String& servicesBindingTag,
            /* [in] */ Int32 userid,
            /* [in] */ Int32 targetSdkVersion);

        ~MyServiceConnection();

        //@Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* binder);

        //@Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    protected:
        AutoPtr<IInterface> mService;

    private:
        ManagedServices* mHost;
        String mServicesBindingTag;
        Int32 mUserid;
        Int32 mTargetSdkVersion;
    };

public:
    ManagedServices();

    ~ManagedServices();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IInterface* mutex,
        /* [in] */ UserProfiles* userProfiles);

    CARAPI_(void) OnBootPhaseAppsCanStart();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ DumpFilter* filter);

    CARAPI_(void) OnPackagesChanged(
        /* [in] */ Boolean queryReplace,
        /* [in] */ ArrayOf<String>* pkgList);

    CARAPI_(void) OnUserSwitched();

    CARAPI CheckServiceTokenLocked(
        /* [in] */ IInterface* service,
        /* [out] */ ManagedServiceInfo** info);

    CARAPI_(void) UnregisterService(
        /* [in] */ IInterface* service,
        /* [in] */ Int32 userid);

    CARAPI_(void) RegisterService(
        /* [in] */ IInterface* service,
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 userid);

protected:
    virtual CARAPI_(AutoPtr<Config>) GetConfig() = 0;

    virtual CARAPI OnServiceAdded(
        /* [in] */ ManagedServiceInfo* info) = 0;

    CARAPI OnServiceRemovedLocked(
        /* [in] */ ManagedServiceInfo* info);

private:
    CARAPI_(String) GetCaption();

    CARAPI_(AutoPtr<ManagedServices::ManagedServiceInfo>) NewServiceInfo(
        /* [in] */ IInterface* service,
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 userid,
        /* [in] */ Boolean isSystem,
        /* [in] */ IServiceConnection* connection,
        /* [in] */ Int32 targetSdkVersion);

    /**
     * Remove access for any services that no longer exist.
     */
    CARAPI_(void) DisableNonexistentServices();

    CARAPI_(void) DisableNonexistentServices(
        /* [in] */ Int32 userId);

    /**
     * Called whenever packages change, the user switches, or the secure setting
     * is altered. (For example in response to USER_SWITCHED in our broadcast receiver)
     */
    CARAPI RebindServices();

    /**
     * Version of registerService that takes the name of a service component to bind to.
     */
    CARAPI RegisterService(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 userid);

    /**
     * Remove a service for the given user by ComponentName
     */
    CARAPI UnregisterService(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 userid);

    /**
     * Removes a service from the list but does not unbind
     *
     * @return the removed service.
     */
    CARAPI_(AutoPtr<ManagedServices::ManagedServiceInfo>) RemoveServiceImpl(
        /* [in] */ IInterface* service,
        /* [in] */ Int32 userid);

    CARAPI_(AutoPtr<ManagedServices::ManagedServiceInfo>) RemoveServiceLocked(
        /* [in] */ Int32 i);

    CARAPI CheckNotNull(
        /* [in] */ IInterface* service);

    CARAPI RegisterServiceImpl(
        /* [in] */ IInterface* service,
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 userid,
        /* [out] */ ManagedServiceInfo** info);

    /**
     * Removes a service from the list and unbinds.
     */
    CARAPI_(void) UnregisterServiceImpl(
        /* [in] */ IInterface* service,
        /* [in] */ Int32 userid);

protected:
    String TAG;// = getClass().getSimpleName();
    Boolean DEBUG; //= Log.isLoggable(TAG, Log.DEBUG);
    AutoPtr<IContext> mContext;
    Object* mMutex;

    // contains connections to all connected services, including app services
    // and system services
    /*protected final ArrayList<ManagedServiceInfo> mServices = new ArrayList<ManagedServiceInfo>();*/
    AutoPtr<IArrayList> mServices;

private:
    static const String ENABLED_SERVICES_SEPARATOR;

    AutoPtr<UserProfiles> mUserProfiles;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<Config> mConfig;

    // things that will be put into mServices as soon as they're ready
    /* ArrayList<String> mServicesBinding = new ArrayList<String>();*/
    AutoPtr<IArrayList> mServicesBinding;
    // lists the component names of all enabled (and therefore connected)
    // app services for current profiles.
    /*ArraySet<ComponentName> mEnabledServicesForCurrentProfiles
            = new ArraySet<ComponentName>();*/
    AutoPtr<IArraySet> mEnabledServicesForCurrentProfiles;
    // Just the packages from mEnabledServicesForCurrentProfiles
    /*ArraySet<String> mEnabledServicesPackageNames = new ArraySet<String>();*/
    AutoPtr<IArraySet> mEnabledServicesPackageNames;

    // Kept to de-dupe user change events (experienced after boot, when we receive a settings and a
    // user change).
    AutoPtr< ArrayOf<Int32> > mLastSeenProfileIds;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_MANAGEDSERVICES_H__
