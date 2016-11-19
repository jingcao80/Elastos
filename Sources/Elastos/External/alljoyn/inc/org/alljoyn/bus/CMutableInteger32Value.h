
#ifndef __ORG_ALLJOYN_BUS_CMUTABLEINTEGER32VALUE_H__
#define __ORG_ALLJOYN_BUS_CMUTABLEINTEGER32VALUE_H__

#include "_Org_Alljoyn_Bus_CMutableInteger32Value.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CMutableInteger32Value)
    , public Object
    , public IMutableInteger32Value
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 v);

    CARAPI GetValue(
        /* [out] */ Int32* value);

    CARAPI SetValue(
        /* [in] */ Int32 value);

private:
    Int32 mValue;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CMUTABLEINTEGER32VALUE_H__
