#ifndef __ELASTOS_APPS_DIALER_LIST_CDIALERPHONENUMBERLISTADAPTER_H__
#define __ELASTOS_APPS_DIALER_LIST_CDIALERPHONENUMBERLISTADAPTER_H__

#include "_Elastos_Apps_Dialer_List_CDialerPhoneNumberListAdapter.h"
#include "DialerPhoneNumberListAdapter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

CarClass(CDialerPhoneNumberListAdapter)
    , public DialerPhoneNumberListAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CDIALERPHONENUMBERLISTADAPTER_H__
