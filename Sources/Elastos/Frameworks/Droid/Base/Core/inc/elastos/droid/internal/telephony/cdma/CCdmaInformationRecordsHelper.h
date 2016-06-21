#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMAINFORMATIONRECORDSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMAINFORMATIONRECORDSHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaInformationRecordsHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaInformationRecordsHelper)
    , public Singleton
    , public ICdmaInformationRecordsHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI IdToString(
        /* [in] */ Int32 id,
        /* [out] */ String* result);

};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMAINFORMATIONRECORDSHELPER_H__
