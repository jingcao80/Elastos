#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTEXTATTRIBUTE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTEXTATTRIBUTE_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CTextAttribute.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/internal/telephony/cat/TextAttribute.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CTextAttribute)
    , public TextAttribute
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTEXTATTRIBUTE_H__
