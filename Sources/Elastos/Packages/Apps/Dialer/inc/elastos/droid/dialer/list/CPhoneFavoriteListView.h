#ifndef __ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITELISTVIEW_H__
#define __ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITELISTVIEW_H__

#include "_Elastos_Droid_Dialer_List_CPhoneFavoriteListView.h"
#include "elastos/droid/dialer/list/PhoneFavoriteListView.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CPHONEFAVORITELISTVIEW_H__
