#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCOMMANDDETAILS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCOMMANDDETAILS_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCommandDetails.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCommandDetails)
    , public CommandDetails
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCOMMANDDETAILS_H__
