
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CINFLATEROUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CINFLATEROUTPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CInflaterOutputStream.h"
#include "InflaterOutputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CInflaterOutputStream)
    , public InflaterOutputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CINFLATEROUTPUTSTREAM_H__
