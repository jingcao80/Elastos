#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICE_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCatService.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/telephony/cat/CatService.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCatService)
    , public CatService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICE_H__
