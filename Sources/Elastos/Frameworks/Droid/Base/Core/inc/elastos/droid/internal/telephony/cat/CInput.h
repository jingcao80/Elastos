#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CINPUT_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CINPUT_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CInput.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/internal/telephony/cat/Input.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CInput)
    , public Input
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CINPUT_H__
