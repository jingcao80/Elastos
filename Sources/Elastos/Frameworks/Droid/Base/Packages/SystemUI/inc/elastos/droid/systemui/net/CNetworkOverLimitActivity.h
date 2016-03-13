#ifndef __ELASTOS_DROID_SYSTEMUI_NET_CNETWORKOVERLIMITACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_NET_CNETWORKOVERLIMITACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Net_CNetworkOverLimitActivity.h"
#include "elastos/droid/systemui/net/NetworkOverLimitActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Net{

CarClass(CNetworkOverLimitActivity)
    , public NetworkOverLimitActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_NET_CNETWORKOVERLIMITACTIVITY_H__
