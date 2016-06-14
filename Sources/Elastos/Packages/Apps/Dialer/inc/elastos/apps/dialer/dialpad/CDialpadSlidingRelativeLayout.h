
#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CDIALPADSLIDINGRELATIVELAYOUT_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CDIALPADSLIDINGRELATIVELAYOUT_H__

#include "DialpadFragment.h"
#include "_Elastos_Apps_Dialer_Dialpad_CDialpadSlidingRelativeLayout.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

CarClass(CDialpadSlidingRelativeLayout)
    , public DialpadFragment::DialpadSlidingRelativeLayout
{
public:
    CAR_OBJECT_DECL()
};

} // Dialpad
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CDIALPADSLIDINGRELATIVELAYOUT_H__