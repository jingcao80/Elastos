
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATERINPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATERINPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CDeflaterInputStream.h"
#include "DeflaterInputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CDeflaterInputStream)
    , public DeflaterInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATERINPUTSTREAM_H__
