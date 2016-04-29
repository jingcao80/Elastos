
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ZENMODECONTROLLERIMPL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ZENMODECONTROLLERIMPL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/systemui/qs/GlobalSetting.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IIConditionListener;
using Elastos::Droid::SystemUI::Qs::GlobalSetting;
using Elastos::Droid::SystemUI::Qs::IGlobalSetting;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILinkedHashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/** Platform implementation of the zen mode controller. **/
class ZenModeControllerImpl
    : public Object
    , public IZenModeController
{
private:
    class ConditionListener
        : public Object
        , public IIConditionListener
    {
    public:
        CAR_INTERFACE_DECL();

        ConditionListener(
            /* [in] */ ZenModeControllerImpl* host);

        // @Override
        CARAPI OnConditionsReceived(
            /* [in] */ ArrayOf<ICondition*>* conditions);

    private:
        ZenModeControllerImpl* mHost;
    };

    class Receiver: public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ ZenModeControllerImpl* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("ZenModeControllerImpl.Receiver");
            return NOERROR;
        }

    private:
        ZenModeControllerImpl* mHost;
    };

    class SetupObserver: public ContentObserver
    {
    public:
        TO_STRING_IMPL("ZenModeControllerImpl::SetupObserver")

        SetupObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ ZenModeControllerImpl* host);

        CARAPI IsUserSetup(
            /* [out] */ Boolean* result);

        CARAPI IsDeviceProvisioned(
            /* [out] */ Boolean* result);

        CARAPI Register();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IContentResolver> mResolver;
        Boolean mRegistered;
        ZenModeControllerImpl* mHost;
    };

    class ModeSetting: public GlobalSetting
    {
    public:
        ModeSetting(
            /* [in] */ ZenModeControllerImpl* host,
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ const String& tag);

    protected:
        // @Override
        CARAPI_(void) HandleValueChanged(
            /* [in] */ Int32 value);

    private:
        ZenModeControllerImpl* mHost;
    };

    class ConfigSetting: public GlobalSetting
    {
    public:
        ConfigSetting(
            /* [in] */ ZenModeControllerImpl* host,
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ const String& tag);

    protected:
        // @Override
        CARAPI_(void) HandleValueChanged(
            /* [in] */ Int32 value);

    private:
        ZenModeControllerImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ZenModeControllerImpl(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    // @Override
    CARAPI AddCallback(
        /* [in] */ IZenModeControllerCallback* callback);

    // @Override
    CARAPI RemoveCallback(
        /* [in] */ IZenModeControllerCallback* callback);

    // @Override
    CARAPI GetZen(
        /* [out] */ Int32* zen);

    // @Override
    CARAPI SetZen(
        /* [in] */ Int32 zen);

    // @Override
    CARAPI IsZenAvailable(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RequestConditions(
        /* [in] */ Boolean request);

    // @Override
    CARAPI SetExitCondition(
        /* [in] */ ICondition* exitCondition);

    // @Override
    CARAPI GetExitCondition(
        /* [out] */ ICondition** c);

    // @Override
    CARAPI GetNextAlarm(
        /* [out] */ Int64* alarm);

    // @Override
    CARAPI SetUserId(
        /* [in] */ Int32 userId);

    // @Override
    CARAPI GetEffectsSuppressor(
        /* [out] */ IComponentName** cn);

private:
    CARAPI_(void) FireNextAlarmChanged();

    CARAPI_(void) FireEffectsSuppressorChanged();

    CARAPI_(void) FireZenChanged(
        /* [in] */ Int32 zen);

    CARAPI_(void) FireZenAvailableChanged(
        /* [in] */ Boolean available);

    CARAPI_(void) FireConditionsChanged(
        /* [in] */ ArrayOf<IInterface*>* conditions);

    CARAPI_(void) FireExitConditionChanged();

    CARAPI_(void) UpdateConditions(
        /* [in] */ ArrayOf<ICondition*>* conditions);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IArrayList> mCallbacks;  /*<Callback*/
    AutoPtr<IContext> mContext;
    AutoPtr<IGlobalSetting> mModeSetting;
    AutoPtr<IGlobalSetting> mConfigSetting;
    AutoPtr<IINotificationManager> mNoMan;
    AutoPtr<ILinkedHashMap> mConditions;  /*<Uri, Condition*/
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<SetupObserver> mSetupObserver;

    Int32 mUserId;
    Boolean mRequesting;
    Boolean mRegistered;
    AutoPtr<IIConditionListener> mListener;
    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ZENMODECONTROLLERIMPL_H__
