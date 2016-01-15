
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPOUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPOUTPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CZipOutputStream.h"
#include "ZipOutputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CZipOutputStream) , public ZipOutputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPOUTPUTSTREAM_H__
