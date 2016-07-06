#ifndef  __ELASTOS_DROID_PHONE_CINCALLSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CINCALLSCREEN_H__

#include "_Elastos_Droid_TeleService_Phone_CInCallScreen.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CInCallScreen)
    , public Object
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CINCALLSCREEN_H__