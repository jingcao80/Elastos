
#ifndef __ELASTOS_UTILITY_CDATE_H__
#define __ELASTOS_UTILITY_CDATE_H__

#include "_Elastos_Utility_CDate.h"
#include "Date.h"

namespace Elastos {
namespace Utility {

CarClass(CDate)
    , public Date
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CDATE_H__
