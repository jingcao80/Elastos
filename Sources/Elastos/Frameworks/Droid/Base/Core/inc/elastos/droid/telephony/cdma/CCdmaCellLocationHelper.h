
#ifndef __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATIONHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATIONHELPER_H__

#include "_Elastos_Droid_Telephony_Cdma_CCdmaCellLocationHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaCellLocationHelper)
    , public Singleton
    , public ICdmaCellLocationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ConvertQuartSecToDecDegrees(
        /* [in] */ Int32 quartSec,
        /* [out] */ Double* val);
};

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATIONHELPER_H__
