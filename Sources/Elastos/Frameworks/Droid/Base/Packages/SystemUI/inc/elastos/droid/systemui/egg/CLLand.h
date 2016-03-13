#ifndef __ELASTOS_DROID_SYSTEMUI_EGG_CLLand_H__
#define __ELASTOS_DROID_SYSTEMUI_EGG_CLLand_H__

#include "_Elastos_Droid_SystemUI_Egg_CLLand.h"
#include "elastos/droid/systemui/egg/LLand.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Egg{

CarClass(CLLand)
    , public LLand
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_EGG_CLLand_H__
