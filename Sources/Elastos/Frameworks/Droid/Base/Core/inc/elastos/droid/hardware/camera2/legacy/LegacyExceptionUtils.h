
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYEXCEPTIONUTILES_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYEXCEPTIONUTILES_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyExceptionUtils
    : public Object
    , public ILegacyExceptionUtils
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI ThrowOnError(
        /* [in] */ Int32 errorFlag,
        /* [out] */ Int32* result);

private:
    LegacyExceptionUtils();

private:
    static const String TAG;

};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYEXCEPTIONUTILES_H__
