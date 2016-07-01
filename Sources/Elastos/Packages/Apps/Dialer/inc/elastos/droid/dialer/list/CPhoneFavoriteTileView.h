#ifndef __ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITETITLEVIEW_H__
#define __ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITETITLEVIEW_H__

#include "_Elastos_Droid_Dialer_List_CPhoneFavoriteTileView.h"
#include "elastos/apps/dialer/list/PhoneFavoriteTileView.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITETITLEVIEW_H__
