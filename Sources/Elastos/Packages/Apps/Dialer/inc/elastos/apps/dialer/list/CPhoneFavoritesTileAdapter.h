#ifndef __ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITESTILEADAPTER_H__
#define __ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITESTILEADAPTER_H__

#include "_Elastos_Apps_Dialer_List_CPhoneFavoritesTileAdapter.h"
#include "PhoneFavoritesTileAdapter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

CarClass(CPhoneFavoritesTileAdapter)
    , public PhoneFavoritesTileAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CPHONEFAVORITESTILEADAPTER_H__
