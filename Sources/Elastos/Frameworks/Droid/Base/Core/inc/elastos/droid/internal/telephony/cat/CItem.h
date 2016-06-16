#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CITEM_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CITEM_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CItem.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/internal/telephony/cat/Item.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CItem)
    , public Item
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CITEM_H__
