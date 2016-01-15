#ifndef __ELASTOS_DROID_INTERNAL_OS_CPOWERPROFILE_H__
#define __ELASTOS_DROID_INTERNAL_OS_CPOWERPROFILE_H__

#include "_Elastos_Droid_Internal_Os_CPowerProfile.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CPowerProfile)
    , public Object
    , public IPowerProfile
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Returns the average current in mA consumed by the subsystem
     * @param type the subsystem type
     * @return the average current in milliAmps.
     */
    CARAPI GetAveragePower(
        /* [in] */ const String& type,
        /* [out] */ Double* result);

    /**
     * Returns the average current in mA consumed by the subsystem for the given level.
     * @param type the subsystem type
     * @param level the level of power at which the subsystem is running. For instance, the
     *  signal strength of the cell network between 0 and 4 (if there are 4 bars max.)
     *  If there is no data for multiple levels, the level is ignored.
     * @return the average current in milliAmps.
     */
    CARAPI GetAveragePower(
        /* [in] */ const String& type,
        /* [in] */ Int32 level,
        /* [out] */ Double* result);

    /**
     * Returns the battery capacity, if available, in milli Amp Hours. If not available,
     * it returns zero.
     * @return the battery capacity in mAh
     */
    CARAPI GetBatteryCapacity(
        /* [out] */ Double* result);

    /**
     * Returns the number of speeds that the CPU can be run at.
     * @return
     */
    CARAPI GetNumSpeedSteps(
        /* [out] */ Int32* result);

private:
    CARAPI ReadPowerValuesFromXml(
        /* [in] */ IContext* context);

public:
    static HashMap<String, AutoPtr<IInterface> > sPowerMap;

private:
    static const String TAG_DEVICE;
    static const String TAG_ITEM;
    static const String TAG_ARRAY;
    static const String TAG_ARRAYITEM;
    static const String ATTR_NAME;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_CPOWERPROFILE_H__
