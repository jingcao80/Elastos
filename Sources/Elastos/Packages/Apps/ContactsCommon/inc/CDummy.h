
#include "_CDummy.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

CarClass(CDummy)
    , public Object
    , public IDummy
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();
};
