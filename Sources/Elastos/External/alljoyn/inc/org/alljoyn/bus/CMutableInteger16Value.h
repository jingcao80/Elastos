
#ifndef __ORG_ALLJOYN_BUS_CMUTABLEINTEGER16VALUE_H__
#define __ORG_ALLJOYN_BUS_CMUTABLEINTEGER16VALUE_H__

#include "_Org_Alljoyn_Bus_CMutableInteger16Value.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CMutableInteger16Value)
    , public Object
    , public IMutableInteger16Value
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int16 v);

    CARAPI GetValue(
        /* [out] */ Int16* value);

    CARAPI SetValue(
        /* [in] */ Int16 value);

private:
    Int16 mValue;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CMUTABLEINTEGER16VALUE_H__
