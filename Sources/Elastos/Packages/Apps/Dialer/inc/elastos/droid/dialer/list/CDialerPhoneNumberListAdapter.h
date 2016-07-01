#ifndef __ELASTOS_DROID_DIALER_LIST_CDIALERPHONENUMBERLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_CDIALERPHONENUMBERLISTADAPTER_H__

#include "_Elastos_Droid_Dialer_List_CDialerPhoneNumberListAdapter.h"
#include "elastos/apps/dialer/list/DialerPhoneNumberListAdapter.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CDialerPhoneNumberListAdapter)
    , public DialerPhoneNumberListAdapter
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CDIALERPHONENUMBERLISTADAPTER_H__
