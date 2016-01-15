
#ifndef __ELASTOS_DROID_APP_CLISTACTIVITY_H__
#define __ELASTOS_DROID_APP_CLISTACTIVITY_H__

#include "_Elastos_Droid_App_CListActivity.h"
#include "elastos/droid/app/ListActivity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CListActivity)
    , public ListActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CLISTACTIVITY_H__