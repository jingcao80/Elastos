
#ifndef __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__
#define __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__

#include "_SystemUI.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManager;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class BatteryController
    : public BroadcastReceiver
    , public IBatteryController
{
public:
    CAR_INTERFACE_DECL();

    BatteryController(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI AddStateChangedCallback(
        /* [in] */ IBatteryStateChangeCallback* cb);

    CARAPI RemoveStateChangedCallback(
        /* [in] */ IBatteryStateChangeCallback* cb);

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI IsPowerSave(
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) UpdatePowerSave();

    CARAPI_(void) SetPowerSave(
        /* [in] */ Boolean powerSave);

    CARAPI_(void) FireBatteryLevelChanged();

    CARAPI_(void) FirePowerSaveChanged();

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IArrayList/*<BatteryStateChangeCallback*/> mChangeCallbacks;
    AutoPtr<IPowerManager> mPowerManager;

    Int32 mLevel;
    Boolean mPluggedIn;
    Boolean mCharging;
    Boolean mCharged;
    Boolean mPowerSave;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__
