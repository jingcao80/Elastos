
#ifndef __ELASTOS_DROID_APP_CEXPANDABLE_LIST_ACTIVITY_H__
#define __ELASTOS_DROID_APP_CEXPANDABLE_LIST_ACTIVITY_H__

#include "_Elastos_Droid_App_CExpandableListActivity.h"
#include "elastos/droid/app/ExpandableListActivity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CExpandableListActivity)
    , public ExpandableListActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CEXPANDABLE_LIST_ACTIVITY_H__