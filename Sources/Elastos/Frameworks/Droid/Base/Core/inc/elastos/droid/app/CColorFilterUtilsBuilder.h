
#ifndef __ELASTOS_DROID_APP_CCOLORFILTERUTILSBUILDER_H__
#define __ELASTOS_DROID_APP_CCOLORFILTERUTILSBUILDER_H__

#include "_Elastos_Droid_App_CColorFilterUtilsBuilder.h"
#include "elastos/droid/app/IconPackHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CColorFilterUtilsBuilder)
    , public IconPackHelper::ColorFilterUtils::Builder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CCOLORFILTERUTILSBUILDER_H__
