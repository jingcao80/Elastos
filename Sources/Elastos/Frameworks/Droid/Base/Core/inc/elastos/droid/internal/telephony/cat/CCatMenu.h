#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATMENU_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATMENU_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCatMenu.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/CatMenu.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCatMenu)
    , public CatMenu
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATMENU_H__
