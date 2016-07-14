
#ifndef __ELASTOS_DROID_INCALLUI_CALL_H__
#define __ELASTOS_DROID_INCALLUI_CALL_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Call
    : public Object
    , public ICall
{
public:
    CAR_INTERFACE_DECL();
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALL_H__
