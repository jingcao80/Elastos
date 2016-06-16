#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTEXTMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTEXTMESSAGE_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CTextMessage.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/internal/telephony/cat/TextMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CTextMessage)
    , public TextMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTEXTMESSAGE_H__
