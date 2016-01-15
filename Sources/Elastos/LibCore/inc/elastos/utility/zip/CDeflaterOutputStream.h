
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATEROUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATEROUTPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CDeflaterOutputStream.h"
#include "DeflaterOutputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CDeflaterOutputStream)
    , public DeflaterOutputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATEROUTPUTSTREAM_H__
