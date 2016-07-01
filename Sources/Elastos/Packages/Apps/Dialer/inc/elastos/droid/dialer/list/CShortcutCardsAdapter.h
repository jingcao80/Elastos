#ifndef __ELASTOS_DROID_DIALER_LIST_CSHORTCUTCARDSADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_CSHORTCUTCARDSADAPTER_H__

#include "_Elastos_Droid_Dialer_List_CShortcutCardsAdapter.h"
#include "elastos/apps/dialer/list/ShortcutCardsAdapter.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CShortcutCardsAdapter)
    , public ShortcutCardsAdapter
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CSHORTCUTCARDSADAPTER_H__
