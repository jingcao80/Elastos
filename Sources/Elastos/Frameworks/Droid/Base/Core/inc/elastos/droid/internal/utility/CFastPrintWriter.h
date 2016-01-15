
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CFASTPRINTWRITER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CFASTPRINTWRITER_H__

#include "_Elastos_Droid_Internal_Utility_CFastPrintWriter.h"
#include "elastos/droid/internal/utility/FastPrintWriter.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CFastPrintWriter)
    , public FastPrintWriter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CFASTPRINTWRITER_H__
