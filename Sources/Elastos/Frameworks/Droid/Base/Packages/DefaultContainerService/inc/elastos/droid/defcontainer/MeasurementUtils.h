#ifndef __ELASTOS_DROID_PACKAGES_MEASUREMENTUTILS_H__
#define __ELASTOS_DROID_PACKAGES_MEASUREMENTUTILS_H__

#include "elastos/core/Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace DefContainer {

class MeasurementUtils
    : public Object
{
public:
    static CARAPI_(Int64) MeasureDirectory(
         /* [in] */ const String& path);
};

} //namespace DefContainer
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PACKAGES_MEASUREMENTUTILS_H__

