
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPFILE_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPFILE_H__

#include "_Elastos_Utility_Zip_CZipFile.h"
#include "ZipFile.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CZipFile) , public ZipFile
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CZIPFILE_H__
