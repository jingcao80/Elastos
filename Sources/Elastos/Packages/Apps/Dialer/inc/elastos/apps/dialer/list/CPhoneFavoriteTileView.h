#ifndef __ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITETITLEVIEW_H__
#define __ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITETITLEVIEW_H__

#include "_Elastos_Apps_Dialer_List_CPhoneFavoriteTileView.h"
#include "elastos/apps/dialer/list/PhoneFavoriteTileView.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CarClass(CPhoneFavoriteTileView)
    , public PhoneFavoriteTileView
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITETITLEVIEW_H__
