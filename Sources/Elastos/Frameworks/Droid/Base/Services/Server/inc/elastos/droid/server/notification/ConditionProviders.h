
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CONDITIONPROVIDERS_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CONDITIONPROVIDERS_H__

#include "elastos/droid/server/notification/CountdownConditionProvider.h"
#include "elastos/droid/server/notification/DowntimeConditionProvider.h"
#include "elastos/droid/server/notification/ManagedServices.h"
#include "elastos/droid/server/notification/ZenModeHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Server::Notification::ManagedServices;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IIConditionListener;
using Elastos::Droid::Service::Notification::IIConditionProvider;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

class ConditionProviders
    : public ManagedServices
{
private:
    class ZenModeHelperCallback
        : public ZenModeHelper::Callback
    {
    public:
        ZenModeHelperCallback(
            /* [in] */ ConditionProviders* host);

        ~ZenModeHelperCallback();

        // @Override
        CARAPI_(void) OnConfigChanged();

        // @Override
        CARAPI_(void) OnZenModeChanged();

    private:
        ConditionProviders* mHost;
    };

    class DowntimeCallback
        : public Object
        , public IDowntimeConditionProviderCallback
    {
    public:
        CAR_INTERFACE_DECL();

        DowntimeCallback(
            /* [in] */ ConditionProviders* host);

        ~DowntimeCallback();

        // @Override
        CARAPI OnDowntimeChanged(
            /* [in] */ Boolean inDowntime);

    private:
        ConditionProviders* mHost;
    };

    class ConditionRecord
        : public Object
    {
    public:
        ~ConditionRecord();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        ConditionRecord(
            /* [in] */ IUri* id,
            /* [in] */ IComponentName* component);

    public:
        AutoPtr<IUri> mId;
        AutoPtr<IComponentName> mComponent;
        AutoPtr<ICondition> mCondition;
        AutoPtr<ManagedServices::ManagedServiceInfo> mInfo;
        Boolean mIsAutomatic;
        Boolean mIsManual;
    };

public:
    ConditionProviders();

    ~ConditionProviders();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IInterface* userProfiles,
        /* [in] */ IInterface* zenModeHelper);

    // @Override
    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ DumpFilter* filter);

    // @Override
    CARAPI_(void) OnBootPhaseAppsCanStart();

    CARAPI_(AutoPtr<ManagedServices::ManagedServiceInfo>) CheckServiceToken(
        /* [in] */ IIConditionProvider* provider);

    CARAPI_(void) RequestZenModeConditions(
        /* [in] */ IIConditionListener* callback,
        /* [in] */ Int32 relevance);

    CARAPI NotifyConditions(
        /* [in] */ const String& pkg,
        /* [in] */ ManagedServices::ManagedServiceInfo* info,
        /* [in] */ ArrayOf<ICondition*>* conditions);

    CARAPI_(void) SetZenModeCondition(
        /* [in] */ ICondition* condition,
        /* [in] */ const String& reason);

    CARAPI_(void) SetAutomaticZenModeConditions(
        /* [in] */ ArrayOf<IUri*>* conditionIds);

    CARAPI_(AutoPtr< ArrayOf<ICondition*> >) GetAutomaticZenModeConditions();

protected:
    // @Override
    CARAPI_(AutoPtr<ManagedServices::Config>) GetConfig();

    // @Override
    CARAPI OnServiceAdded(
        /* [in] */ ManagedServices::ManagedServiceInfo* info);

    // @Override
    CARAPI OnServiceRemovedLocked(
        /* [in] */ ManagedServices::ManagedServiceInfo* removed);

private:
    CARAPI_(AutoPtr< ArrayOf<ICondition*> >) ValidateConditions(
        /* [in] */ const String& pkg,
        /* [in] */ ArrayOf<ICondition*>* conditions);

    CARAPI_(AutoPtr<ConditionProviders::ConditionRecord>) GetRecordLocked(
        /* [in] */ IUri* id,
        /* [in] */ IComponentName* component);

    CARAPI SubscribeLocked(
        /* [in] */ ConditionRecord* r);

    static CARAPI_(AutoPtr<IArraySet>) SafeSet(
        /* [in] */ ArrayOf<IInterface*>* items);

    CARAPI_(void) SetAutomaticZenModeConditions(
        /* [in] */ ArrayOf<IUri*>* conditionIds,
        /* [in] */ Boolean save);

    CARAPI UnsubscribeLocked(
        /* [in] */ ConditionRecord* r);

    static CARAPI_(AutoPtr<IIConditionProvider>) Provider(
        /* [in] */ ConditionRecord* r);

    static CARAPI_(AutoPtr<IIConditionProvider>) Provider(
        /* [in] */ ManagedServices::ManagedServiceInfo* info);

    CARAPI RequestConditionsLocked(
        /* [in] */ Int32 flags);

    CARAPI_(void) LoadZenConfig();

    CARAPI_(void) SaveZenConfigLocked();

private:
    static const AutoPtr< ArrayOf<ICondition*> > NO_CONDITIONS;// = new Condition[0];

    AutoPtr<ZenModeHelper> mZenModeHelper;
    // private final ArrayMap<IBinder, IConditionListener> mListeners
    //         = new ArrayMap<IBinder, IConditionListener>();
    AutoPtr<IArrayMap> mListeners;
    // private final ArrayList<ConditionRecord> mRecords = new ArrayList<ConditionRecord>();
    AutoPtr<IArrayList> mRecords;
    AutoPtr<CountdownConditionProvider> mCountdown;// = new CountdownConditionProvider();
    AutoPtr<DowntimeConditionProvider> mDowntime;// = new DowntimeConditionProvider();

    AutoPtr<ICondition> mExitCondition;
    AutoPtr<IComponentName> mExitConditionComponent;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_CONDITIONPROVIDERS_H__
