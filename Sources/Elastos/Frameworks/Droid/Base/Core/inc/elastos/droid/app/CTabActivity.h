
#ifndef __ELASTOS_DROID_APP_CTAB_ACTIVITY_H__
#define __ELASTOS_DROID_APP_CTAB_ACTIVITY_H__

#include "_Elastos_Droid_App_CTabActivity.h"
#include "elastos/droid/app/TabActivity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CTabActivity)
    , public TabActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CTAB_ACTIVITY_H__