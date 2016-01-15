
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPENTRY_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPENTRY_H__

#include "_Elastos_Utility_Zip_CZipEntry.h"
#include "ZipEntry.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CZipEntry)
    , public ZipEntry
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPENTRY_H__
