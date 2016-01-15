#ifndef __ELASTOS_DROID_OS_CZIGBEEMANAGER_H__
#define __ELASTOS_DROID_OS_CZIGBEEMANAGER_H__

#include "_Elastos_Droid_Os_CZigbeeManager.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CZigbeeManager)
{
public:
    /*
     *  Check if type is defined
     *  Return value: true - type is defined, false - invalid type
     */
    CARAPI IsValidType(
        /* [in] */ Int32 checkType,
        /* [out] */ Boolean* res);

    // Get available ACTION for Zigbee service
    CARAPI GetAllActions(
        /* [out, callee] */ ArrayOf<String>** actions);

private:
    static const AutoPtr<ArrayOf<Int32> > sDeviceTypes;
    static const AutoPtr<ArrayOf<String> > sActions;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CZIGBEEMANAGER_H__
