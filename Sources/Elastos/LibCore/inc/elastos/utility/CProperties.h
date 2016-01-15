#ifndef __ELASTOS_UTILITY_CPROPERTIES_H__
#define __ELASTOS_UTILITY_CPROPERTIES_H__

#include "_Elastos_Utility_CProperties.h"
#include "Properties.h"

namespace Elastos{
namespace Utility{

CarClass(CProperties), public Properties
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IProperties* properties);
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CPROPERTIES_H__
