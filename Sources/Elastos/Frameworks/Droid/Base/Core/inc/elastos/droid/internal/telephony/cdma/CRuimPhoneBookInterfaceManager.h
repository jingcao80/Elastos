#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CRUIMPHONEBOOKINTERFACEMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CRUIMPHONEBOOKINTERFACEMANAGER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CRuimPhoneBookInterfaceManager.h"
#include "elastos/droid/internal/telephony/cdma/RuimPhoneBookInterfaceManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CRuimPhoneBookInterfaceManager)
    , public RuimPhoneBookInterfaceManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CRUIMPHONEBOOKINTERFACEMANAGER_H__
