
#ifndef __ELASTOS_DROID_HARDWARE_CGEOMAGNETICFIELD_H__
#define __ELASTOS_DROID_HARDWARE_CGEOMAGNETICFIELD_H__

#include "_Elastos_Droid_Hardware_CGeomagneticField.h"
#include "elastos/droid/hardware/GeomagneticField.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CGeomagneticField)
    , public GeomagneticField
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CGEOMAGNETICFIELD_H__
