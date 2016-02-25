
#ifndef __ELASTOS_DROID_SERVER_TRUST_TRUSTMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_TRUST_TRUSTMANAGERSERVICE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/SystemService.h"
#include <Elastos.Droid.App.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/content/PackageMonitor.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/provider/Settings.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::Trust::IITrustListener;
using Elastos::Droid::App::Trust::IITrustManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastosx::Net::Ssl::ITrustManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Trust {

class TrustAgentWrapper;
class TrustArchive;

/**
 * Manages trust agents and trust listeners.
 *
 * It is responsible for binding to the enabled {@link android.service.trust.TrustAgentService}s
 * of each user and notifies them about events that are relevant to them.
 * It start and stops them based on the value of
 * {@link com.android.internal.widget.LockPatternUtils#getEnabledTrustAgents(int)}.
 *
 * It also keeps a set of {@link android.app.trust.ITrustListener}s that are notified whenever the
 * trust state changes for any user.
 *
 * Trust state and the setting of enabled agents is kept per user and each user has its own
 * instance of a {@link android.service.trust.TrustAgentService}.
 */
class TrustManagerService
    : public SystemService
{
private:
    // Agent management
    class AgentInfo
        : public Object
        , public IAgentInfo
    {
    public:
        CAR_INTERFACE_DECL()

        AgentInfo();

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* result);

    public:
        AutoPtr<ICharSequence> mLabel;

        AutoPtr<IDrawable> mIcon;

        AutoPtr<IComponentName> mComponent;

        // service that implements ITrustAgent
        AutoPtr<IComponentName> mSettings;

        // setting to launch to modify agent.
        AutoPtr<TrustAgentWrapper> mAgent;

        Int32 mUserId;
    };

    class Receiver
        : public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ TrustManagerService* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI Register(
            /* [in] */ IContext* context);

    private:
        TrustManagerService* mHost;
    };

    class InnerSub_ITrustManager
        : public Object
        , public IITrustManager
        , public IBinder
    {
    private:
        class InnerSub_Runnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            InnerSub_Runnable(
                /* [in] */ InnerSub_ITrustManager* host,
                /* [in] */ IPrintWriter* fout,
                /* [in] */ IList* userInfos,
                /* [in] */ IUserInfo* mCurrentUser);

            // @Override
            CARAPI Run();

        private:
            InnerSub_ITrustManager* mHost;
            AutoPtr<IPrintWriter> mFout;
            AutoPtr<IList> mUserInfos;
            AutoPtr<IUserInfo> mCurrentUser;
        };

    public:
        CAR_INTERFACE_DECL()

        InnerSub_ITrustManager(
            /* [in] */ TrustManagerService* host);

        // @Override
        CARAPI ReportUnlockAttempt(
            /* [in] */ Boolean authenticated,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI ReportEnabledTrustAgentsChanged(
            /* [in] */ Int32 userId);

        // @Override
        CARAPI ReportRequireCredentialEntry(
            /* [in] */ Int32 userId);

        // @Override
        CARAPI RegisterTrustListener(
            /* [in] */ IITrustListener* trustListener);

        // @Override
        CARAPI UnregisterTrustListener(
            /* [in] */ IITrustListener* trustListener);

        // @Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* fout,
            /* [in] */ ArrayOf<String>* args);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CARAPI EnforceReportPermission();

        CARAPI EnforceListenerPermission();

        CARAPI DumpUser(
            /* [in] */ IPrintWriter* fout,
            /* [in] */ IUserInfo* user,
            /* [in] */ Boolean isCurrent);

        CARAPI_(String) DumpBool(
            /* [in] */ Boolean b);

    private:
        TrustManagerService* mHost;
    };

    class InnerSub_Handler
        : public Handler
    {
    public:
        InnerSub_Handler(
            /* [in] */ TrustManagerService* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TrustManagerService* mHost;
    };

    class InnerSub_PackageMonitor
        : public PackageMonitor
    {
    public:
        InnerSub_PackageMonitor(
            /* [in] */ TrustManagerService* host);

        // @Override
        CARAPI OnSomePackagesChanged();

        // @Override
        CARAPI OnPackageChanged(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid,
            /* [in] */ ArrayOf<String>* components,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnPackageDisappeared(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 reason);

    private:
        TrustManagerService* mHost;
    };

public:
    TrustManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

    CARAPI UpdateTrust(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean initiatedByUser);

    CARAPI RefreshAgentList(
        /* [in] */ Int32 userId);

    CARAPI UpdateDevicePolicyFeatures();

    CARAPI ResetAgent(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 userId);

private:
    CARAPI UpdateTrustAll();

    CARAPI RemoveAgentsOfPackage(
        /* [in] */ const String& packageName);

    CARAPI GetSettingsComponentName(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IResolveInfo* resolveInfo,
        /* [out] */ IComponentName** result);

    CARAPI_(AutoPtr<IComponentName>) GetComponentName(
        /* [in] */ IResolveInfo* resolveInfo);

    CARAPI MaybeEnableFactoryTrustAgents(
        /* [in] */ ILockPatternUtils* utils,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IList>) ResolveAllowedTrustAgents(
        /* [in] */ IPackageManager* pm,
        /* [in] */ Int32 userId);

    // Agent dispatch and aggregation
    CARAPI_(Boolean) AggregateIsTrusted(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) AggregateIsTrustManaged(
        /* [in] */ Int32 userId);

    CARAPI DispatchUnlockAttempt(
        /* [in] */ Boolean successful,
        /* [in] */ Int32 userId);

    CARAPI UpdateUserHasAuthenticated(
        /* [in] */ Int32 userId);

    CARAPI RequireCredentialEntry(
        /* [in] */ Int32 userId);

    // Listeners
    CARAPI AddListener(
        /* [in] */ IITrustListener* listener);

    CARAPI RemoveListener(
        /* [in] */ IITrustListener* listener);

    CARAPI DispatchOnTrustChanged(
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean initiatedByUser);

    CARAPI DispatchOnTrustManagedChanged(
        /* [in] */ Boolean managed,
        /* [in] */ Int32 userId);

    static CARAPI_(AutoPtr<IIntent>) InitIntent();

public:
    /* package */ AutoPtr<TrustArchive> mArchive;

private:
    static const Boolean DEBUG;

    static const String TAG;

    static AutoPtr<IIntent> TRUST_AGENT_INTENT;

    AutoPtr<IArraySet> mActiveAgents;

    AutoPtr<IArrayList> mTrustListeners;

    AutoPtr<Receiver> mReceiver;

    AutoPtr<ISparseBooleanArray> mUserHasAuthenticatedSinceBoot;

    // Plumbing
    AutoPtr<IBinder> mService;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IPackageMonitor> mPackageMonitor;

    static const String PERMISSION_PROVIDE_AGENT;

    static const Int32 MSG_REGISTER_LISTENER;

    static const Int32 MSG_UNREGISTER_LISTENER;

    static const Int32 MSG_DISPATCH_UNLOCK_ATTEMPT;

    static const Int32 MSG_ENABLED_AGENTS_CHANGED;

    static const Int32 MSG_REQUIRE_CREDENTIAL_ENTRY;

    AutoPtr<IContext> mContext;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    AutoPtr<IUserManager> mUserManager;
};

} // namespace Trust
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TRUST_TRUSTMANAGERSERVICE_H__
