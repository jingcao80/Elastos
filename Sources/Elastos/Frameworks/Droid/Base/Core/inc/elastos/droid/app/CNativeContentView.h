
#ifndef __ELASTOS_DROID_APP_CNATIVECONTENTVIEW_H__
#define __ELASTOS_DROID_APP_CNATIVECONTENTVIEW_H__

#include "_Elastos_Droid_App_CNativeContentView.h"
#include "elastos/droid/app/NativeActivity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNativeContentView)
    , public NativeActivity::NativeContentView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNATIVECONTENTVIEW_H__
