
#ifndef __ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENER_H__
#define __ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENER_H__

#include "Elastos.Droid.Telecomm.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Telecom::IVideoCallListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallVideoCallListener
    : public Object
    , public IVideoCallListener
{
public:
    CAR_INTERFACE_DECL();
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENER_H__
