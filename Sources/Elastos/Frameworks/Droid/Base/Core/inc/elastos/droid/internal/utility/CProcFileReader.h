
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CPROCFILEREADER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CPROCFILEREADER_H__

#include "_Elastos_Droid_Internal_Utility_CProcFileReader.h"
#include "elastos/droid/internal/utility/ProcFileReader.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CProcFileReader)
    , public ProcFileReader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CPROCFILEREADER_H__
