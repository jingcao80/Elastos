
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ZENMODECONTROLLERCALLBACK_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ZENMODECONTROLLERCALLBACK_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class ZenModeControllerCallback
    : public Object
    , public IZenModeControllerCallback
{
public:
    CAR_INTERFACE_DECL()

    CARAPI OnZenChanged(
        /* [in] */ Int32 zen);

    CARAPI OnExitConditionChanged(
        /* [in] */ ICondition* exitCondition);

    CARAPI OnConditionsChanged(
        /* [in] */ ArrayOf<ICondition*>* conditions);

    CARAPI OnNextAlarmChanged();

    CARAPI OnZenAvailableChanged(
        /* [in] */ Boolean available);

    CARAPI OnEffectsSupressorChanged();
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ZENMODECONTROLLERCALLBACK_H__
