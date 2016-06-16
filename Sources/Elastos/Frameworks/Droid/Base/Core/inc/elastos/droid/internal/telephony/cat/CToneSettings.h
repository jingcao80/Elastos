#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTONESETTINGS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTONESETTINGS_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CToneSettings.h"
#include "elastos/droid/internal/telephony/cat/ToneSettings.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CToneSettings)
    , public ToneSettings
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CTONESETTINGS_H__
