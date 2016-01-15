
#ifndef __ELASTOS_DROID_VIEW_CINPUTCHANNEL_H__
#define __ELASTOS_DROID_VIEW_CINPUTCHANNEL_H__

#include "_Elastos_Droid_View_CInputChannel.h"
#include "elastos/droid/view/InputChannel.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CInputChannel)
    , public InputChannel
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CINPUTCHANNEL_H__
