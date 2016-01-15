
#ifndef __ELASTOS_DROID_APP_CLISTFRAGMENT_H__
#define __ELASTOS_DROID_APP_CLISTFRAGMENT_H__

#include "_Elastos_Droid_App_CListFragment.h"
#include "elastos/droid/app/ListFragment.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CListFragment)
    , public ListFragment
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CLISTFRAGMENT_H__