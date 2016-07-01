#ifndef __ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITESTILEADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITESTILEADAPTER_H__

#include "_Elastos_Droid_Dialer_List_CPhoneFavoritesTileAdapter.h"
#include "elastos/apps/dialer/list/PhoneFavoritesTileAdapter.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CPhoneFavoritesTileAdapter)
    , public PhoneFavoritesTileAdapter
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITESTILEADAPTER_H__
