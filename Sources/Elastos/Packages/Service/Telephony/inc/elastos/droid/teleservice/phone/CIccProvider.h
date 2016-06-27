#ifndef  __ELASTOS_DROID_PHONE_CICCPROVIDER_H__
#define  __ELASTOS_DROID_PHONE_CICCPROVIDER_H__

#include "_Elastos_Droid_TeleService_Phone_CIccProvider.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/IccProvider.h"

using Elastos::Droid::Internal::Telephony::IccProvider;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * ICC address book content provider.
 */
CarClass(CIccProvider)
    , public IccProvider
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CICCPROVIDER_H__