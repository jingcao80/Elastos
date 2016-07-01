
#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CDIALPADSLIDINGRELATIVELAYOUT_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CDIALPADSLIDINGRELATIVELAYOUT_H__

#include "elastos/apps/dialer/dialpad/DialpadFragment.h"
#include "_Elastos_Droid_Dialer_Dialpad_CDialpadSlidingRelativeLayout.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CDialpadSlidingRelativeLayout)
    , public DialpadFragment::DialpadSlidingRelativeLayout
{
public:
    CAR_OBJECT_DECL();
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CDIALPADSLIDINGRELATIVELAYOUT_H__