#ifndef __ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITELISTVIEW_H__
#define __ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITELISTVIEW_H__

#include "_Elastos_Apps_Dialer_List_CPhoneFavoriteListView.h"
#include "PhoneFavoriteListView.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

CarClass(CPhoneFavoriteListView)
    , public PhoneFavoriteListView
{
public:
    CAR_OBJECT_DECL()
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITELISTVIEW_H__
