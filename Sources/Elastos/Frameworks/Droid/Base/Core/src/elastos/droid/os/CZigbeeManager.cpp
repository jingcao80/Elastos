
#include "elastos/droid/os/CZigbeeManager.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/logging/Slogger.h>

namespace Elastos {
namespace Droid {
namespace Os {

AutoPtr<ArrayOf<Int32> > InitDeviceTypes()
{
    AutoPtr<ArrayOf<Int32> > deviceTypes = ArrayOf<Int32>::Alloc(4);
    (*deviceTypes)[0] = 0x0102;
    (*deviceTypes)[1] = 0x0402;
    (*deviceTypes)[2] = 0x0002;
    (*deviceTypes)[3] = 0x0302;
    return deviceTypes;
}

const AutoPtr<ArrayOf<Int32> > CZigbeeManager::sDeviceTypes = InitDeviceTypes();

AutoPtr<ArrayOf<String> > InitActions()
{
    AutoPtr<ArrayOf<String> > actions = ArrayOf<String>::Alloc(14);
    (*actions)[0] = String("android.intent.action.zigbee.DEVICE_STATE_CHANGED");
    (*actions)[1] = String("android.intent.action.zigbee.DEVICE_COUNT_CHANGED");
    (*actions)[2] = String("android.intent.action.zigbee.ONLINE_STATUS_CHANGED");
    (*actions)[3] = String("android.intent.action.zigbee.SENSOR_TEMPERATURE_CHANGED");
    (*actions)[4] = String("android.intent.action.zigbee.SENSOR_HUMIDITY_CHANGED");
    (*actions)[5] = String("android.intent.action.zigbee.DOOR_ONOFF_STATE_CHANGED");
    (*actions)[6] = String("android.intent.action.zigbee.SENSOR_VOLTAGE_CHANGED");
    (*actions)[7] = String("android.intent.action.zigbee.ACTION_SENSOR_DOOR_WARNING");
    (*actions)[8] = String("android.intent.action.zigbee.ACTION_SENSOR_TEMPERATURE_WARNING");
    (*actions)[9] = String("android.intent.action.zigbee.ACTION_SENSOR_HUMIDITY_WARNING");
    (*actions)[10] = String("android.intent.action.zigbee.SENSOR_TEMPERATURE_UPPER_LIMIT_WARNING");
    (*actions)[11] = String("android.intent.action.zigbee.SENSOR_TEMPERATURE_LOWER_LIMIT_WARNING");
    (*actions)[12] = String("android.intent.action.zigbee.SENSOR_HUMIDITY_UPPER_LIMIT_WARNING");
    (*actions)[13] = String("android.intent.action.zigbee.SENSOR_HUMIDITY_LOWER_LIMIT_WARNING");
    return actions;
}

const AutoPtr<ArrayOf<String> > CZigbeeManager::sActions = InitActions();

CAR_INTERFACE_IMPL(CZigbeeManager)

CAR_OBJECT_IMPL(CZigbeeManager)

ECode CZigbeeManager::IsValidType(
    /* [in] */ Int32 checkType,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (sDeviceTypes->Contains(checkType)) {
        *res = TRUE;
        SLOGGERD("CZigbeeManager", "IsValidType(): Valid type: 0x%x", checkType);
    }
    else {
        *res = FALSE;
        SLOGGERD("CZigbeeManager", "IsValidType(): not valid type: 0x%x", checkType);
    }

    return NOERROR;
}

ECode CZigbeeManager::GetAllActions(
    /* [out, callee] */ ArrayOf<String>** actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = sActions;
    REFCOUNT_ADD(*actions)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
