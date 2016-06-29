
#include "elastos/droid/systemui/statusbar/policy/ZenModeControllerCallback.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(ZenModeControllerCallback, Object, IZenModeControllerCallback)

ECode ZenModeControllerCallback::OnZenChanged(
    /* [in] */ Int32 zen)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnExitConditionChanged(
    /* [in] */ ICondition* exitCondition)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnConditionsChanged(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnNextAlarmChanged()
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnZenAvailableChanged(
    /* [in] */ Boolean available)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnEffectsSupressorChanged()
{
    return NOERROR;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
