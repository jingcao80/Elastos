
#ifndef __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__
#define __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManager;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class BatteryController
    : public Object
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

    CARAPI IsPowerSave(
        /* [out] */ Boolean* result);

    CARAPI_(void) SetPowerSave(
        /* [in] */ Boolean powerSave);

    CARAPI_(void) FireBatteryLevelChanged();

    CARAPI_(void) UpdatePowerSave();

private:
    CARAPI_(void) FirePowerSaveChanged();

public:
    Int32 mLevel;
    Boolean mCharging;
    Boolean mCharged;
    Boolean mPluggedIn;

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IArrayList> mChangeCallbacks;  /*<BatteryStateChangeCallback*/
    AutoPtr<IPowerManager> mPowerManager;

    Boolean mPowerSave;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__
