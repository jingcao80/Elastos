#ifndef __ELASTOS_DROID_SYSTEMUI_EGG_CLLANDACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_EGG_CLLANDACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Egg_CLLandActivity.h"
#include "elastos/droid/systemui/egg/LLandActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Egg{

CarClass(CLLandActivity)
    , public LLandActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_EGG_CLLANDACTIVITY_H__
